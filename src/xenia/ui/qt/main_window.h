#ifndef XENIA_UI_QT_MAINWINDOW_H_
#define XENIA_UI_QT_MAINWINDOW_H_

#include "xenia/ui/qt/themeable_widget.h"
#include "xenia/ui/qt/widgets/shell.h"

#include <QMainWindow>

namespace xe {
namespace ui {
namespace qt {
class XStatusBar;

class MainWindow : public Themeable<QMainWindow> {
  Q_OBJECT
 public:
  explicit MainWindow();

  void AddStatusBarWidget(QWidget* widget, bool permanent = false);
  void RemoveStatusBarWidget(QWidget* widget);

  QString window_title() const { return window_title_; }
  const XStatusBar* status_bar() const { return status_bar_; }

 private:
  QString window_title_;
  XShell* shell_ = nullptr;
  XStatusBar* status_bar_ = nullptr;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif