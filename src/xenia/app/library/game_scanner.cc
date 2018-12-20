#include "xenia/app/library/game_scanner.h"
#include "xenia/app/library/scanner_utils.h"

#include <deque>

namespace xe {
namespace app {
using filesystem::FileInfo;

const vector<const GameInfo*> XGameScanner::ScanPath(const wstring& path) {
  vector<const GameInfo*> info;

  // Check if the given path exists
  if (!filesystem::PathExists(path)) {
    return info;
  }

  // Scan if the given path is a file
  if (!filesystem::IsFolder(path)) {
    const GameInfo* game_info = ScanGame(path);
    info.push_back(game_info);
    return info;
  }

  // Path is a directory, scan recursively
  // TODO: Warn about recursively scanning paths with large hierarchies
  std::deque<wstring> queue;
  queue.push_front(path);

  while (!queue.empty()) {
    wstring current_path = queue.front();
    FileInfo current_file;
    filesystem::GetInfo(current_path, &current_file);
    queue.pop_front();

    if (current_file.type == FileInfo::Type::kDirectory) {
      vector<FileInfo> directory_files = filesystem::ListFiles(current_path);
      for (FileInfo file : directory_files) {
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

      const GameInfo* game_info = ScanGame(current_path);
      info.push_back(game_info);
    }
  }

  return info;
}

const GameInfo* XGameScanner::ScanGame(const std::wstring& path) {
  GameInfo* info = new GameInfo;
  info->filename = GetFileName(path);
  info->path = path;
  info->format = ResolveFormat(path);

  auto device = CreateDevice(path);
  if (device == nullptr || !device->Initialize()) {
    return info;  // TODO
  }

  // Read XEX
  auto xex_entry = device->ResolvePath("default.xex");
  if (xex_entry) {
    File* xex_file = nullptr;
    auto status = xex_entry->Open(vfs::FileAccess::kFileReadData, &xex_file);
    if (XSUCCEEDED(status)) {
      info->xex_info = XexScanner::ScanXex(xex_file);
    }

    xex_file->Destroy();
  }

  // Read NXE
  auto nxe_entry = device->ResolvePath("nxeart");
  if (nxe_entry) {
    File* nxe_file = nullptr;
    auto status = nxe_entry->Open(vfs::FileAccess::kFileReadData, &nxe_file);
    if (XSUCCEEDED(status)) {
      info->nxe_info = NxeScanner::ScanNxe(nxe_file);
    }

    nxe_file->Destroy();
  }

  delete device;
  return info;
}

}  // namespace app
}  // namespace xe