#include "library_pane.h"

namespace xe {
namespace ui {
namespace qt {

void LibraryPane::Build() {
  QWidget* widget = new QWidget();
  widget->setStyleSheet("background: lime");

  set_widget(widget);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe