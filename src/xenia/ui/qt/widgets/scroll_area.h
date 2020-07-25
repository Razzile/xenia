#ifndef XENIA_UI_QT_SCROLL_AREA_H_
#define XENIA_UI_QT_SCROLL_AREA_H_

#include <QScrollArea>

#include "xenia/ui/qt/themeable_widget.h"

namespace xe {
namespace ui {
namespace qt {

class XScrollArea : public Themeable<QScrollArea> {
  Q_OBJECT
 public:
  explicit XScrollArea(QWidget* parent = nullptr)
      : Themeable<QScrollArea>("XScrollArea", parent) {
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWidgetResizable(true);
  }
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif