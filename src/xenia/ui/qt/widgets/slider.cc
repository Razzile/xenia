#include "xenia/ui/qt/widgets/slider.h"
#include <QMouseEvent>

namespace xe {
namespace ui {
namespace qt {

XSlider::XSlider(Qt::Orientation orientation, QWidget* parent)
    : Themeable<QSlider>("XSlider", orientation, parent) {}

void XSlider::mouseReleaseEvent(QMouseEvent* e) { clearFocus(); }

}  // namespace qt
}  // namespace ui
}  // namespace xe