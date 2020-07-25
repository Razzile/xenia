#include "xenia/ui/qt/widgets/status_bar.h"

namespace xe {
namespace ui {
namespace qt {

XStatusBar::XStatusBar(QWidget* parent)
    : Themeable<QStatusBar>("XStatusBar", parent) {
  setSizeGripEnabled(false);

  QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect;
  effect->setBlurRadius(16);
  effect->setXOffset(0);
  effect->setYOffset(-2);
  effect->setColor(QColor(0, 0, 0, 64));

  setGraphicsEffect(effect);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe
