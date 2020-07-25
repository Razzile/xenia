#ifndef XENIA_UI_QT_TABS_SETTINGS_H_
#define XENIA_UI_QT_TABS_SETTINGS_H_

#include <QHBoxLayout>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QVector>

#include "xenia/ui/qt/widgets/sidebar.h"
#include "xenia/ui/qt/widgets/tab.h"

namespace xe {
namespace ui {
namespace qt {

class SettingsPane;

class SettingsTab : public XTab {
  Q_OBJECT
 public:
  explicit SettingsTab();

 private:
  void Build();
  void BuildSidebar();

  QHBoxLayout* layout_ = nullptr;
  QWidget* sidebar_container_ = nullptr;
  XSideBar* sidebar_ = nullptr;
  QStackedWidget* content_widget_ = nullptr;
  QList<SettingsPane*> settings_panes_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif