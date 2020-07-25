#include "interface_pane.h"

namespace xe {
namespace ui {
namespace qt {

void InterfacePane::Build() {
  QWidget* widget = new QWidget();
  widget->setStyleSheet("background: brown");

  set_widget(widget);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe