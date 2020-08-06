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
#include <QMimeData>
#include <QScreen>
#include <QWindow>

namespace xe {
namespace ui {
namespace qt {

QtWindow::QtWindow(Loop* loop, const std::wstring& title)
    : Window(loop, title), main_menu_enabled_(false) {}

NativePlatformHandle QtWindow::native_platform_handle() const {
  return nullptr;
}

NativeWindowHandle QtWindow::native_handle() const {
  return reinterpret_cast<NativeWindowHandle>(this->winId());
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
  windowHandle()->setTitle(QString::fromWCharArray(title.c_str()));
  return true;
}

bool QtWindow::SetIcon(const void* buffer, size_t size) {
  if (!buffer || size == 0) {
    windowHandle()->setIcon(QIcon());
    return true;
  } else {
    auto pixmap = QPixmap();
    if (pixmap.loadFromData(static_cast<const uint8_t*>(buffer),
                            static_cast<uint32_t>(size))) {
      windowHandle()->setIcon(pixmap);
      return true;
    }
    return false;
  }
}

bool QtWindow::SetIcon(const QIcon& icon) {
  this->setWindowIcon(icon);
  return true;
}

bool QtWindow::is_fullscreen() const {
  return windowHandle()->windowStates() & Qt::WindowFullScreen;
}

void QtWindow::ToggleFullscreen(bool fullscreen) {
  if (is_fullscreen() == fullscreen) {
    return;
  }

  auto win_states = windowHandle()->windowStates();
  win_states ^= Qt::WindowFullScreen;
  windowHandle()->setWindowStates(win_states);
}

int QtWindow::get_dpi() const {
  return windowHandle()->screen()->logicalDotsPerInch();
}

void QtWindow::set_focus(bool value) {
  if (value) {
    QApplication::setActiveWindow(this);
  } else {
    // TODO: test this works with actual windows
    this->clearFocus();
  }
}

void QtWindow::Resize(int32_t width, int32_t height) {
  this->resize(width, height);
  auto e = UIEvent(this);
  OnResize(&e);
}

void QtWindow::Resize(int32_t left, int32_t top, int32_t right,
                      int32_t bottom) {
  Resize(right - left, bottom - top);
}

bool QtWindow::Initialize() {
  setAttribute(Qt::WA_NativeWindow);
  setAcceptDrops(true);

  connect(this->windowHandle(), &QWindow::screenChanged, this,
          &QtWindow::HandleWindowScreenChange);

  if (MakeReady()) {
    this->show();
    OnCreate();
    return true;
  }
  return false;
}

void QtWindow::Close() { this->close(); }

bool QtWindow::MakeReady() {
  this->resize(width_, height_);
  // TODO: Apply menu item to menu bar here.

  return true;
}

void QtWindow::UpdateWindow() {
  this->menuBar()->setEnabled(main_menu_enabled_);
}

void QtWindow::HandleWindowScreenChange(QScreen* screen) {
  // emit this event regardless of if the new screen has the same dpi
  UIEvent event(this);
  OnDpiChanged(&event);
}

void QtWindow::HandleWindowStateChange(QWindowStateChangeEvent* ev) {
  UIEvent event(this);
  const auto old_state = ev->oldState();
  const auto state = windowState();

  if ((old_state & Qt::WindowMinimized) && (state & Qt::WindowMinimized) == 0) {
    OnVisible(&event);
  } else if (!(old_state & Qt::WindowMinimized) &&
             state & Qt::WindowMinimized) {
    OnHidden(&event);
  }
}

void QtWindow::HandleKeyPress(QKeyEvent* ev) {
  const auto& modifiers = ev->modifiers();

#ifdef Q_OS_MACOS
  const auto ctrl_mod = modifiers & Qt::MetaModifier;
  const auto meta_mod = modifiers & Qt::ControlModifier;
#else
  const auto ctrl_mod = modifiers & Qt::ControlModifier;
  const auto meta_mod = modifiers & Qt::MetaModifier;
#endif

  auto event = KeyEvent(this, ev->key(), ev->count(), false,
                        modifiers & Qt::ShiftModifier, modifiers & ctrl_mod,
                        modifiers & Qt::AltModifier, modifiers & meta_mod);
  if (ev->isAutoRepeat()) {
    OnKeyChar(&event);
  } else {
    OnKeyDown(&event);
  }
}

void QtWindow::HandleKeyRelease(QKeyEvent* ev) {
  // key is being held down, don't handle event
  if (ev->isAutoRepeat()) {
    return;
  }

  const auto& modifiers = ev->modifiers();

#ifdef Q_OS_MACOS
  const auto ctrl_mod = modifiers & Qt::MetaModifier;
  const auto meta_mod = modifiers & Qt::ControlModifier;
#else
  const auto ctrl_mod = modifiers & Qt::ControlModifier;
  const auto meta_mod = modifiers & Qt::MetaModifier;
#endif

  auto event = KeyEvent(this, ev->key(), ev->count(), true,
                        modifiers & Qt::ShiftModifier, modifiers & ctrl_mod,
                        modifiers & Qt::AltModifier, modifiers & meta_mod);
  OnKeyUp(&event);
}

void QtWindow::HandleMouseMove(QMouseEvent* ev) {
  const auto dx = ev->x() - last_mouse_pos_.x();
  const auto dy = ev->y() - last_mouse_pos_.y();
  auto mouse_event =
      MouseEvent(this, MouseEvent::Button::kNone, ev->x(), ev->y(), dx, dy);

  last_mouse_pos_ = ev->pos();

  OnMouseMove(&mouse_event);
}

void QtWindow::HandleMouseClick(QMouseEvent* ev, bool release) {
  auto gen_button_event = [&](MouseEvent::Button btn) {
    auto mouse_event = MouseEvent(this, btn, ev->x(), ev->y());
    if (!release) {
      OnMouseDown(&mouse_event);
    } else {
      OnMouseUp(&mouse_event);
    }
  };

  const auto buttons = ev->buttons();

  if (buttons & Qt::LeftButton) {
    gen_button_event(MouseEvent::Button::kLeft);
  }
  if (buttons & Qt::RightButton) {
    gen_button_event(MouseEvent::Button::kRight);
  }
  if (buttons & Qt::MiddleButton) {
    gen_button_event(MouseEvent::Button::kMiddle);
  }
}

void QtWindow::OnResize(UIEvent* e) {
  width_ = QMainWindow::width();
  height_ = QMainWindow::height();

  Window::OnResize(e);
}

bool QtWindow::event(QEvent* event) {
  switch (event->type()) {
    case QEvent::Close: {
      OnClose();

      break;
    }

    case QEvent::Resize: {
      UIEvent ev(this);
      OnResize(&ev);

      break;
    }

    case QEvent::FocusIn: {
      UIEvent ev(this);
      OnGotFocus(&ev);

      break;
    }
    case QEvent::FocusOut: {
      UIEvent ev(this);
      OnLostFocus(&ev);

      break;
    }

    case QEvent::KeyPress: {
      const auto key_event = static_cast<QKeyEvent*>(event);
      HandleKeyPress(key_event);

      break;
    }
    case QEvent::KeyRelease: {
      const auto key_event = static_cast<QKeyEvent*>(event);
      HandleKeyRelease(key_event);

      break;
    }

    case QEvent::MouseMove: {
      const auto mouse_event = static_cast<QMouseEvent*>(event);
      HandleMouseMove(mouse_event);

      break;
    }
    case QEvent::MouseButtonRelease: {
      const auto mouse_event = static_cast<QMouseEvent*>(event);
      HandleMouseClick(mouse_event, true);

      break;
    }
    case QEvent::MouseButtonPress: {
      const auto mouse_event = static_cast<QMouseEvent*>(event);
      HandleMouseClick(mouse_event, false);

      break;
    }
    default:
      break;
  }

  return QMainWindow::event(event);
}

void QtWindow::changeEvent(QEvent* event) {
  QMainWindow::changeEvent(event);

  switch (event->type()) {
    case QEvent::WindowStateChange: {
      const auto window_state_event =
          static_cast<QWindowStateChangeEvent*>(event);
      HandleWindowStateChange(window_state_event);

      break;
    }

    default:
      break;
  }
}

void QtWindow::dragEnterEvent(QDragEnterEvent* event) {
  if (event->mimeData()->hasUrls()) {
    event->acceptProposedAction();
  }
}

void QtWindow::dropEvent(QDropEvent* event) {
  std::vector<std::wstring> files;
  for (const auto& url : event->mimeData()->urls()) {
    const auto path = url.path();
    // QString::toStdWString() is broken in debug mode
    std::wstring file;
    file.resize(path.size());
    file.resize(path.toWCharArray(&file.front()));
    files.push_back(file);
  }
  auto file_drop_event = FileDropEvent(this, files);
  OnFileDrop(&file_drop_event);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe
