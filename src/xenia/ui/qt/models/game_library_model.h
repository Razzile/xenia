#ifndef XENIA_UI_QT_GAME_LIBRARY_MODEL_H_
#define XENIA_UI_QT_GAME_LIBRARY_MODEL_H_

#include "xenia/app/library/game_library.h"

#include <QAbstractTableModel>
#include <memory>

namespace xe {
namespace ui {
namespace qt {
using app::XGameEntry;
using app::XGameLibrary;

enum class GameColumn {
  kIconColumn,
  kTitleColumn,
  kTitleIdColumn,
  kMediaIdColumn,
  kPathColumn,
  kVersionColumn,
  kGenreColumn,
  kReleaseDateColumn,
  kBuildDateColumn,
  kLastPlayedColumn,
  kTimePlayedColumn,
  kAchievementsUnlockedColumn,
  kGamerscoreUnlockedColumn,
  kGameRatingColumn,
  kGameRegionColumn,
  kCompatabilityColumn,
  kPlayerCountColumn,

  kColumnCount,  // For column counting, unused, keep as last entry
};

static std::map<app::XGameRegions, QString> RegionStringMap{
    {XEX_REGION_ALL, "Region Free"},
    {XEX_REGION_NTSCJ, "NTSC-J"},
    {XEX_REGION_NTSCJ_CHINA, "NTSC-J (China)"},
    {XEX_REGION_NTSCJ_JAPAN, "JTSC-J (Japan)"},
    {XEX_REGION_NTSCU, "NTSC-U"},
    {XEX_REGION_OTHER, "Other"},
    {XEX_REGION_PAL, "PAL"},
    {XEX_REGION_PAL_AU_NZ, "PAL (AU/NZ)"},
};

class XGameLibraryModel final : public QAbstractTableModel {
  Q_OBJECT

 public:
  explicit XGameLibraryModel(QObject* parent = nullptr);

  // QAbstractTableModel Implementation
  QVariant data(const QModelIndex& index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  int rowCount(const QModelIndex& parent) const override;
  int columnCount(const QModelIndex& parent) const override;

  void refresh();

 private:
  XGameLibrary* library_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif