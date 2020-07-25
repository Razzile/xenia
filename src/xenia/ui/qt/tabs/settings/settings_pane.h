#ifndef XENIA_UI_QT_SETTINGS_PANE_H_
#define XENIA_UI_QT_SETTINGS_PANE_H_

#include <QWidget>

#include "xenia/base/cvar.h"
#include "xenia/config.h"
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
  void set_widget(QWidget* widget) { widget_ = widget; }

  template <typename T>
  bool update_config_var(cvar::ConfigVar<T>* var, const T& value) const;

  template <typename T>
  bool update_config_var(cvar::ConfigVar<T>* var, QVariant value) const;

 private:
  QChar glyph_;
  QString title_;
  QWidget* widget_ = nullptr;
};

template <typename T>
bool SettingsPane::update_config_var(cvar::ConfigVar<T>* var,
                                     const T& value) const {
  var->SetConfigValue(value);

  config::SaveConfig();

  return true;
}

template <typename T>
bool SettingsPane::update_config_var(cvar::ConfigVar<T>* var,
                                     QVariant value) const {
  // QVariant can't be converted to type T
  if (!value.canConvert<T>()) {
    return false;
  }

  var->SetConfigValue(value.value<T>());

  config::SaveConfig();

  return true;
}

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif
