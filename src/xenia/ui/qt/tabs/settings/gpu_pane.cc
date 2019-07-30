#include "gpu_pane.h"

namespace xe {
namespace ui {
namespace qt {

void GPUPane::Build() {
  widget_ = new QWidget();
  widget_->setStyleSheet("background: orange");
}

}  // namespace qt
}  // namespace ui
}  // namespace xe