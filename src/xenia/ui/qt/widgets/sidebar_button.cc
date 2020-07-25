#include "xenia/ui/qt/widgets/sidebar_button.h"

namespace xe {
namespace ui {
namespace qt {

XSideBarButton::XSideBarButton(const QString& text, QWidget* parent)
    : Themeable<QPushButton>("XSideBarButton", parent), text_(text) {
  setCursor(Qt::PointingHandCursor);
}

XSideBarButton::XSideBarButton(QChar glyph, const QString& text,
                               QWidget* parent)
    : Themeable<QPushButton>("XSideBarButton", parent),
      glyph_(glyph),
      text_(text) {
  setCursor(Qt::PointingHandCursor);
}

void XSideBarButton::paintEvent(QPaintEvent* event) {
  // Call original paintEvent to apply stylesheet
  QPushButton::paintEvent(event);

  QPainter painter(this);
  painter.setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);

  // Draw Glyph
  QFont glyph_font = QFont("Segoe MDL2 Assets Xenia");
  glyph_font.setPixelSize(24);

  // Paint the Glyph
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setFont(glyph_font);
  painter.setPen(QPen(Qt::white));

  QRect glyph_rect = this->rect();
  glyph_rect.translate(20, 0);

  painter.drawText(glyph_rect, Qt::AlignVCenter, glyph_);

  // Draw Text
  QRect text_rect = glyph_rect;
  text_rect.translate(40, 0);

  QFont text_font = QFont("Segoe UI");
  text_font.setPixelSize(20);

  painter.setFont(text_font);
  painter.drawText(text_rect, Qt::AlignVCenter, text_);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe
