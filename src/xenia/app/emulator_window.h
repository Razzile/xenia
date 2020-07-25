/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2018 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_APP_MAIN_WINDOW_H_
#define XENIA_APP_MAIN_WINDOW_H_

#include <QMainWindow>
#include <QVulkanInstance>
#include <QWindow>

#include "xenia/emulator.h"
#include "xenia/ui/graphics_context.h"
#include "xenia/ui/graphics_provider.h"

namespace xe {
namespace app {

class VulkanWindow;
class VulkanRenderer;

class EmulatorWindow : public QMainWindow {
  Q_OBJECT

 public:
  EmulatorWindow();

  bool Launch(const std::wstring& path);

  xe::Emulator* emulator() { return emulator_.get(); }

 protected:
  // Events

 private slots:

 private:
  explicit EmulatorWindow(Emulator* emulator);

  bool Initialize();

  void FileDrop(const wchar_t* filename);
  void FileOpen();
  void FileClose();
  void ShowContentDirectory();
  void CheckHideCursor();
  void CpuTimeScalarReset();
  void CpuTimeScalarSetHalf();
  void CpuTimeScalarSetDouble();
  void CpuBreakIntoDebugger();
  void CpuBreakIntoHostDebugger();
  void GpuTraceFrame();
  void GpuClearCaches();
  void ShowHelpWebsite();
  void ShowCommitID();

  Emulator* emulator_;
  std::unique_ptr<ui::Loop> loop_;
  std::unique_ptr<ui::Window> window_;
  std::wstring base_title_;
  uint64_t cursor_hide_time_ = 0;
};

}  // namespace app
}  // namespace xe

#endif  // XENIA_UI_QT_MAIN_WINDOW_H_
