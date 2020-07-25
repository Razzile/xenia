#ifndef XENIA_UI_QT_TOOLBAR_H_
#define XENIA_UI_QT_TOOLBAR_H_

#include "xenia/ui/qt/actions/action.h"
#include "xenia/ui/qt/themeable_widget.h"
#include "xenia/ui/qt/widgets/toolbar_item.h"

#include <QToolBar>
#include <QVector>

namespace xe {
namespace ui {
namespace qt {

class XToolBar : public Themeable<QToolBar> {
  Q_OBJECT
  Q_PROPERTY(int spacing READ getSpacing WRITE setSpacing);

 public:
  explicit XToolBar(QWidget* parent = nullptr);

  XToolBarItem* addAction(XAction* action);
  QWidget* addSpacing(int size = 0);
  void addSeparator();
  QWidget* addWidget(QWidget* widget);

  const int getSpacing() const { return spacing_; }
  void setSpacing(const int& spacing);

 private:
  int spacing_ = 0;
  QVector<QWidget*> spacers_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif