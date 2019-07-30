#include "general_pane.h"

namespace xe {
namespace ui {
namespace qt {

void GeneralPane::Build() {
  widget_ = new QWidget();
  widget_->setStyleSheet("background: red");
}

}  // namespace qt
}  // namespace ui
}  // namespace xe