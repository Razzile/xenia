/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2020 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/ui/loop.h"

#ifndef XENIA_UI_QT_LOOP_QT_H_
#define XENIA_UI_QT_LOOP_QT_H_

namespace xe {
namespace ui {
namespace qt {

class QtLoop final : public Loop {
 public:
  QtLoop();
  bool is_on_loop_thread() override;

  void Post(std::function<void()> fn) override;
  void PostDelayed(std::function<void()> fn, uint64_t delay_millis) override;

  void Quit() override;
  void AwaitQuit() override;

private:
  bool has_quit_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif