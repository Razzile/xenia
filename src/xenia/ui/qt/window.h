/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2020 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include <QMainWindow>

#include "xenia/emulator.h"
#include "xenia/ui/window.h"

namespace xe {
namespace ui {
namespace qt {

class QtWindow : public ui::Window {
 public:
  QtWindow(Loop* loop, const std::wstring& title);
  NativePlatformHandle native_platform_handle() const override;
  NativeWindowHandle native_handle() const override;

  void EnableMainMenu() override;
  void DisableMainMenu() override;

  bool set_title(const std::wstring& title) override;
  bool SetIcon(const void* buffer, size_t size) override;
  virtual bool SetIcon(const QIcon& icon);

  bool is_fullscreen() const override;
  void ToggleFullscreen(bool fullscreen) override;

  int get_dpi() const override;

  void set_focus(bool value) override;

  void Resize(int32_t width, int32_t height) override;
  void Resize(int32_t left, int32_t top, int32_t right,
              int32_t bottom) override;

  bool Initialize() override;
  void Close() override;

  QWidget* window() const { return window_; }

 protected:
  bool MakeReady() override;

  void UpdateWindow();

  QWindow* window_handle() const { return window_->windowHandle(); }

  QMainWindow* window_;

 private:
  bool main_menu_enabled_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe