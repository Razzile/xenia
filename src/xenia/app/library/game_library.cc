#include "xenia/app/library/game_library.h"

#include <algorithm>

namespace xe {
namespace app {

XGameLibrary* XGameLibrary::Instance() {
  static XGameLibrary* instance = new XGameLibrary;
  return instance;
}

bool XGameLibrary::add(const std::string file_path) {
  // TODO
  return false;
}

bool XGameLibrary::add(XGameEntry* game_entry) {
   if (!game_entry->is_valid()) {
    return false;  // Game is not valid
  }

  // if (games_.count(game_entry->title_id())) {
  //  return false;  // Game is already present
  //}

  auto ptr = std::unique_ptr<XGameEntry>(game_entry);
  games_.push_back(std::move(ptr));

  {
    const auto& game = games_.back();
    auto pair = std::make_pair(game->title_id(), game.get());
    games_titleid_map_.insert(pair);
  }

  return true;
}

bool XGameLibrary::remove(const XGameEntry* game_entry) {
  // if (!games_.count(game_entry->title_id())) {
  //  return false;  // Game is not present
  //}

  // games_.erase(game_entry->title_id());
  return true;
}

bool XGameLibrary::rescan_game(const XGameEntry* game_entry) {
  // TODO
  return false;
}

bool XGameLibrary::add_path(const std::string& path) {
  auto entry = std::find(game_paths_.begin(), game_paths_.end(), path);

  if (entry != game_paths_.end()) {
    return false;  // Path already present
  }

  game_paths_.push_back(path);
  return true;
}

bool XGameLibrary::remove_path(const std::string& path) {
  auto entry = std::find(game_paths_.begin(), game_paths_.end(), path);

  if (entry == game_paths_.end()) {
    return false;  // Path not present
  }

  game_paths_.erase(entry);
  return true;
}

bool XGameLibrary::scan_game_paths() {
  // TODO
  return false;
}

bool XGameLibrary::load() {
  // TODO
  return false;
}

bool XGameLibrary::save() {
  // TODO
  return false;
}

const XGameEntry* XGameLibrary::game(const uint32_t& title_id) const {
  auto game = games_titleid_map_.find(title_id);
  if (game == games_titleid_map_.end()) {
    return nullptr;
  }

  return game->second;
}

}  // namespace app
}  // namespace xe