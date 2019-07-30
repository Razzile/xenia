#include "cpu_pane.h"

namespace xe {
namespace ui {
namespace qt {

void CPUPane::Build() {
  widget_ = new QWidget();
  widget_->setStyleSheet("background: gray");
}

}  // namespace qt
}  // namespace ui
}  // namespace xe