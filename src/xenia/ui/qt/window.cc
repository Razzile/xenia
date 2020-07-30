/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2020 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "window.h"

#include <QApplication>
#include <QKeyEvent>
#include <QMenuBar>
#include <QScreen>
#include <QWindow>

namespace xe {
namespace ui {
namespace qt {

QtWindow::QtWindow(Loop* loop, const std::wstring& title)
    : Window(loop, title), window_(nullptr), main_menu_enabled_(false) {}

NativePlatformHandle QtWindow::native_platform_handle() const {
  return nullptr;
}

NativeWindowHandle QtWindow::native_handle() const {
  return reinterpret_cast<NativeWindowHandle>(window_->winId());
}

void QtWindow::EnableMainMenu() {
  main_menu_enabled_ = true;
  UpdateWindow();
}

void QtWindow::DisableMainMenu() {
  main_menu_enabled_ = false;
  UpdateWindow();
}

bool QtWindow::set_title(const std::wstring& title) {
  window_handle()->setTitle(QString::fromWCharArray(title.c_str()));
  return true;
}

bool QtWindow::SetIcon(const void* buffer, size_t size) {
  auto pixmap = QPixmap();
  if (pixmap.loadFromData(static_cast<const uint8_t*>(buffer),
                          static_cast<uint32_t>(size))) {
    window_handle()->setIcon(pixmap);
    return true;
  }
  return false;
}

bool QtWindow::SetIcon(const QIcon& icon) {
  window_->setWindowIcon(icon);
  return true;
}

bool QtWindow::is_fullscreen() const {
  return window_handle()->windowStates() & Qt::WindowFullScreen;
}

void QtWindow::ToggleFullscreen(bool fullscreen) {
  if (is_fullscreen() == fullscreen) {
    return;
  }

  auto win_states = window_handle()->windowStates();
  win_states ^= Qt::WindowFullScreen;
  window_handle()->setWindowStates(win_states);
}

int QtWindow::get_dpi() const {
  return window_handle()->screen()->logicalDotsPerInch();
}

void QtWindow::set_focus(bool value) {
  if (value) {
    QApplication::setActiveWindow(window_);
  } else {
    // TODO: test this works with actual windows
    window_->clearFocus();
  }
}

void QtWindow::Resize(int32_t width, int32_t height) {
  window_->resize(width, height);
}

void QtWindow::Resize(int32_t left, int32_t top, int32_t right,
                      int32_t bottom) {
  window_->resize(right - left, bottom - top);
}

bool QtWindow::Initialize() {
  if (!window_) {
    window_ = new QMainWindow();
  }
  if (MakeReady()) {
    window_->show();
    return true;
  }
  return false;
}

void QtWindow::Close() { window_->close(); }

bool QtWindow::MakeReady() {
  window_->resize(width(), height());
  // TODO: Apply menu item to menu bar here.

  return true;
}

void QtWindow::UpdateWindow() {
  window_->menuBar()->setEnabled(main_menu_enabled_);
}

void QtWindow::HandleKeyPress(QKeyEvent* ev) {
  const auto& modifiers = ev->modifiers();

#ifdef Q_OS_MACOS
  auto ctrl_mod = modifiers & Qt::MetaModifier;
  auto meta_mod = modifiers & Qt::ControlModifier;
#else
  const auto ctrl_mod = modifiers & Qt::ControlModifier;
  const auto meta_mod = modifiers & Qt::MetaModifier;
#endif

  auto event = KeyEvent(this, ev->key(), ev->count(), false,
                        modifiers & Qt::ShiftModifier, modifiers & ctrl_mod,
                        modifiers & Qt::AltModifier, modifiers & meta_mod);
  OnKeyDown(&event);
}

void QtWindow::HandleKeyRelease(QKeyEvent* ev) {
  const auto& modifiers = ev->modifiers();

#ifdef Q_OS_MACOS
  auto ctrl_mod = modifiers & Qt::MetaModifier;
  auto meta_mod = modifiers & Qt::ControlModifier;
#else
  const auto ctrl_mod = modifiers & Qt::ControlModifier;
  const auto meta_mod = modifiers & Qt::MetaModifier;
#endif

  auto event = KeyEvent(this, ev->key(), ev->count(), true,
                        modifiers & Qt::ShiftModifier, modifiers & ctrl_mod,
                        modifiers & Qt::AltModifier, modifiers & meta_mod);
  OnKeyUp(&event);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe
