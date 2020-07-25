#ifndef XENIA_UI_QT_SHELL_H_
#define XENIA_UI_QT_SHELL_H_

#include "xenia/ui/qt/themeable_widget.h"
#include "xenia/ui/qt/widgets/nav.h"
#include "xenia/ui/qt/widgets/tab.h"

#include <QKeyEvent>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <map>
#include <vector>

namespace xe {
namespace ui {
namespace qt {

class XShell : public Themeable<QWidget> {
  Q_OBJECT

 public:
  explicit XShell(QMainWindow* window);

 public slots:
  void TabChanged(XTab* tab);

 private:
  QMainWindow* window_;
  QVBoxLayout* layout_;
  XNav* nav_;
  QStackedLayout* tab_stack_;
  std::vector<XTab*> tabs_;

  void Build();
  void BuildNav();
  void BuildTabStack();
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif