#ifndef XENIA_UI_QT_TABS_DEBUG_H_
#define XENIA_UI_QT_TABS_DEBUG_H_

#include <QHBoxLayout>
#include <QStackedLayout>
#include <QStackedWidget>

#include "xenia/ui/qt/widgets/sidebar.h"
#include "xenia/ui/qt/widgets/tab.h"

namespace xe {
namespace ui {
namespace qt {

class XTabSelector;

// TODO: should this be in its own file for reusability?
// Represents a sidebar item and a widget that is shown when the item is clicked
struct SidebarItem {
  QChar glyph;
  const char* name;
  QWidget* widget;
};

class DebugTab : public XTab {
  Q_OBJECT
 public:
  explicit DebugTab();

 private:
  void Build();
  void BuildSidebar();

  QWidget* CreateComponentsTab();
  QWidget* CreateNavigationTab();
  QWidget* CreateThemeTab();
  QWidget* CreateLibraryTab();

  // create widgets for "components" tab
  QWidget* CreateButtonGroup();
  QWidget* CreateSliderGroup();
  QWidget* CreateCheckboxGroup();
  QWidget* CreateRadioButtonGroup();
  QWidget* CreateInputGroup();

  // create widgets for "navigation" tab
  QWidget* CreateTab1Widget(XTabSelector* tab_selector,
                            QStackedLayout* tab_stack_layout);
  QWidget* CreateTab2Widget(XTabSelector* tab_selector,
                            QStackedLayout* tab_stack_layout);
  QWidget* CreateTab3Widget(XTabSelector* tab_selector,
                            QStackedLayout* tab_stack_layout);

  QHBoxLayout* layout_ = nullptr;
  QWidget* sidebar_container_ = nullptr;
  XSideBar* sidebar_ = nullptr;
  QStackedWidget* content_widget_ = nullptr;
  QList<SidebarItem> sidebar_items_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif