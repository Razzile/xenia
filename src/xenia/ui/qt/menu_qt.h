/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2020 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_UI_QT_MENU_QT_H_
#define XENIA_UI_QT_MENU_QT_H_

#include <QMenu>
#include <QMenuBar>
#include "xenia/ui/menu.h"

namespace xe {
namespace ui {
namespace qt {

class QtMenuItem : public MenuItem {
  friend class QtMenu;
public:
  MenuItem* CreateChild(Type type, std::wstring text, std::wstring hotkey,
                        Callback callback) override;

  void EnableMenuItem(Window& window) override;
  void DisableMenuItem(Window& window) override;

 protected:
  QtMenuItem(QtMenu* menu, Type type, const std::wstring& text,
             const std::wstring& hotkey, std::function<void()> callback);

  void OnChildAdded(MenuItem* child_item) override;
  void OnChildRemoved(MenuItem* child_item) override;

private:
  QMenu* qt_menu_;
};

class QtMenu : public Menu {
 public:
  explicit QtMenu(Window* window);
  MenuItem* CreateMenuItem(const std::wstring& title) override;

  QMenuBar* menu_bar() const { return menu_bar_; }
 private:
  QMenuBar* menu_bar_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif