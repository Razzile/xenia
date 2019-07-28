#include "xenia/app/library/nxe_scanner.h"
#include "xenia/app/library/scanner_utils.h"
#include "xenia/vfs/devices/stfs_container_entry.h"

namespace xe {
namespace app {
using vfs::StfsHeader;

X_STATUS NxeScanner::ScanNxe(File* file, NxeInfo* out_info) {
  // Read Header
  uint8_t* data = Read(file);
  StfsHeader header;
  header.Read(data);

  // Read Title
  std::wstring title(header.title_name);
  out_info->game_title = xe::to_string(title);

  // Read Icon
  out_info->icon_size = header.title_thumbnail_image_size;
  out_info->icon = (uint8_t*)calloc(1, out_info->icon_size);
  memcpy(out_info->icon, header.title_thumbnail_image, out_info->icon_size);

  // TODO: Read nxebg.jpg
  // TODO: Read nxeslot.jpg
  //   How can we open the file with a StfsContainerDevice?

  delete[] data;
  return X_STATUS_SUCCESS;
}

}  // namespace app
}  // namespace xe