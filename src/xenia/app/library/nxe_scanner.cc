#include "xenia/app/library/nxe_scanner.h"
#include "xenia/app/library/scanner_utils.h"
#include "xenia/vfs/devices/stfs_container_entry.h"

namespace xe {
namespace app {
using vfs::StfsHeader;

X_STATUS NxeScanner::ScanNxe(File* file, GameInfo* out_info) {
  NxeInfo* nxe_info = &out_info->nxe_info;
  // Read Header
  size_t file_size = file->entry()->size();
  uint8_t* data = new uint8_t[file_size];

  Read(file, &data[0]);
  StfsHeader header;
  header.Read(data);

  // Read Title
  std::wstring title(header.title_name);
  nxe_info->game_title = xe::to_string(title);

  // Read Icon
  nxe_info->icon_size = header.title_thumbnail_image_size;
  nxe_info->icon = (uint8_t*)calloc(1, nxe_info->icon_size);
  memcpy(nxe_info->icon, header.title_thumbnail_image, nxe_info->icon_size);

  // TODO: Read nxebg.jpg
  // TODO: Read nxeslot.jpg
  //   How can we open the file with a StfsContainerDevice?

  delete[] data;

  return X_STATUS_SUCCESS;
}

}  // namespace app
}  // namespace xe