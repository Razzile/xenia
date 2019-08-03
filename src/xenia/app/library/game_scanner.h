#ifndef XENIA_APP_GAME_SCANNER_H_
#define XENIA_APP_GAME_SCANNER_H_

#include <vector>
#include "xenia/app/library/game_entry.h"
#include "xenia/app/library/nxe_scanner.h"
#include "xenia/app/library/scanner_utils.h"
#include "xenia/app/library/xex_scanner.h"

namespace xe {
namespace app {
using std::vector;

class XGameScanner {
 public:
  static const vector<XGameEntry> ScanPath(const wstring& path);
  static X_STATUS ScanGame(const wstring& path, XGameEntry* out_info);

 private:
};

}  // namespace app
}  // namespace xe

#endif