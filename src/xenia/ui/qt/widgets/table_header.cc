#include "xenia/ui/qt/widgets/table_header.h"

#include <QPainter>

namespace xe {
namespace ui {
namespace qt {

XTableHeader::XTableHeader(Qt::Orientation orientation, QWidget* parent)
    : Themeable<QHeaderView>("XTableHeader", orientation, parent) {
  setSectionResizeMode(QHeaderView::ResizeMode::Interactive);
  setSectionsClickable(true);
  setSectionsMovable(true);
  setVerticalScrollMode(ScrollMode::ScrollPerPixel);
}

//void XTableHeader::paintSection(QPainter* painter, const QRect& rect,
//                                int section) const {
//  painter->setRenderHints(QPainter::HighQualityAntialiasing);
//  // QHeaderView::paintSection(painter, rect, section);
//
//  // Paint Background
//  QColor background_color(palette().background().color());
//  painter->setPen(QPen(background_color));
//  painter->setBrush(QBrush(background_color));
//  painter->drawRect(rect);
//
//  // Paint Text
//  painter->setPen(Qt::NoPen);
//  painter->setBrush(palette().foreground());
//  painter->setRenderHints(QPainter::HighQualityAntialiasing);
//  auto header_data = model()->headerData(section, orientation());
//  auto column_text = header_data.value<QString>();
//
//  QFontMetrics measure(font());
//  QPointF text_baseline(rect.left() + margins_.left(),
//                        rect.height() - measure.descent());
//
//  QPainterPath text_path;
//  text_path.addText(text_baseline, font(), column_text);
//  painter->drawPath(text_path);
//
//  // Paint Separator
//  QColor separator_color(palette().windowText().color());
//  painter->setPen(QPen(separator_color));
//
//  double x = rect.x() + rect.width() - 1;
//  double margin = rect.height() - (rect.height() * separator_ratio_);
//  QLine separator(x, margin, x, rect.height() - margin);
//  painter->drawLine(separator);
//
//  auto pa = palette().currentColorGroup();
//}

QSize XTableHeader::sizeHint() const {
  auto parent_rect = parentWidget()->rect();
  if(orientation() == Qt::Horizontal)
    return QSize(parent_rect.width(), 20);
  else
    return QSize(20, parent_rect.height());
}

}  // namespace qt
}  // namespace ui
}  // namespace xe