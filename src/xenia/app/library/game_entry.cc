#include "xenia/app/library/game_entry.h"

namespace xe {
namespace app {

XGameEntry* XGameEntry::from_game_info(const GameInfo& info) {
  auto entry = new XGameEntry();
  auto result = entry->apply_info(info);

  if (!result) return nullptr;
  return entry;
};

XGameEntry::XGameEntry() {
  format_ = XGameFormat::kUnknown;
  ratings_ = xex2_game_ratings_t();
  version_.value = 0x0;
  base_version_.value = 0x0;
  regions_ = XGameRegions::XEX_REGION_ALL;
}

XGameEntry::XGameEntry(const XGameEntry& other) {
  format_ = other.format_;
  file_path_ = other.file_path_;
  file_name_ = other.file_name_;
  launch_paths_ = other.launch_paths_;
  default_launch_paths_ = other.default_launch_paths_;
  title_ = other.title_;
  icon_ = other.icon_;
  icon_size_ = other.icon_size_;
  title_id_ = other.title_id_;
  media_id_ = other.media_id_;
  alt_title_ids_ = other.alt_title_ids_;
  alt_media_ids_ = other.alt_media_ids_;
  disc_map_ = other.disc_map_;
  version_ = other.version_;
  base_version_ = other.base_version_;
  ratings_ = other.ratings_;
  regions_ = other.regions_;
  build_date_ = other.build_date_;
  genre_ = other.genre_;
  release_date_ = other.release_date_;
  player_count_ = other.player_count_;
}

bool XGameEntry::is_valid() {
  // Minimum requirements
  return file_path_.length() && title_id_ && media_id_;
}

bool XGameEntry::is_missing_data() {
  return title_.length() == 0 || icon_[0] == 0 || disc_map_.size() == 0;
  // TODO: Version
  // TODO: Base Version
  // TODO: Ratings
  // TODO: Regions
}

bool XGameEntry::apply_info(const GameInfo& info) {
  auto xex = &info.xex_info;
  auto nxe = &info.nxe_info;

  format_ = info.format;
  file_path_ = info.path;
  file_name_ = info.filename;

  if (!xex) return false;

  title_id_ = xex->execution_info.title_id;
  media_id_ = xex->execution_info.media_id;
  version_ = xex->execution_info.version.value;
  base_version_ = xex->execution_info.base_version.value;
  ratings_ = xex->game_ratings;
  regions_ = (xex2_region_flags)xe::byte_swap<uint32_t>(
      xex->security_info.region.value);

  // Add to disc map / launch paths
  auto disc_id = xex->execution_info.disc_number;
  disc_map_.insert_or_assign(disc_id, media_id_);
  launch_paths_.insert_or_assign(info.path, media_id_);
  if (!default_launch_paths_.count(media_id_)) {
    default_launch_paths_.insert(std::make_pair(media_id_, info.path));
  }

  if (xex->game_title.length() > 0) {
    title_ = xex->game_title;
  } else if (nxe && nxe->game_title.length() > 0) {
    title_ = nxe->game_title;
  }

  if (xex->icon) {
    delete[] icon_;
    icon_size_ = xex->icon_size;
    icon_ = (uint8_t*)calloc(1, icon_size_);
    memcpy(icon_, xex->icon, icon_size_);
  } else if (nxe && nxe->icon) {
    delete[] icon_;
    icon_size_ = nxe->icon_size;
    icon_ = (uint8_t*)calloc(1, icon_size_);
    memcpy(icon_, nxe->icon, icon_size_);
  }

  return true;
}

}  // namespace app
}  // namespace xe