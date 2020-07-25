#ifndef XENIA_UI_QT_TAB_H_
#define XENIA_UI_QT_TAB_H_

#include "xenia/ui/qt/themeable_widget.h"

#include <QString>
#include <QWidget>

#include <QLabel>
#include <QVBoxLayout>

namespace xe {
namespace ui {
namespace qt {

class XTab : public Themeable<QWidget> {
  Q_OBJECT
 public:
  explicit XTab(const QString& tab_name)
      : Themeable<QWidget>("XTab"), tab_name_(tab_name) {}

  explicit XTab(const QString& tab_name, const QString& object_name)
      : Themeable<QWidget>(object_name), tab_name_(tab_name) {}

  const QString& tab_name() const { return tab_name_; }

 private:
  QString tab_name_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif