#include "combobox.h"

namespace xe {
namespace ui {
namespace qt {

XComboBox::XComboBox(QWidget* parent)
    : Themeable<QComboBox>("XComboBox", parent) {
  setCursor(Qt::PointingHandCursor);
  setFocusPolicy(Qt::TabFocus);  // disable retaining focus through mouse click
}

}  // namespace qt
}  // namespace ui
}  // namespace xe