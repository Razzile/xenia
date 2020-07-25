/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2018 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */
#ifndef XENIA_UI_QT_THEMEMANAGER_H_
#define XENIA_UI_QT_THEMEMANAGER_H_

#include <QColor>
#include <QString>
#include <QVector>

#include "xenia/ui/qt/theme.h"
#include "xenia/vfs/virtual_file_system.h"

namespace xe {
namespace ui {
namespace qt {

class ThemeManager {
 public:
  static ThemeManager& Instance();
  Theme& current_theme() { return themes_.front(); }
  const QVector<Theme>& themes() const { return themes_; }
  const QString& base_style() const;

 private:
  void LoadThemes();
  Theme LoadTheme(const QString& name);
  ThemeManager();

  QVector<Theme> themes_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe
#endif  // XENIA_UI_QT_THEMEMANAGER_H_
