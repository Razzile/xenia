#include "push_button.h"

namespace xe {
namespace ui {
namespace qt {

XPushButton::XPushButton(QWidget* parent)
    : Themeable<QPushButton>("XPushButton", parent) {
  Build();
}

XPushButton::XPushButton(const QString& text, QWidget* parent)
    : Themeable<QPushButton>("XPushButton", text, parent) {
  Build();
}

XPushButton::XPushButton(const QIcon& icon, const QString& text,
                         QWidget* parent)
    : Themeable<QPushButton>("XPushButton", icon, text, parent) {
  Build();
}

void XPushButton::Build() {
  setFlat(true);
  setCursor(Qt::PointingHandCursor);
  setFocusPolicy(Qt::TabFocus);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe
