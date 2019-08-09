#include "xenia/app/library/game_library.h"
#include <algorithm>
#include "xenia/app/library/game_scanner.h"

namespace xe {
namespace app {
using AsyncCallback = XGameLibrary::AsyncCallback;

XGameLibrary* XGameLibrary::Instance() {
  static XGameLibrary* instance = new XGameLibrary;
  return instance;
}

bool XGameLibrary::ContainsPath(const std::wstring& path) const {
  auto existing = std::find(paths_.begin(), paths_.end(), path);
  if (existing != paths_.end()) {
    return false;  // Path already exists.
  }
  return true;
}

bool XGameLibrary::ContainsGame(uint32_t title_id) const {
  return FindGame(title_id) != nullptr;
}

const XGameEntry* XGameLibrary::FindGame(const uint32_t title_id) const {
  auto result = std::find_if(games_.begin(), games_.end(),
                             [title_id](const XGameEntry& entry) {
                               return entry.title_id() == title_id;
                             });
  if (result != games_.end()) {
    return &*result;
  }
  return nullptr;
}

bool XGameLibrary::RemovePath(const std::wstring& path) {
  auto existing = std::find(paths_.begin(), paths_.end(), path);
  if (existing == paths_.end()) {
    return false;  // Path does not exist.
  }

  paths_.erase(existing);
  return true;
}

int XGameLibrary::ScanPath(const wstring& path) {
  int count = 0;

  AddPath(path);
  const auto& results = XGameScanner::ScanPath(path);
  for (const XGameEntry& result : results) {
    count++;
    AddGame(result);
  }
  return count;
}

int XGameLibrary::ScanPathAsync(const wstring& path, const AsyncCallback& cb) {
  AddPath(path);

  auto paths = XGameScanner::FindGamesInPath(path);
  int count = static_cast<int>(paths.size());
  return XGameScanner::ScanPathsAsync(
      paths, [=](const XGameEntry& entry, int scanned) {
        AddGame(entry);
        if (cb) {
          cb(((double)scanned / count) * 100.0, entry);
        }
      });
}

void XGameLibrary::AddGame(const XGameEntry& game) {
  std::lock_guard<std::mutex> lock(mutex_);

  uint32_t title_id = game.title_id();

  const auto& begin = games_.begin();
  const auto& end = games_.end();

  auto result = end;
  if (title_id != 0x00000000) {
    result = std::find_if(begin, end, [title_id](const XGameEntry& entry) {
      return entry.title_id() == title_id;
    });
  }

  // title already exists in library, overwrite existing
  if (result != games_.end()) {
    *result = game;
  } else {
    games_.push_back(game);
  }
}

void XGameLibrary::AddPath(const wstring& path) {
  auto result = std::find(paths_.begin(), paths_.end(), path);

  // only save unique paths
  if (result == paths_.end()) {
    paths_.push_back(path);
  }
}

}  // namespace app
}  // namespace xe
