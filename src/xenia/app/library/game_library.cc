#include "xenia/app/library/game_library.h"
#include <algorithm>
#include "xenia/app/library/game_scanner.h"

namespace xe {
namespace app {

XGameLibrary* XGameLibrary::Instance() {
  static XGameLibrary* instance = new XGameLibrary;
  return instance;
}

bool XGameLibrary::add_path(const std::wstring& path) {
  auto existing = std::find(paths_.begin(), paths_.end(), path);
  if (existing != paths_.end()) {
    return false;  // Path already exists.
  }

  paths_.push_back(path);
  return true;
}

bool XGameLibrary::remove_path(const std::wstring& path) {
  auto existing = std::find(paths_.begin(), paths_.end(), path);
  if (existing == paths_.end()) {
    return false;  // Path does not exist.
  }

  paths_.erase(existing);
  return true;
}

void XGameLibrary::scan_paths() {
  clear();

  for (auto path : paths_) {
    const auto& results = XGameScanner::ScanPath(path);
    for (const XGameEntry& result : results) {
      uint32_t title_id = result.title_id();

      const auto& begin = games_.begin();
      const auto& end = games_.end();

      // title already exists in library
      if (std::find_if(begin, end, [title_id](const XGameEntry& entry) -> bool {
            return entry.title_id() == title_id;
          }) != games_.end()) {
        continue;
      } else {
        games_.push_back(result);
      }
    }
  }
}

const XGameEntry* XGameLibrary::game(const uint32_t title_id) const {
  auto result = std::find_if(games_.begin(), games_.end(),
                             [title_id](const XGameEntry& entry) {
                               return entry.title_id() == title_id;
                             });
  if (result != games_.end()) {
    return &*result;
  }
  return nullptr;
}

}  // namespace app
}  // namespace xe
