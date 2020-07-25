#include "checkbox.h"

#include <QStyleOption>

#include "xenia/ui/qt/theme_manager.h"

namespace xe {
namespace ui {
namespace qt {

XCheckBox::XCheckBox(QWidget* parent)
    : Themeable<QCheckBox>("XCheckBox", parent) {
  setFocusPolicy(Qt::TabFocus);  // disable retaining focus through mouse click
}

void XCheckBox::paintEvent(QPaintEvent* e) {
  QStyleOptionButton option;
  initStyleOption(&option);

  // create rect for indicator box
  // rect must start at 1 as the painter draws either side of start offset so
  // starting at (0,0) would leave 2 sides cut off
  QRectF indicator_box = QRectF(1, 1, 16, 16);

  // get original rect for checkbox label

  QRect label_rect = style()->proxy()->subElementRect(
      QStyle::SE_CheckBoxContents, &option, this);

  QFont font = this->font();

  QFontMetrics metrics(font);
  QRect font_rect = metrics.boundingRect(text());

  // TODO(Razzile): I can't seem to work out why this -1 is needed. I think the
  // Segoe UI font file misreports height of font
  label_rect.setY(indicator_box.center().y() - (font_rect.height() / 2) - 1);
  label_rect.translate(label_indent_, 0);

  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);

  QPen pen(border_color_);

  if (hasFocus()) {
    pen.setColor(focus_color_);
  }
  painter.setFont(font);

  pen.setJoinStyle(Qt::MiterJoin);
  painter.setPen(pen);

  painter.drawRect(indicator_box);

  painter.drawText(label_rect, text());

  // paint checked inner box if checkbox is checked
  if (isChecked()) {
    painter.setPen(Qt::transparent);
    QBrush checked_brush = QBrush(checked_color_);
    QRectF checked_rect =
        QRectF(indicator_box.x() + 2, indicator_box.y() + 2, 12, 12);

    painter.setBrush(checked_brush);
    painter.drawRect(checked_rect);
  }
}

QSize XCheckBox::sizeHint() const {
  // Increase sizeHint by indent amount to compensate for slightly larget
  // indicator box and translated label.
  // This is not exact, but to get it exact would require using an algorithm
  // with QFontMetrics.
  return QCheckBox::sizeHint() + QSize(label_indent_, 0);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe
