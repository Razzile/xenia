/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2020 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "menu_qt.h"

#include "window_qt.h"

namespace xe {
namespace ui {
namespace qt {

MenuItem* QtMenuItem::CreateChild(Type type, std::wstring text,
                                  std::wstring hotkey, Callback callback) {
  auto child = std::unique_ptr<QtMenuItem>(new QtMenuItem(
      static_cast<QtMenu*>(menu_), type, text, hotkey, callback));
  children_.push_back(std::move(child));
  auto child_ptr = children_.back().get();
  OnChildAdded(child_ptr);
  return child_ptr;
}

void QtMenuItem::EnableMenuItem(Window& window) {
  qt_menu_->setEnabled(true);
  for (const auto& child : children_) {
    child->EnableMenuItem(window);
  }
}

void QtMenuItem::DisableMenuItem(Window& window) {
  qt_menu_->setDisabled(true);
  for (const auto& child : children_) {
    child->DisableMenuItem(window);
  }
}

QtMenuItem::QtMenuItem(QtMenu* menu, Type type, const std::wstring& text,
                       const std::wstring& hotkey,
                       std::function<void()> callback)
    : MenuItem(menu, type, text, hotkey, callback), qt_menu_(nullptr) {
}

void QtMenuItem::OnChildAdded(MenuItem* generic_child_item) {
  assert(qt_menu_ != nullptr);

  auto child_item = static_cast<QtMenuItem*>(generic_child_item);

  switch (child_item->type()) {
    case Type::kSubmenu: {
      child_item->qt_menu_ =
          new QMenu(QString::fromWCharArray(child_item->text().c_str()));
      qt_menu_->addMenu(child_item->qt_menu_);
    } break;

    case Type::kSeparator: {
      qt_menu_->addSeparator();
    } break;

    case Type::kString: {
      auto title = QString::fromWCharArray(child_item->text().c_str());
      auto hotkey = child_item->hotkey();
      auto shortcut =
          hotkey.empty()
              ? 0
              : QKeySequence(QString::fromWCharArray(hotkey.c_str()));

      const auto& callback = child_item->callback_;
      qt_menu_->addAction(
          title,
          [&callback]() {
            if (callback) {
              callback();
            }
          },
          shortcut);
    } break;

    case Type::kRoot:
    default:
      break;
  }
}

void QtMenuItem::OnChildRemoved(MenuItem* child_item) {}

QtMenu::QtMenu(Window* window) : Menu(window) {
  auto qt_window = static_cast<QtWindow*>(window);
  menu_bar_ = qt_window->menuBar();
}

MenuItem* QtMenu::CreateMenuItem(const std::wstring& title) {
  auto menu_item = std::unique_ptr<QtMenuItem>(
      new QtMenuItem(this, MenuType::kRoot, title, L"", nullptr));

  auto menu = new QMenu(QString::fromWCharArray(title.c_str()));
  menu_item->qt_menu_ = menu;

  menu_items_.push_back(std::move(menu_item));

  menu_bar_->addMenu(menu);

  return menu_items_.back().get();
}

}  // namespace qt
}  // namespace ui
}  // namespace xe
