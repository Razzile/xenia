#ifndef XENIA_UI_QT_SETTINGS_PANE_H_
#define XENIA_UI_QT_SETTINGS_PANE_H_

#include <QWidget>
#include "xenia/ui/qt/themeable_widget.h"

namespace xe {
namespace ui {
namespace qt {

class SettingsPane : public Themeable<QWidget> {
  Q_OBJECT
 public:
  explicit SettingsPane(QChar glyph, const QString& title,
                        QWidget* parent = nullptr)
      : Themeable<QWidget>("SettingsPane", parent),
        glyph_(glyph),
        title_(title) {}

  virtual ~SettingsPane() = default;

  QChar glyph() const { return glyph_; }
  const QString& title() const { return title_; }

  QWidget* widget() const { return widget_; }

  virtual void Build() = 0;

 protected:
  QWidget* widget_ = nullptr;

 private:
  QChar glyph_;
  QString title_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif