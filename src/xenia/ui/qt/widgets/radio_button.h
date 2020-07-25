#ifndef XENIA_UI_QT_RADIO_BUTTON_H_
#define XENIA_UI_QT_RADIO_BUTTON_H_

#include <QRadioButton>

#include "xenia/ui/qt/themeable_widget.h"

namespace xe {
namespace ui {
namespace qt {

class XRadioButton : public Themeable<QRadioButton> {
  Q_OBJECT

  Q_PROPERTY(bool custom READ custom WRITE set_custom);
  Q_PROPERTY(QColor border_color READ border_color WRITE set_border_color);
  Q_PROPERTY(QColor focus_color READ focus_color WRITE set_focus_color);
  Q_PROPERTY(QColor checked_color READ checked_color WRITE set_checked_color);

 public:
  explicit XRadioButton(QWidget* parent = nullptr);

  double label_indent() const { return label_indent_; }
  void set_label_indent(double indent) { label_indent_ = indent; }

  bool custom() const { return custom_; }
  void set_custom(bool value) { custom_ = value; }

  const QColor& border_color() const { return border_color_; }
  void set_border_color(const QColor& color) { border_color_ = color; }
  const QColor& focus_color() const { return focus_color_; }
  void set_focus_color(const QColor& color) { focus_color_ = color; }
  const QColor& checked_color() const { return checked_color_; }
  void set_checked_color(const QColor& color) { checked_color_ = color; }

 protected:
  void paintEvent(QPaintEvent* e) override;
  QSize sizeHint() const override;

 private:
  double label_indent_ = 8.0;
  bool custom_ = false;
  QColor border_color_;
  QColor focus_color_;
  QColor checked_color_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif
