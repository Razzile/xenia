#ifndef XENIA_UI_QT_TABLE_HEADER_H_
#define XENIA_UI_QT_TABLE_HEADER_H_

#include "xenia/ui/qt/themeable_widget.h"

#include <QHeaderView>

namespace xe {
namespace ui {
namespace qt {

class XTableHeader : public Themeable<QHeaderView> {
  Q_OBJECT

 public:
  explicit XTableHeader(Qt::Orientation orientation = Qt::Horizontal,
                        QWidget* parent = nullptr);

 protected:
  /*void paintSection(QPainter* painter, const QRect& rect,
                    int section) const override;*/
  QSize sizeHint() const override;

 private:
  QMargins margins_ = QMargins(5, 0, 0, 0);
  double separator_ratio_ = 0.8;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif