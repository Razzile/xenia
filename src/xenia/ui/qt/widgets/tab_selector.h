#ifndef XENIA_UI_QT_TAB_SELECTOR_H_
#define XENIA_UI_QT_TAB_SELECTOR_H_

#include "xenia/ui/qt/widgets/tab.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <map>

namespace xe {
namespace ui {
namespace qt {

class XTabSelector : public Themeable<QWidget> {
  Q_OBJECT

  Q_PROPERTY(QRectF bar_rect READ getBarRect WRITE setBarRect);
  Q_PROPERTY(QColor bar_color READ getBarColor WRITE setBarColor);
  Q_PROPERTY(QFont font READ getFont WRITE setFont);
  Q_PROPERTY(QColor font_color READ getFontColor WRITE setFontColor);
  Q_PROPERTY(int font_size READ getFontSize WRITE setFontSize);
  Q_PROPERTY(int tab_spacing READ getTabSpacing WRITE setTabSpacing);

 public:
  explicit XTabSelector();
  explicit XTabSelector(std::vector<XTab*> tabs);

  void addTab(XTab* tab) {
    tabs_.push_back(tab);
    Rebuild();
  }
  void removeTab(XTab* tab) {
    tabs_.erase(std::remove(tabs_.begin(), tabs_.end(), tab), tabs_.end());  
    Rebuild();
  }
  XTab* getActiveTab() const { return active_tab_; }

  QColor getBarColor() const { return bar_color_; }
  void setBarColor(const QColor& color) { bar_color_ = color; }

  int getBarHeight() const { return bar_height_; }
  void setBarHeight(int height) {
    bar_height_ = height;
    Rebuild();
  }
  int getBarMoveDuration() const { return bar_move_duration_; }
  void setBarMoveDuration(int ms) { bar_move_duration_ = ms; }

  double getBarRatio() const { return bar_ratio_; }
  void setBarRatio(double ratio) {
    bar_ratio_ = ratio;
    Rebuild();
  }
  QRectF getBarRect() const { return bar_rect_; }
  void setBarRect(QRectF rect) { bar_rect_ = rect; }

  int getBarTextGap() const { return bar_text_gap_; }
  void setBarTextGap(int gap) {
    bar_text_gap_ = gap;
    Rebuild();
  }
  QFont getFont() const { return font_; }
  void setFont(const QFont& font) {
    font_ = font;
    Rebuild();
  }
  QColor getFontColor() const { return font_color_; }
  void setFontColor(const QColor& color) { font_color_ = color; }

  int getFontSize() const { return font_size_; }
  void setFontSize(int size) {
    font_size_ = size;
    Rebuild();
  }
  int getTabSpacing() const { return tab_spacing_; }
  void setTabSpacing(int spacing) {
    tab_spacing_ = spacing;
    Rebuild();
  }
  std::vector<XTab*> getTabs() const { return tabs_; }

  void Rebuild() { needs_build_ = true; }

 signals:
  void TabChanged(XTab* tab);

 public slots:
  void SetTab(XTab* tab);
  void SetTabIndex(int tab_index);

 protected:
  void mousePressEvent(QMouseEvent* event) override;
  void paintEvent(QPaintEvent*) override;

 private:
  void Build();
  void MoveBarRect(const QRectF& rect);
  QRectF GetBarRect(XTab* tab);

  bool needs_build_ = true;
  XTab* active_tab_ = nullptr;
  QColor bar_color_;
  int bar_height_ = 2;
  int bar_move_duration_ = 150;
  QRectF bar_rect_;
  double bar_ratio_ = 0.6;
  int bar_text_gap_ = 5;
  QFont font_ = QFont("Segoe UI");
  QColor font_color_ = Qt::white;
  int font_size_ = 24;
  int tab_spacing_ = 20;
  std::vector<XTab*> tabs_;
  std::map<XTab*, QRectF> tab_map_;
};

}  // namespace qt
}  // namespace ui
}  // namespace xe

#endif