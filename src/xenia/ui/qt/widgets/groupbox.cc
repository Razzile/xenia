#include "groupbox.h"
#include <QGraphicsEffect>

namespace xe {
namespace ui {
namespace qt {

XGroupBox::XGroupBox(QWidget* parent)
    : Themeable<QGroupBox>("XGroupBox", parent) {
  Build();
}

XGroupBox::XGroupBox(const QString& title, QWidget* parent)
    : Themeable<QGroupBox>("XGroupBox", title, parent) {
  Build();
}

void XGroupBox::Build() {
  QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect;
  effect->setBlurRadius(2);
  effect->setXOffset(0);
  effect->setYOffset(0);
  effect->setColor(QColor(0, 0, 0, 96));
  setGraphicsEffect(effect);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe
