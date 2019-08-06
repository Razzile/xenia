#ifndef XENIA_APP_GAME_SCANNER_H_
#define XENIA_APP_GAME_SCANNER_H_

#include <functional>
#include <thread>
#include <vector>
#include "xenia/app/library/game_entry.h"
#include "xenia/app/library/nxe_scanner.h"
#include "xenia/app/library/scanner_utils.h"
#include "xenia/app/library/xex_scanner.h"

namespace xe {
namespace app {
class XGameLibrary;

class XGameScanner {
 public:
  using AsyncCallback = std::function<void(const XGameEntry&)>;

  ~XGameScanner() {
    if (scan_thread_.joinable()) {
      scan_thread_.join();
    }
  }

  // Returns a vector of all supported games in provided path.
  std::vector<wstring> FindGamesInPath(const wstring& path);

  // Scans a provided path and recursively parses the games.
  // Returns a vector of parsed game entries.
  std::vector<XGameEntry> ScanPath(const wstring& path);

  // Scans a provided path and recursively parses the games asynchronously.
  // The callback provided is called on each successfully parsed game.
  // Returns the number of games found in the path.
  int ScanPathAsync(const wstring& path, const AsyncCallback& cb = nullptr);

  // Scans a list of provided paths and recursively parses the games
  // asynchronously. The callback provided is called on each successfully parsed
  // game. Returns the number of games found in all paths provided.
  int ScanPathsAsync(const std::vector<wstring>& paths,
                     const AsyncCallback& cb = nullptr);

  // Scans a path for a single game, populating the provided output game entry
  // if found and parsed successfully.
  X_STATUS ScanGame(const wstring& path, XGameEntry* out_info);

 private:
  std::thread scan_thread_;
};

}  // namespace app
}  // namespace xe

#endif