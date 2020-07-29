/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/app/library/game_library.h"
//#include "xenia/base/debugging.h"
//#include "xenia/base/logging.h"
#include "xenia/base/cvar.h"
#include "xenia/base/main.h"
//#include "xenia/base/platform.h"
//#include "xenia/base/profiling.h"
//#include "xenia/base/threading.h"

#include "xenia/app/emulator_window.h"
#include "xenia/ui/qt/main_window.h"

#include <QApplication>
#include <QFontDatabase>
#include <QtPlugin>

#include "xenia/base/logging.h"
#include "xenia/config.h"
#include "xenia/ui/qt/qt_loop.h"

#if XE_PLATFORM_WIN32 && QT_STATIC
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#endif

DEFINE_bool(mount_scratch, false, "Enable scratch mount", "General");
DEFINE_bool(mount_cache, false, "Enable cache mount", "General");
DEFINE_bool(show_debug_tab, true, "Show the debug tab in the Qt UI", "General");
DEFINE_string(
    storage_root, "",
    "Root path for persistent internal data storage (config, etc.), or empty "
    "to use the path preferred for the OS, such as the documents folder, or "
    "the emulator executable directory if portable.txt is present in it.",
    "Storage");
DEFINE_string(
    content_root, "",
    "Root path for guest content storage (saves, etc.), or empty to use the "
    "content folder under the storage root.",
    "Storage");

namespace xe {
namespace app {

int xenia_main(const std::vector<std::wstring>& args) {
  /*Profiler::Initialize();
  Profiler::ThreadEnter("main");*/

  // auto emulator = std::make_unique<xe::Emulator>(L"");

  qputenv("QT_SCALE_FACTOR", "0.75");

  // Start Qt
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  QCoreApplication::setApplicationName("Xenia");
  QCoreApplication::setOrganizationName(
      "Xenia Xbox 360 Emulator Research Project");
  QCoreApplication::setOrganizationDomain("https://xenia.jp");

  std::wstring storage_root = xe::to_wstring(cvars::storage_root);
  if (storage_root.empty()) {
    storage_root = xe::filesystem::GetExecutableFolder();
    if (!xe::filesystem::PathExists(xe::format_string(L"%S/%S", storage_root.c_str(), "portable.txt"))) {
      storage_root = xe::filesystem::GetUserFolder();
#if defined(XE_PLATFORM_WIN32) || defined(XE_PLATFORM_LINUX)
      storage_root = storage_root + L"/Xenia";
#else
#warning Unhandled platform for the data root.
      storage_root = storage_root + L"/Xenia";
#endif
    }
  }
  storage_root = xe::fix_path_separators(storage_root);
  XELOGI("Storage root: %S", storage_root.c_str());

  config::SetupConfig(storage_root);

  int argc = 1;
  char* argv[] = {"xenia", nullptr};
  QApplication app(argc, argv);

  // Load Fonts
  QFontDatabase fonts;
  fonts.addApplicationFont(":/resources/fonts/SegMDL2.ttf");
  fonts.addApplicationFont(":/resources/fonts/segoeui.ttf");
  fonts.addApplicationFont(":/resources/fonts/segoeuisb.ttf");
  QApplication::setFont(QFont("Segoe UI"));

  // EmulatorWindow main_wnd;
  ui::qt::QtLoop loop;
  auto main_wnd = new ui::qt::MainWindow(&loop, L"Xenia Qt");
  main_wnd->Initialize();
  main_wnd->SetIcon(QIcon(":/resources/graphics/icon.ico"));
  main_wnd->Resize(1280, 720);

  /*
  if (FLAGS_mount_scratch) {
    auto scratch_device = std::make_unique<xe::vfs::HostPathDevice>(
        "\\SCRATCH", L"scratch", false);
    if (!scratch_device->Initialize()) {
      XELOGE("Unable to scan scratch path");
    } else {
      if (!emulator->file_system()->RegisterDevice(std::move(scratch_device))) {
        XELOGE("Unable to register scratch path");
      } else {
        emulator->file_system()->RegisterSymbolicLink("scratch:", "\\SCRATCH");
      }
    }
  }





  if (FLAGS_mount_cache) {
    auto cache0_device =
        std::make_unique<xe::vfs::HostPathDevice>("\\CACHE0", L"cache0", false);
    if (!cache0_device->Initialize()) {
      XELOGE("Unable to scan cache0 path");
    } else {
      if (!emulator->file_system()->RegisterDevice(std::move(cache0_device))) {
        XELOGE("Unable to register cache0 path");
      } else {
        emulator->file_system()->RegisterSymbolicLink("cache0:", "\\CACHE0");
      }
    }

    auto cache1_device =
        std::make_unique<xe::vfs::HostPathDevice>("\\CACHE1", L"cache1", false);
    if (!cache1_device->Initialize()) {
      XELOGE("Unable to scan cache1 path");
    } else {
      if (!emulator->file_system()->RegisterDevice(std::move(cache1_device))) {
        XELOGE("Unable to register cache1 path");
      } else {
        emulator->file_system()->RegisterSymbolicLink("cache1:", "\\CACHE1");
      }
    }
  }

  // Set a debug handler.
  // This will respond to debugging requests so we can open the debug UI.
  std::unique_ptr<xe::debug::ui::DebugWindow> debug_window;
  if (FLAGS_debug) {
    emulator->processor()->set_debug_listener_request_handler(
        [&](xe::cpu::Processor* processor) {
          if (debug_window) {
            return debug_window.get();
          }
          emulator_window->loop()->PostSynchronous([&]() {
            debug_window = xe::debug::ui::DebugWindow::Create(
                emulator.get(), emulator_window->loop());
            debug_window->window()->on_closed.AddListener(
                [&](xe::ui::UIEvent* e) {
                  emulator->processor()->set_debug_listener(nullptr);
                  emulator_window->loop()->Post(
                      [&]() { debug_window.reset(); });
                });
          });
          return debug_window.get();
        });
  }
  */

  // if (args.size() >= 2) {
  //  // Launch the path passed in args[1].
  //  main_wnd.Launch(args[1]);
  //}

  int rc = app.exec();

  /*Profiler::Dump();
  Profiler::Shutdown();*/
  return rc;
}

}  // namespace app
}  // namespace xe

DEFINE_ENTRY_POINT(L"xenia", L"xenia some.xex", xe::app::xenia_main);
