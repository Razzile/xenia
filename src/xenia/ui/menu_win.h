/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2020 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_UI_MENU_WIN_H_
#define XENIA_UI_MENU_WIN_H_

#include "menu.h"
#include "xenia/base/platform_win.h"

namespace xe {
namespace ui {

class Win32MenuItem : public MenuItem {
  friend class Win32Menu;
 public:

  MenuItem* CreateChild(Type type, std::wstring text, std::wstring hotkey,
      Callback callback) override;
  HMENU handle() const { return handle_; }

  void EnableMenuItem(Window& window) override;
  void DisableMenuItem(Window& window) override;

  using MenuItem::OnSelected;

 protected:
  void OnChildAdded(MenuItem* child_item) override;
  void OnChildRemoved(MenuItem* child_item) override;

  Win32MenuItem(Win32Menu *menu, Type type, const std::wstring& text, const std::wstring& hotkey,
                std::function<void()> callback);

 private:
  // menu item handle (if submenu)
  HMENU handle_;
};

class Win32Menu : public Menu {
 public:
  explicit Win32Menu(Window* window);
  ~Win32Menu();

  HMENU handle() const { return menu_; }

  MenuItem* CreateMenuItem(const std::wstring& title) override;
private:
  HMENU menu_;
};

}  // namespace ui
}  // namespace xe

#endif