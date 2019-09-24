#include "advanced_pane.h"

namespace xe {
namespace ui {
namespace qt {

void AdvancedPane::Build() {
  QWidget* widget = new QWidget();
  widget->setStyleSheet("background: green");

  set_widget(widget);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe