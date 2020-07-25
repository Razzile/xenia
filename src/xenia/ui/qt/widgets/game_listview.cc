
#include "xenia/ui/qt/widgets/game_listview.h"
#include "xenia/app/library/game_library.h"
#include "xenia/base/string_util.h"
#include "xenia/ui/qt/delegates/game_listview_delegate.h"

#include <QHeaderView>
#include <QIcon>
#include <QMenu>

namespace xe {
namespace ui {
namespace qt {
using app::XGameLibrary;

XGameListView::XGameListView(QWidget* parent) : XTableView(parent) {
  model_ = new XGameLibraryModel();
  proxy_model_ = new QSortFilterProxyModel();

  Build();
  return;
}

void XGameListView::Build() {
  // Properties
  horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
  setShowGrid(false);

  // Delegates
  this->setItemDelegate(new XGameListViewDelegate);

  connect(model_, &XGameLibraryModel::dataChanged, [this]() {
    update();
    setVisible(false);
    resizeColumnsToContents();
    setVisible(true);
  });

  proxy_model_->setSourceModel(model_);
  setModel(proxy_model_);

  horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
          SLOT(customHeaderMenuRequested(QPoint)));

  resizeColumnsToContents();

  menu_ = new QMenu(this);

  auto add_item = [&](const QString& name, GameColumn column) {
    QAction* action = new QAction(name, this);
    action->setCheckable(true);
    action->setChecked(true);

    connect(action, &QAction::triggered, [=]() {
      int pos = (int)column;
      bool hidden = isColumnHidden(pos);
      setColumnHidden(pos, !hidden);
      action->setChecked(hidden);
    });

    menu_->addAction(action);
  };

  // These names could be retrieved from XGameLibraryModel
  add_item("Icon", GameColumn::kIconColumn);
  add_item("Title", GameColumn::kTitleColumn);
  add_item("Title ID", GameColumn::kTitleIdColumn);
  add_item("Media ID", GameColumn::kMediaIdColumn);
  add_item("Path", GameColumn::kPathColumn);
  add_item("Version", GameColumn::kVersionColumn);
  add_item("Genre", GameColumn::kGenreColumn);
  add_item("Release Date", GameColumn::kReleaseDateColumn);
  add_item("Build Date", GameColumn::kBuildDateColumn);
  add_item("Last Played", GameColumn::kLastPlayedColumn);
  add_item("Time Played", GameColumn::kTimePlayedColumn);
  add_item("Achievements", GameColumn::kAchievementsUnlockedColumn);
  add_item("Gamerscore", GameColumn::kGamerscoreUnlockedColumn);
  add_item("Rating", GameColumn::kGameRatingColumn);
  add_item("Region", GameColumn::kGameRegionColumn);
  add_item("Compatibility", GameColumn::kCompatabilityColumn);
  add_item("# Players", GameColumn::kPlayerCountColumn);
}

void XGameListView::RefreshGameList() { model_->refresh(); }

void XGameListView::customHeaderMenuRequested(QPoint pos) {
  menu_->popup(horizontalHeader()->viewport()->mapToGlobal(pos));
}

}  // namespace qt
}  // namespace ui
}  // namespace xe