#ifndef XENIA_APP_NXE_SCANNER_H_
#define XENIA_APP_NXE_SCANNER_H_

#include "xenia/vfs/file.h"

namespace xe {
namespace app {

using vfs::File;

struct NxeInfo {
  std::string game_title;
  uint8_t* icon = nullptr;
  size_t icon_size;
  uint8_t* nxe_background_image = nullptr;  // TODO
  size_t nxe_background_image_size;         // TODO
  uint8_t* nxe_slot_image = nullptr;        // TODO
  size_t nxe_slot_image_size;               // TODO

  NxeInfo() = default;
  NxeInfo(const NxeInfo& other) = delete;
  NxeInfo& operator=(const NxeInfo& other) = delete;
  ~NxeInfo() {
    delete[] icon;
    delete[] nxe_background_image;
    delete[] nxe_slot_image;
  }
};

class NxeScanner {
 public:
  static X_STATUS ScanNxe(File* file, NxeInfo* out_info);
};

}  // namespace app
}  // namespace xe

#endif