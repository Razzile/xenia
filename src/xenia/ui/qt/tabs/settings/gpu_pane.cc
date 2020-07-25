#include "gpu_pane.h"

namespace xe {
namespace ui {
namespace qt {

void GPUPane::Build() {
  QWidget* widget = new QWidget();
  widget->setStyleSheet("background: orange");

  set_widget(widget);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe