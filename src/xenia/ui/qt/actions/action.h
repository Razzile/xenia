#ifndef XENIA_UI_QT_ACTION_H_
#define XENIA_UI_QT_ACTION_H_

#include "xenia/ui/qt/themeable_widget.h"

#include <QAction>
#include <QPixmap>

namespace xe {
namespace ui {
namespace qt {

class XAction : public QAction {
  Q_OBJECT

 public:
  explicit XAction();
  explicit XAction(const QChar& icon, const QString& text);

  void setGlyphIcon(const QFont& font, const QChar& icon);

 private:
  void rebuildGlyphIcons();

  // Glyph Icon
  QChar glyph_char_;
  QPalette glyph_palette_;
  QFont glyph_font_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif