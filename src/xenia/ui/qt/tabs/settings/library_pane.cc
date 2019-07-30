#include "library_pane.h"

namespace xe {
namespace ui {
namespace qt {

void LibraryPane::Build() {
  widget_ = new QWidget();
  widget_->setStyleSheet("background: lime");
}

}  // namespace qt
}  // namespace ui
}  // namespace xe