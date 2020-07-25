#ifndef XENIA_UI_QT_TEXTEDIT_H_
#define XENIA_UI_QT_TEXTEDIT_H_

#include <QPlainTextEdit>

#include "xenia/ui/qt/themeable_widget.h"

namespace xe {
namespace ui {
namespace qt {

class XTextEdit : public Themeable<QPlainTextEdit> {
  Q_OBJECT
 public:
  explicit XTextEdit(QWidget* parent = nullptr);
  XTextEdit(const QString& text, QWidget* parent = nullptr);

 private:
  void Build();
};

}  // namespace qt
}  // namespace ui
}  // namespace xe
#endif