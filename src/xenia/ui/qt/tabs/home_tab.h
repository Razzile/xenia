#ifndef XENIA_UI_QT_TABS_HOME_H_
#define XENIA_UI_QT_TABS_HOME_H_

#include <QToolBar>
#include "xenia/ui/qt/widgets/game_listview.h"
#include "xenia/ui/qt/widgets/sidebar.h"
#include "xenia/ui/qt/widgets/sidebar_button.h"
#include "xenia/ui/qt/widgets/tab.h"
#include "xenia/ui/qt/widgets/toolbar.h"

namespace xe {
namespace ui {
namespace qt {

class HomeTab : public XTab {
  Q_OBJECT
 public:
  explicit HomeTab();

 public slots:
  void PlayTriggered();
  void OpenFileTriggered();
  void ImportFolderTriggered();

 private:
  void Build();
  void BuildSidebar();
  void BuildRecentView();

  QHBoxLayout* layout_ = nullptr;
  QWidget* sidebar_ = nullptr;
  XSideBar* sidebar_toolbar_ = nullptr;
  XToolBar* recent_toolbar_ = nullptr;
  XGameListView* list_view_ = nullptr;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif XENIA_UI_QT_TABS_HOME_H_
