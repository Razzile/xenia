/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2020 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/ui/menu.h"

namespace xe {
namespace ui {


MenuItem::MenuItem(Menu* menu, Type type, const std::wstring& text,
                   const std::wstring& hotkey, std::function<void()> callback)
    : menu_(menu),
      type_(type),
      parent_item_(nullptr),
      text_(text),
      hotkey_(hotkey),
      callback_(std::move(callback)) {}

MenuItem::~MenuItem() = default;

void MenuItem::RemoveChild(MenuItem* child_item) {
  for (auto it = children_.begin(); it != children_.end(); ++it) {
    if (it->get() == child_item) {
      children_.erase(it);
      OnChildRemoved(child_item);
      break;
    }
  }
}

MenuItem* MenuItem::child(size_t index) { return children_[index].get(); }

void MenuItem::OnSelected(UIEvent* e) {
  if (callback_) {
    callback_();
  }
}

}  // namespace ui
}  // namespace xe
