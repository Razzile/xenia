#ifndef XENIA_UI_QT_ADD_GAME_ACTION_H_
#define XENIA_UI_QT_ADD_GAME_ACTION_H_

#include "xenia/ui/qt/actions/action.h"

namespace xe {
namespace ui {
namespace qt {

class XAddGameAction : public XAction {
  Q_OBJECT

 public:
  explicit XAddGameAction() : XAction() {
    setIconText("Add Game");
    setGlyphIcon(QFont("Segoe MDL2 Assets"), QChar(0xE710));
  }
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif