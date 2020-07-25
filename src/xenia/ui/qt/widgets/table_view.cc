#include "xenia/ui/qt/widgets/table_view.h"

#include <QPainter>

namespace xe {
namespace ui {
namespace qt {

XTableView::XTableView(QWidget* parent)
    : Themeable<QTableView>("XTableView", parent),
      col_header_(new XTableHeader(Qt::Horizontal, this)) {
  setContentsMargins(0,0,0,0);
  setHorizontalHeader(col_header_);
  QFont font("Segoe UI Semibold");
  font.setPixelSize(12);
  setFont(font);
  setFrameStyle(QFrame::NoFrame);
  setSortingEnabled(true);

  verticalHeader()->setVisible(false);
  verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
  verticalHeader()->setDefaultSectionSize(row_size_);

  connect(this,SIGNAL(rowSizeChanged(int)),SLOT(setRowSize(int)));
}

void XTableView::setRowSize(int size) {
  verticalHeader()->setDefaultSectionSize(size);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe