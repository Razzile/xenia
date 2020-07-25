#include "controls_pane.h"

namespace xe {
namespace ui {
namespace qt {

void ControlsPane::Build() {
  QWidget* widget = new QWidget();
  widget->setStyleSheet("background: yellow");

  set_widget(widget);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe