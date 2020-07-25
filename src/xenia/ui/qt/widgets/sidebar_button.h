#ifndef XENIA_UI_QT_SIDEBAR_BUTTON_H_
#define XENIA_UI_QT_SIDEBAR_BUTTON_H_

#include <QPushButton>
#include "xenia/ui/qt/themeable_widget.h"

namespace xe {
namespace ui {
namespace qt {

class XSideBarButton : public Themeable<QPushButton> {
  Q_OBJECT
 public:
  explicit XSideBarButton(const QString& text, QWidget* parent = nullptr);
  explicit XSideBarButton(QChar glyph, const QString& text,
                          QWidget* parent = nullptr);

 protected:
  void paintEvent(QPaintEvent*) override;

 private:
  QChar glyph_;
  QString text_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif
