#include <QMenu>
#include <QToolButton>
#include "xenia/ui/qt/themeable_widget.h"

namespace xe {
namespace ui {
namespace qt {

class XDropdownButton : public Themeable<QToolButton> {
  Q_OBJECT
 public:
  explicit XDropdownButton(QWidget* parent = nullptr);
  explicit XDropdownButton(const QString& text, QWidget* parent = nullptr);
  explicit XDropdownButton(const QString& text, QMenu* menu,
                           QWidget* parent = nullptr);

  // TODO: pass-through action slots?

  const QMenu* menu() const { return menu_; }

 private:
  void Build();

  QMenu* menu_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe