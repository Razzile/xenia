#ifndef XENIA_UI_QT_SLIDER_H_
#define XENIA_UI_QT_SLIDER_H_

#include "xenia/ui/qt/themeable_widget.h"

#include <QSlider>

namespace xe {
namespace ui {
namespace qt {

class XSlider : public Themeable<QSlider> {
  Q_OBJECT

 public:
  explicit XSlider(Qt::Orientation orientation = Qt::Horizontal,
                   QWidget* parent = nullptr);

 private:
  void mouseReleaseEvent(QMouseEvent* e) override;

  double bar_size_ = 3;
  double bar_radius_ = 3;
  double slider_radius_ = 5;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif