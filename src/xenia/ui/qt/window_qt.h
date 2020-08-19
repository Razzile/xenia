/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2020 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_UI_QT_WINDOW_QT_H_
#define XENIA_UI_QT_WINDOW_QT_H_

#include <QMainWindow>

#include "xenia/emulator.h"
#include "xenia/ui/window.h"

class QWindowStateChangeEvent;

namespace xe {
namespace ui {
namespace qt {

class QtWindow : public QMainWindow, public ui::Window {
  Q_OBJECT
 public:
  QtWindow(Loop* loop, const std::wstring& title);
  NativePlatformHandle native_platform_handle() const override;
  NativeWindowHandle native_handle() const override;


  Menu* CreateMenu() override;
  void EnableMainMenu() override;
  void DisableMainMenu() override;

  bool set_title(const std::wstring& title) override;
  bool SetIcon(const void* buffer, size_t size) override;
  virtual bool SetIcon(const QIcon& icon);

  bool is_fullscreen() const override;
  void ToggleFullscreen(bool fullscreen) override;

  bool is_bordered() const override;
  void set_bordered(bool enabled) override;

  int get_dpi() const override;

  void set_focus(bool value) override;
  void set_cursor_visible(bool value) override;

  void Resize(int32_t width, int32_t height) override;
  void Resize(int32_t left, int32_t top, int32_t right,
              int32_t bottom) override;

  bool Initialize() override;
  void Close() override;

 protected:
  bool MakeReady() override;

  void UpdateWindow();

  void HandleWindowStateChange(QWindowStateChangeEvent* ev);
  void HandleWindowScreenChange(QScreen* screen);

  void HandleKeyPress(QKeyEvent* ev);
  void HandleKeyRelease(QKeyEvent* ev);
  void HandleMouseMove(QMouseEvent* ev);
  void HandleMouseClick(QMouseEvent* ev, bool release);

  void OnResize(UIEvent* e) override;

  bool event(QEvent* event) override;
  void changeEvent(QEvent*) override;
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dropEvent(QDropEvent* event) override;

 private:
  bool main_menu_enabled_;
  QPoint last_mouse_pos_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif  // XENIA_UI_QT_WINDOW_H_
