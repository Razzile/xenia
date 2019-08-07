#include "xenia/app/library/game_scanner.h"
#include "xenia/app/library/scanner_utils.h"
#include "xenia/base/logging.h"

#include <deque>

namespace xe {
namespace app {
using filesystem::FileInfo;
using AsyncCallback = XGameScanner::AsyncCallback;

std::vector<wstring> XGameScanner::FindGamesInPath(const wstring& path) {
  // Path is a directory, scan recursively
  // TODO: Warn about recursively scanning paths with large hierarchies

  std::deque<wstring> queue;
  queue.push_front(path);

  std::vector<wstring> paths;
  int game_count = 0;
  while (!queue.empty()) {
    wstring current_path = queue.front();
    FileInfo current_file;
    filesystem::GetInfo(current_path, &current_file);
    queue.pop_front();

    if (current_file.type == FileInfo::Type::kDirectory) {
      std::vector<FileInfo> directory_files =
          filesystem::ListFiles(current_path);
      for (FileInfo file : directory_files) {
        if (CompareCaseInsensitive(file.name, L"$SystemUpdate")) continue;

        auto next_path = xe::join_paths(current_path, file.name);
        queue.push_front(next_path);
      }
    } else {
      // Exclusively scan iso, xex, or files without an extension.
      auto extension = GetFileExtension(current_path);
      if (memcmp(&extension, L"xex", 3) && memcmp(&extension, L"iso", 3) &&
          extension.size() > 0)
        continue;

      // Do not attempt to scan SVOD data files
      auto filename = GetFileName(current_path);
      if (memcmp(filename.c_str(), L"Data", 4) == 0) continue;

      // Skip empty files
      if (current_file.total_size == 0) continue;

      paths.push_back(current_path);
      game_count++;
    }
  }
  return paths;
}

std::vector<XGameEntry> XGameScanner::ScanPath(const wstring& path) {
  std::vector<XGameEntry> games;

  // Check if the given path exists
  if (!filesystem::PathExists(path)) {
    return games;
  }

  // Scan if the given path is a file
  if (!filesystem::IsFolder(path)) {
    XGameEntry game_entry;
    if (XFAILED(ScanGame(path, &game_entry))) {
      XELOGE("Failed to scan game at %ls", path.c_str());
    } else {
      games.emplace_back(std::move(game_entry));
    }
    return games;
  }

  const std::vector<wstring>& game_paths = FindGamesInPath(path);
  for (const wstring& game_path : game_paths) {
    XGameEntry game_entry;
    if (XFAILED(ScanGame(game_path, &game_entry))) {
      continue;
    }
    games.emplace_back(std::move(game_entry));
  }

  XELOGI("Scanned %d files", games.size());
  return games;
}

int XGameScanner::ScanPathAsync(const wstring& path, const AsyncCallback& cb) {
  std::vector<wstring> paths = {path};
  return ScanPathsAsync(paths, cb);
}

int XGameScanner::ScanPathsAsync(const std::vector<wstring>& paths,
                                 const AsyncCallback& cb) {
  int count = 0;
  std::vector<wstring> game_paths;
  for (const wstring& scan_path : paths) {
    const std::vector<wstring>& scanned_games = FindGamesInPath(scan_path);
    game_paths.insert(game_paths.end(), scanned_games.begin(),
                      scanned_games.end());
  }

  std::thread scan_thread = std::thread(
      [](std::vector<wstring> paths, AsyncCallback cb) {
        thread_local int scanned = 0;
        for (const wstring& path : paths) {
          scanned++;
          if (!filesystem::PathExists(path)) {
            continue;
          }

          if (filesystem::IsFolder(path)) {
            continue;
          }

          XGameEntry game_info;
          auto status = ScanGame(path, &game_info);
          if (cb && XSUCCEEDED(status)) {
            cb(game_info, scanned);
          } else {
            // XELOGE("Failed to scan game at %ls", path.c_str());
          }
        }
      },
      game_paths, cb);

  scan_thread.detach();

  return (int)game_paths.size();
}

X_STATUS XGameScanner::ScanGame(const std::wstring& path,
                                XGameEntry* out_info) {
  if (!out_info) {
    return X_STATUS_SUCCESS;
  }

  XGameFormat format = ResolveFormat(path);
  const char* format_str;

  switch (format) {
    case XGameFormat::kIso: {
      format_str = "ISO";
      break;
    }
    case XGameFormat::kStfs: {
      format_str = "STFS";
      break;
    }
    case XGameFormat::kXex: {
      format_str = "XEX";
      break;
    }
    default: {
      format_str = "Unknown";
      break;
    }
  }

  XELOGD("Scanning %s", xe::to_string(path).c_str());

  auto device = CreateDevice(path);
  if (device == nullptr || !device->Initialize()) {
    return X_STATUS_UNSUCCESSFUL;
  }

  XELOGI("Format is %s", format_str);

  GameInfo game_info;
  game_info.filename = GetFileName(path);
  game_info.path = xe::fix_path_separators(path);
  game_info.format = format;

  // Read XEX
  auto xex_entry = device->ResolvePath("default.xex");
  if (xex_entry) {
    File* xex_file = nullptr;
    auto status = xex_entry->Open(vfs::FileAccess::kFileReadData, &xex_file);
    if (XSUCCEEDED(status)) {
      status = XexScanner::ScanXex(xex_file, &game_info);
      if (!XSUCCEEDED(status)) {
        XELOGE("Could not parse xex file: %s",
               xex_file->entry()->path().c_str());
        return status;
      }
    } else {
      XELOGE("Could not load default.xex from device: %x", status);
      return X_STATUS_UNSUCCESSFUL;
    }

    xex_file->Destroy();
  } else {
    XELOGE("Could not resolve default.xex");
    return X_STATUS_UNSUCCESSFUL;
  }

  // Read NXE
  auto nxe_entry = device->ResolvePath("nxeart");
  if (nxe_entry) {
    File* nxe_file = nullptr;
    auto status = nxe_entry->Open(vfs::FileAccess::kFileReadData, &nxe_file);
    if (XSUCCEEDED(status)) {
      status = NxeScanner::ScanNxe(nxe_file, &game_info);
      if (!XSUCCEEDED(status)) {
        XELOGE("Could not parse nxeart file: %s",
               nxe_file->entry()->path().c_str());
        return status;
      }
    } else {
      XELOGE("Could not load nxeart from device: %x", status);
    }

    nxe_file->Destroy();
    return X_STATUS_UNSUCCESSFUL;
  } else {
    XELOGI("Game does not have an nxeart file");
  }

  out_info->apply_info(game_info);

  return X_STATUS_SUCCESS;
}

}  // namespace app
}  // namespace xe