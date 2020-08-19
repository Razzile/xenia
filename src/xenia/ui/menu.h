/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2020 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_UI_MENU_H_
#define XENIA_UI_MENU_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "xenia/ui/ui_event.h"

namespace xe {
namespace ui {

class Window;

class MenuItem {
  friend class Menu;

 public:
  using Callback = std::function<void()>;
  enum class Type {
    kSubmenu,  // Submenu (Popup menu in win32)
    kSeparator,
    kRoot,    // Root menu
    kString,  // Menu is just a string
  };

  virtual ~MenuItem();

  MenuItem* parent_item() const { return parent_item_; }
  Type type() { return type_; }
  const std::wstring& text() { return text_; }
  const std::wstring& hotkey() { return hotkey_; }

  virtual MenuItem* CreateChild(Type type, std::wstring text = L"",
                                std::wstring hotkey = L"",
                                Callback callback = nullptr) = 0;
  void RemoveChild(MenuItem* child_item);
  MenuItem* child(size_t index);

  virtual void EnableMenuItem(Window& window) = 0;
  virtual void DisableMenuItem(Window& window) = 0;

 protected:
  MenuItem(Menu* menu, Type type, const std::wstring& text,
           const std::wstring& hotkey, Callback callback);

  virtual void OnChildAdded(MenuItem* child_item) {}
  virtual void OnChildRemoved(MenuItem* child_item) {}

  virtual void OnSelected(UIEvent* e);

  Menu* menu_;
  Type type_;
  MenuItem* parent_item_;
  std::vector<std::unique_ptr<MenuItem>> children_;
  std::wstring text_;
  std::wstring hotkey_;
  std::function<void()> callback_;
};

class Menu {
  using MenuType = MenuItem::Type;

 public:
  explicit Menu(Window* window) : window_(window), enabled_(true) {}

  virtual ~Menu() = default;

  virtual MenuItem* CreateMenuItem(const std::wstring& title) = 0;

  Window* window() const { return window_; }
  const std::vector<std::unique_ptr<MenuItem>>& menu_items() const {
    return menu_items_;
  }

  bool enabled() const { return enabled_; }

  void set_enabled(bool enabled) {
    enabled_ = enabled;
    for (const auto& menu_item : menu_items_) {
      if (enabled) {
        menu_item->EnableMenuItem(*window());
      } else {
        menu_item->DisableMenuItem(*window());
      }
    }
  }

 protected:
  Window* window_;
  std::vector<std::unique_ptr<MenuItem>> menu_items_;
  bool enabled_;
};

}  // namespace ui
}  // namespace xe

#endif  // XENIA_UI_MENU_ITEM_H_
