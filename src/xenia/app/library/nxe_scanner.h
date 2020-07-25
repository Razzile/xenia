#ifndef XENIA_APP_NXE_SCANNER_H_
#define XENIA_APP_NXE_SCANNER_H_

#include "xenia/app/library/scanner_utils.h"
#include "xenia/vfs/file.h"

namespace xe {
namespace app {

using vfs::File;

class NxeScanner {
 public:
  static X_STATUS ScanNxe(File* file, GameInfo* out_info);
};

}  // namespace app
}  // namespace xe

#endif