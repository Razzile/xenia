#ifndef XENIA_UI_QT_NAV_H_
#define XENIA_UI_QT_NAV_H_

#include "xenia/ui/qt/themeable_widget.h"
#include "xenia/ui/qt/widgets/tab.h"
#include "xenia/ui/qt/widgets/tab_selector.h"

#include <QHBoxLayout>
#include <vector>

namespace xe {
namespace ui {
namespace qt {

class XNav : public Themeable<QWidget> {
  Q_OBJECT

 public:
  explicit XNav();

  XTab* active_tab() const { return tab_selector_->getActiveTab(); }
  std::vector<XTab*> tabs() const { return tab_selector_->getTabs(); }

 signals:
  void TabChanged(XTab* tab);

 private:
  QHBoxLayout* layout_;
  QLabel* xenia_icon_;
  XTabSelector* tab_selector_;

  void Build();
  void BuildXeniaIcon();
  void BuildTabs();
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif