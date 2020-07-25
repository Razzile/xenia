#ifndef XENIA_UI_QT_LINEEDIT_H_
#define XENIA_UI_QT_LINEEDIT_H_

#include <QLineEdit>

#include "xenia/ui/qt/themeable_widget.h"

namespace xe {
namespace ui {
namespace qt {

class XLineEdit : public Themeable<QLineEdit> {
  Q_OBJECT
 public:
  explicit XLineEdit(QWidget* parent = nullptr);
  XLineEdit(const QString& text, QWidget* parent = nullptr);
};

}  // namespace qt
}  // namespace ui
}  // namespace xe
#endif