#include "general_pane.h"

namespace xe {
namespace ui {
namespace qt {

void GeneralPane::Build() {
  QWidget* widget = new QWidget();
  widget->setStyleSheet("background: red");

  set_widget(widget);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe