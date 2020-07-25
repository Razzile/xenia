#ifndef XENIA_UI_QT_TABS_LIBRARY_H_
#define XENIA_UI_QT_TABS_LIBRARY_H_

#include "xenia/ui/qt/widgets/game_listview.h"
#include "xenia/ui/qt/widgets/tab.h"
#include "xenia/ui/qt/widgets/toolbar.h"

namespace xe {
namespace ui {
namespace qt {

class LibraryTab : public XTab {
  Q_OBJECT
  public:
    explicit LibraryTab();

  private:
    void Build();
    void BuildToolBar();
    void BuildListView();

    QVBoxLayout* layout_ = nullptr;
    XToolBar* toolbar_ = nullptr;
    XGameListView* list_view_ = nullptr;
    QSlider* slider_ = nullptr;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif