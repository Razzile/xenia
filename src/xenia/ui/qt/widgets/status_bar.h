#ifndef XENIA_UI_QT_STATUS_BAR_H_
#define XENIA_UI_QT_STATUS_BAR_H_

#include <QGraphicsEffect>
#include <QStatusBar>
#include "xenia/ui/qt/themeable_widget.h"

namespace xe {
namespace ui {
namespace qt {

class XStatusBar : public Themeable<QStatusBar> {
 public:
     explicit XStatusBar(QWidget* parent = nullptr);
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif
