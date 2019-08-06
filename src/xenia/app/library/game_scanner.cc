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
    ScanGame(path, &game_entry);
    games.emplace_back(std::move(game_entry));
    return games;
  }

  const std::vector<wstring>& game_paths = FindGamesInPath(path);
  for (const wstring& game_path : game_paths) {
    XGameEntry game_entry;
    ScanGame(game_path, &game_entry);
    games.emplace_back(std::move(game_entry));
  }

  XELOGI("Scanned %d files", games.size());
  return games;
}

int XGameScanner::ScanPathAsync(const wstring& path, const AsyncCallback& cb) {
  if (!filesystem::PathExists(path)) {
    return 0;
  }

  if (!filesystem::IsFolder(path)) {
    XGameEntry game_info;
    ScanGame(path, &game_info);
    if (cb) {
      cb(game_info);
      return 1;
    }
  }
  const std::vector<wstring>& game_paths = FindGamesInPath(path);

  std::thread scan_thread = std::thread(
      [this](std::vector<wstring> paths, AsyncCallback cb) {
        for (const wstring& path : paths) {
          XGameEntry game_info;
          ScanGame(path, &game_info);
          if (cb) {
            cb(game_info);
          }
        }
      },
      game_paths, cb);
  scan_thread.detach();

  return (int)game_paths.size();
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

  scan_thread_ = std::thread(
      [this](std::vector<wstring> paths, AsyncCallback cb) {
        for (const wstring& path : paths) {
          XGameEntry game_info;
          ScanGame(path, &game_info);
          if (cb) {
            cb(game_info);
          }
        }
      },
      game_paths, cb);

  scan_thread_.detach();

  return (int)game_paths.size();
}

X_STATUS XGameScanner::ScanGame(const std::wstring& path,
                                XGameEntry* out_info) {
  if (!out_info) {
    return X_STATUS_SUCCESS;
  }

  GameInfo game_info;
  game_info.filename = GetFileName(path);
  game_info.path = xe::fix_path_separators(path);
  game_info.format = ResolveFormat(path);

  XELOGI("==================================================================");
  XGameFormat format = out_info->format();
  std::string format_str;

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

  XELOGI("Scanning %s", xe::to_string(path).c_str());
  XELOGI("Format is %s", format_str);

  auto device = CreateDevice(path);
  if (device == nullptr || !device->Initialize()) {
    XELOGE("Could not create a device");
    return X_STATUS_UNSUCCESSFUL;
  }

  // Read XEX
  auto xex_entry = device->ResolvePath("default.xex");
  if (xex_entry) {
    File* xex_file = nullptr;
    auto status = xex_entry->Open(vfs::FileAccess::kFileReadData, &xex_file);
    if (XSUCCEEDED(status)) {
      XexScanner::ScanXex(xex_file, &game_info);
    } else {
      XELOGE("Could not load default.xex from device: %x", status);
    }

    xex_file->Destroy();
  } else {
    XELOGE("Could not resolve default.xex");
  }

  // Read NXE
  auto nxe_entry = device->ResolvePath("nxeart");
  if (nxe_entry) {
    File* nxe_file = nullptr;
    auto status = nxe_entry->Open(vfs::FileAccess::kFileReadData, &nxe_file);
    if (XSUCCEEDED(status)) {
      NxeScanner::ScanNxe(nxe_file, &game_info.nxe_info);
    } else {
      XELOGE("Could not load nxeart from device: %x", status);
    }

    nxe_file->Destroy();
  } else {
    XELOGI("Game does not have an nxeart file");
  }

  out_info->apply_info(game_info);

  delete device;
  return X_STATUS_SUCCESS;
}

}  // namespace app
}  // namespace xe