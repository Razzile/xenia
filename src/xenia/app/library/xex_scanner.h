#ifndef XENIA_APP_XEX_SCANNER_H_
#define XENIA_APP_XEX_SCANNER_H_

#include "xenia/app/library/game_entry.h"
#include "xenia/app/library/scanner_utils.h"
#include "xenia/kernel/util/xdbf_utils.h"
#include "xenia/kernel/util/xex2_info.h"
#include "xenia/vfs/file.h"

#include <string>

namespace xe {
namespace app {

using kernel::util::XdbfGameData;
using vfs::File;

static const uint8_t xex2_retail_key[16] = {0x20, 0xB1, 0x85, 0xA5, 0x9D, 0x28,
                                            0xFD, 0xC3, 0x40, 0x58, 0x3F, 0xBB,
                                            0x08, 0x96, 0xBF, 0x91};
static const uint8_t xex2_devkit_key[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};

class XexScanner {
 public:
  static X_STATUS ScanXex(File* xex_file, GameInfo* out_info);
};

}  // namespace app
}  // namespace xe

#endif