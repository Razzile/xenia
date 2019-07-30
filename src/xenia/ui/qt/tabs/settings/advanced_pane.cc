#include "advanced_pane.h"

namespace xe {
namespace ui {
namespace qt {

void AdvancedPane::Build() {
  widget_ = new QWidget();
  widget_->setStyleSheet("background: green");
}

}  // namespace qt
}  // namespace ui
}  // namespace xe