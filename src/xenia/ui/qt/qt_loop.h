/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2020 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/ui/loop.h"

namespace xe {
namespace ui {
namespace qt {

class QtLoop final : public Loop {
 public:
  QtLoop() = default;
  bool is_on_loop_thread() override;

  void Post(std::function<void()> fn) override;
  void PostDelayed(std::function<void()> fn, uint64_t delay_millis) override;

  void Quit() override;
  void AwaitQuit() override;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe