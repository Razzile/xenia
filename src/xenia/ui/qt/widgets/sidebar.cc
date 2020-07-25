#include "xenia/ui/qt/widgets/sidebar.h"

namespace xe {
namespace ui {
namespace qt {

XSideBar::XSideBar() : Themeable<QToolBar>("XSideBar") {}

XSideBarButton* XSideBar::addAction(const QString& text) {
  auto button = new XSideBarButton(text);
  
  buttons_.append(button);
  QToolBar::addWidget(button);

  return button;
}

XSideBarButton* XSideBar::addAction(QChar glyph, const QString& text) {
  auto button = new XSideBarButton(glyph, text);

  buttons_.append(button);
  QToolBar::addWidget(button);

  return button;
}

QWidget* XSideBar::addSpacing(int size) {
  QWidget* spacer = new QWidget(this);
  spacer->setFixedHeight(size);
  QToolBar::addWidget(spacer);
  return spacer;
}

}  // namespace qt
}  // namespace ui
}  // namespace xe
