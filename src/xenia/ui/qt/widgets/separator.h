#ifndef XENIA_UI_QT_SEPARATOR_H_
#define XENIA_UI_QT_SEPARATOR_H_

#include "xenia/ui/qt/themeable_widget.h"

namespace xe {
namespace ui {
namespace qt {

class XSeparator : public Themeable<QWidget> {
  Q_OBJECT

  Q_PROPERTY(qreal thickness READ getThickness WRITE setThickness);

 public:
  explicit XSeparator(double thickness = 1, QWidget* parent = nullptr)
      : Themeable<QWidget>("XSeparator", parent), thickness_(thickness) {
    Update();
  }

  double getThickness() const { return thickness_; }
  void setThickness(double thickness) {
    thickness_ = thickness;
    Update();
  }

 private:
  void Update() { setFixedHeight(thickness_); }

  double thickness_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif
