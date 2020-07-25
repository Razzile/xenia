#include "text_edit.h"
#include <QStyleFactory>

namespace xe {
namespace ui {
namespace qt {

XTextEdit::XTextEdit(QWidget* parent)
    : Themeable<QPlainTextEdit>("XTextEdit", parent) {
  Build();
}

XTextEdit::XTextEdit(const QString& text, QWidget* parent)
    : Themeable<QPlainTextEdit>("XTextEdit", text, parent) {
  Build();
}

void XTextEdit::Build() {
  // the default Windows QStyle doesn't support theming most of the text box
  // so we switch to the Fusion style here to bypass this
  setStyle(QStyleFactory::create("Fusion"));
  setFrameShape(QFrame::NoFrame);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe