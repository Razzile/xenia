#ifndef XENIA_UI_QT_COMBOBOX_H_
#define XENIA_UI_QT_COMBOBOX_H_

#include <QComboBox>

#include "xenia/ui/qt/themeable_widget.h"

namespace xe {
namespace ui {
namespace qt {

class XComboBox : public Themeable<QComboBox> {
  Q_OBJECT
 public:
  XComboBox(QWidget* parent = nullptr);
};
}  // namespace qt
}  // namespace ui
}  // namespace xe
#endif