#include "line_edit.h"
#include <QStyleFactory>

namespace xe {
namespace ui {
namespace qt {

XLineEdit::XLineEdit(QWidget* parent)
    : Themeable<QLineEdit>("XLineEdit", parent) {}

XLineEdit::XLineEdit(const QString& text, QWidget* parent)
    : Themeable<QLineEdit>("XLineEdit", text, parent) {}

}  // namespace qt
}  // namespace ui
}  // namespace xe