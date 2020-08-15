/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2020 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "menu_win.h"

#include "window.h"

namespace xe {
namespace ui {

MenuItem* Win32MenuItem::CreateChild(Type type, std::wstring text,
                                     std::wstring hotkey, Callback callback) {
  auto child = std::unique_ptr<Win32MenuItem>(new Win32MenuItem(
      static_cast<Win32Menu*>(menu_), type, text, hotkey, callback));

  children_.push_back(std::move(child));
  auto child_ptr = children_.back().get();
  OnChildAdded(child_ptr);
  return child_ptr;
}

Win32MenuItem::Win32MenuItem(Win32Menu* menu, Type type,
                             const std::wstring& text,
                             const std::wstring& hotkey,
                             std::function<void()> callback)
    : MenuItem(menu, type, text, hotkey, std::move(callback)),
      handle_(nullptr) {
  switch (type) {
    case Type::kRoot:
    case Type::kSubmenu: {
      handle_ = CreatePopupMenu();
      break;
    }
    default:
      // May just be a placeholder.
      break;
  }
  if (handle_) {
    MENUINFO menu_info = {0};
    menu_info.cbSize = sizeof(menu_info);
    menu_info.fMask = MIM_MENUDATA | MIM_STYLE;
    menu_info.dwMenuData = ULONG_PTR(this);
    menu_info.dwStyle = MNS_NOTIFYBYPOS;
    SetMenuInfo(handle_, &menu_info);
  }
}

void Win32MenuItem::EnableMenuItem(Window& window) {
  int i = 0;
  for (auto iter = children_.begin(); iter != children_.end(); ++iter, i++) {
    ::EnableMenuItem(handle_, i, MF_BYPOSITION | MF_ENABLED);
  }
  DrawMenuBar((HWND)window.native_handle());
}

void Win32MenuItem::DisableMenuItem(Window& window) {
  int i = 0;
  for (auto iter = children_.begin(); iter != children_.end(); ++iter, i++) {
    ::EnableMenuItem(handle_, i, MF_BYPOSITION | MF_GRAYED);
  }
  DrawMenuBar((HWND)window.native_handle());
}

void Win32MenuItem::OnChildAdded(MenuItem* generic_child_item) {
  auto child_item = static_cast<Win32MenuItem*>(generic_child_item);

  switch (child_item->type()) {
    case MenuItem::Type::kRoot:
      // Nothing special.
      break;
    case MenuItem::Type::kSubmenu:
      AppendMenuW(handle_, MF_POPUP,
                  reinterpret_cast<UINT_PTR>(child_item->handle()),
                  child_item->text().c_str());
      break;
    case MenuItem::Type::kSeparator:
      AppendMenuW(handle_, MF_SEPARATOR, UINT_PTR(child_item->handle()), 0);
      break;
    case MenuItem::Type::kString:
      auto full_name = child_item->text();
      if (!child_item->hotkey().empty()) {
        full_name += L"\t" + child_item->hotkey();
      }
      AppendMenuW(handle_, MF_STRING, UINT_PTR(child_item->handle_),
                  full_name.c_str());
      break;
  }
}

void Win32MenuItem::OnChildRemoved(MenuItem* generic_child_item) {}

Win32Menu::Win32Menu(Window* window) : Menu(window) {
  menu_ = ::CreateMenu();

  MENUINFO menu_info = {0};
  menu_info.cbSize = sizeof(menu_info);
  menu_info.fMask = MIM_MENUDATA | MIM_STYLE;
  menu_info.dwMenuData = ULONG_PTR(this);
  menu_info.dwStyle = MNS_NOTIFYBYPOS;
  SetMenuInfo(menu_, &menu_info);
}

Win32Menu::~Win32Menu() { DestroyMenu(menu_); }


MenuItem* Win32Menu::CreateMenuItem(const std::wstring& title) {
  auto menu_item = std::unique_ptr<Win32MenuItem>(
      new Win32MenuItem(this, MenuType::kRoot, title, L"", nullptr));
  auto menu_item_ptr = menu_item.get();
  menu_items_.push_back(std::move(menu_item));
  AppendMenuW(menu_, MF_POPUP,
              reinterpret_cast<UINT_PTR>(menu_item_ptr->handle()),
              reinterpret_cast<LPCWSTR>(menu_item_ptr->text().c_str()));
  return menu_item_ptr;
}

}  // namespace ui
}  // namespace xe