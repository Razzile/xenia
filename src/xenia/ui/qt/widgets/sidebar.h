#ifndef XENIA_UI_QT_SIDEBAR_H_
#define XENIA_UI_QT_SIDEBAR_H_

#include <QToolBar>
#include "xenia/ui/qt/themeable_widget.h"
#include "xenia/ui/qt/widgets/sidebar_button.h"

namespace xe {
namespace ui {
namespace qt {

class XSideBar : public Themeable<QToolBar> {
 public:
  explicit XSideBar();

  XSideBarButton* addAction(const QString& text);
  XSideBarButton* addAction(QChar glyph, const QString& text);

  QWidget* addSpacing(int size);

 private:
  QVector<XSideBarButton*> buttons_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif
