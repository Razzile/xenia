#include "controls_pane.h"

namespace xe {
namespace ui {
namespace qt {

void ControlsPane::Build() {
  widget_ = new QWidget();
  widget_->setStyleSheet("background: yellow");
}

}  // namespace qt
}  // namespace ui
}  // namespace xe