#ifndef XENIA_UI_QT_GAME_LISTVIEW_H_
#define XENIA_UI_QT_GAME_LISTVIEW_H_

#include "xenia/ui/qt/models/game_library_model.h"
#include "xenia/ui/qt/themeable_widget.h"
#include "xenia/ui/qt/widgets/table_view.h"

#include <QSortFilterProxyModel>
#include <QTableView>

namespace xe {
namespace ui {
namespace qt {

class XGameListView : public XTableView {
  Q_OBJECT

 public:
  explicit XGameListView(QWidget* parent = nullptr);

  void RefreshGameList();
 public slots:
  void customHeaderMenuRequested(QPoint pos);

 private:
  void Build();

  XGameLibraryModel* model_;
  QSortFilterProxyModel* proxy_model_;
  QMenu* menu_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif