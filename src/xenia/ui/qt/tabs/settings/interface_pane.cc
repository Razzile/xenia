#include "interface_pane.h"

namespace xe {
namespace ui {
namespace qt {

void InterfacePane::Build() {
  widget_ = new QWidget();
  widget_->setStyleSheet("background: brown");
}

}  // namespace qt
}  // namespace ui
}  // namespace xe