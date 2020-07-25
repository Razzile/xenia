#ifndef XENIA_UI_QT_PUSH_BUTTON_H_
#define XENIA_UI_QT_PUSH_BUTTON_H_

#include <QPushButton>
#include "xenia/ui/qt/themeable_widget.h"

namespace xe {
namespace ui {
namespace qt {

class XPushButton : public Themeable<QPushButton> {
  Q_OBJECT
 public:
  explicit XPushButton(QWidget* parent = nullptr);
  explicit XPushButton(const QString& text, QWidget* parent = nullptr);
  XPushButton(const QIcon& icon, const QString& text,
              QWidget* parent = nullptr);

 private:
  void Build();
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif