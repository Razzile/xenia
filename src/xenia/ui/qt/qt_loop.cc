/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2020 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "qt_loop.h"

#include <QApplication>
#include <QThread>
#include <QTimer>

#include "xenia/base/assert.h"

namespace xe {
namespace ui {

std::unique_ptr<Loop> Loop::Create() { return std::make_unique<qt::QtLoop>(); }

namespace qt {

bool QtLoop::is_on_loop_thread() {
  return QThread::currentThread() == QApplication::instance()->thread();
}

void QtLoop::Post(std::function<void()> fn) { PostDelayed(fn, 0); }

void QtLoop::PostDelayed(std::function<void()> fn, uint64_t delay_millis) {
  // https://riptutorial.com/qt/example/21783/using-qtimer-to-run-code-on-main-thread
  const auto& main_thread = QApplication::instance()->thread();
  QTimer* timer = new QTimer();
  timer->moveToThread(main_thread);
  timer->setSingleShot(true);

  QObject::connect(timer, &QTimer::timeout, [=]() {
    // we are now in the gui thread
    fn();
    timer->deleteLater();
  });
  QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection,
                            Q_ARG(uint64_t, delay_millis));
}

void QtLoop::Quit() {
  // As QtLoop uses Qt's internal event loop, we shouldn't kill it
  assert_always();
}

void QtLoop::AwaitQuit() {
  // As QtLoop uses Qt's internal event loop, it can't be killed
  assert_always();
}

}  // namespace qt
}  // namespace ui
}  // namespace xe