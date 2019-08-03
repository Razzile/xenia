#ifndef XENIA_APP_GAME_LIBRARY_H_
#define XENIA_APP_GAME_LIBRARY_H_

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "xenia/app/library/game_entry.h"
#include "xenia/app/library/scanner_utils.h"

namespace xe {
namespace app {

class XGameLibrary {
 public:
  XGameLibrary(XGameLibrary const&) = delete;
  XGameLibrary& operator=(XGameLibrary const&) = delete;
  static XGameLibrary* Instance();

  bool add_path(const std::wstring& path);
  bool remove_path(const std::wstring& path);
  void scan_paths();

  const XGameEntry* game(const uint32_t title_id) const;
  const std::vector<XGameEntry>& games() const { return games_; }
  const size_t size() const { return games_.size(); }

  void clear() { games_.clear(); }

  // bool load();
  // bool save();

 private:
  XGameLibrary() = default;

  std::vector<XGameEntry> games_;
  std::vector<std::wstring> paths_;
};

}  // namespace app
}  // namespace xe

#endif