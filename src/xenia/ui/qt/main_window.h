#ifndef XENIA_UI_QT_MAINWINDOW_H_
#define XENIA_UI_QT_MAINWINDOW_H_

#include <QMainWindow>

#include "window.h"
#include "xenia/ui/qt/themeable_widget.h"
#include "xenia/ui/qt/widgets/shell.h"

namespace xe {
namespace ui {
namespace qt {
class XStatusBar;

class MainWindow final : public Themeable<QMainWindow>, public virtual QtWindow {
  Q_OBJECT
 public:
  MainWindow(Loop* loop, const std::wstring& title)
      : QtWindow(loop, title), Themeable<QMainWindow>("MainWindow") {
    window_ = this;
  }
  void AddStatusBarWidget(QWidget* widget, bool permanent = false);
  void RemoveStatusBarWidget(QWidget* widget);

  const XStatusBar* status_bar() const { return status_bar_; }

  bool Initialize() override;

protected:
  bool event(QEvent* event) override;


 private:
  XShell* shell_ = nullptr;
  XStatusBar* status_bar_ = nullptr;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif