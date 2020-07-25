/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2018 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_UI_QT_THEME_CONFIGURATION_H_
#define XENIA_UI_QT_THEME_CONFIGURATION_H_

#include <QColor>
#include <QPair>
#include <QString>
#include <memory>
#include <vector>

namespace xe {
namespace ui {
namespace qt {

using ThemeColor = std::pair<QString, QColor>;

class ThemeConfiguration {
 public:
  ThemeConfiguration() = default;
  ThemeConfiguration(const QString& file) : file_(file) {}
  ThemeConfiguration(QString name, QString desc)
      : name_(name), description_(desc) {}
  // maybe remove this one?
  ThemeConfiguration(QString name, QString desc, std::vector<ThemeColor> colors)
      : name_(name), description_(desc), colors_(colors) {}

  // TODO: change return type to error code?
  bool LoadFromFile();
  bool LoadFromFile(const QString& file);

  bool SaveToFile();
  bool SaveToFile(const QString& file);

  const QString& name() const { return name_; }
  const QString& description() const { return description_; }

  void set_name(const QString& name) { name_ = name; }
  void set_description(const QString& desc) { description_ = desc; }

  const std::vector<ThemeColor>& colors() const { return colors_; }
  QColor ColorForKey(const QString& key) const;

  void add_color(const QString& name, const QColor& color);
  void add_color(const QString& name, const QString& hex);
  void add_color(const QString& name, int r, int g, int b);
  void add_color(const QString& name, int r, int g, int b, int a);

 private:
  QString file_;
  QString name_;
  QString description_;
  std::vector<ThemeColor> colors_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe
#endif  // XENIA_UI_QT_THEME_CONFIGURATION_H_
