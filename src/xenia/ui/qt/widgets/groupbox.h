#ifndef XENIA_UI_QT_GROUPBOX_H_
#define XENIA_UI_QT_GROUPBOX_H_

#include <QGroupBox>
#include "xenia/ui/qt/themeable_widget.h"
namespace xe {
namespace ui {
namespace qt {

class XGroupBox : public Themeable<QGroupBox> {
  Q_OBJECT
 public:
  explicit XGroupBox(QWidget* parent = nullptr);
  explicit XGroupBox(const QString& title, QWidget* parent = nullptr);

  /*void paintEvent(QPaintEvent* e) override;*/

 private:
  void Build();
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif