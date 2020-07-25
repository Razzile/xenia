#ifndef XENIA_UI_QT_TABLE_VIEW_H_
#define XENIA_UI_QT_TABLE_VIEW_H_

#include "xenia/ui/qt/themeable_widget.h"
#include "xenia/ui/qt/widgets/table_header.h"

#include <QTableView>

namespace xe {
namespace ui {
namespace qt {

class XTableView : public Themeable<QTableView> {
  Q_OBJECT
  Q_PROPERTY(int rowSize MEMBER row_size_ NOTIFY rowSizeChanged);

 public:
  explicit XTableView(QWidget* parent = nullptr);

 signals:
  void rowSizeChanged(int size);

 public slots:
  void setRowSize(int size);

 private:
  XTableHeader* col_header_;
  XTableHeader* row_header_;

  int row_size_ = 48;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif