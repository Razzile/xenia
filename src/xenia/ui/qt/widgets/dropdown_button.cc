#include "dropdown_button.h"

namespace xe {
namespace ui {
namespace qt {

XDropdownButton::XDropdownButton(QWidget* parent)
    : Themeable<QToolButton>("XDropdownButton", parent) {
  menu_ = new QMenu();
  setMenu(menu_);

  Build();
}

XDropdownButton::XDropdownButton(const QString& text, QWidget* parent)
    : Themeable<QToolButton>("XDropdownButton", parent) {
  setText(text);
  menu_ = new QMenu(this);
  setMenu(menu_);

  Build();
}

XDropdownButton::XDropdownButton(const QString& text, QMenu* menu,
                                 QWidget* parent)
    : Themeable<QToolButton>("XDropdownButton", parent) {
  setText(text);
  menu_ = menu;
  setMenu(menu_);

  Build();
}

void XDropdownButton::Build() {
  setPopupMode(QToolButton::MenuButtonPopup);
  setCursor(Qt::PointingHandCursor);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe