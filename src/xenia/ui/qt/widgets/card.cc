#include "xenia/ui/qt/widgets/card.h"
#include <QGraphicsEffect>
#include <QLabel>
#include <QVBoxLayout>

namespace xe {
namespace ui {
namespace qt {

XCard::XCard(QWidget* parent) : Themeable<QWidget>("XCard", parent) { Build(); }

XCard::XCard(const QString& title, QWidget* parent)
    : Themeable<QWidget>("XCard", parent), title_(title) {
  Build();

  Update();
}

void XCard::Build() {
  layout_ = new QGridLayout();
  layout_->setContentsMargins(0, 0, 0, 0);
  layout_->setSpacing(0);
  layout_->setRowStretch(0, 0);
  layout_->setRowStretch(1, 1);
  setLayout(layout_);

  // container for widgets added through AddWidget()
  // needed to allow the card to provide a scrollbar for the widgets
  container_ = new QListWidget(this);
  container_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  container_->setFocusPolicy(Qt::NoFocus);
  layout_->addWidget(container_, 1, 0);

  // Drop shadow around card
  QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect;
  effect->setBlurRadius(16);
  effect->setXOffset(0);
  effect->setYOffset(0);
  effect->setColor(QColor(0, 0, 0, 64));
  setGraphicsEffect(effect);

  if (!title_.isEmpty()) {
    BuildTitle();
  }
}

void XCard::BuildTitle() {
  // Add title to pos(0,0) in grid
  QWidget* title_container = new QWidget();
  title_container->setObjectName("titleContainer");

  QVBoxLayout* title_container_layout = new QVBoxLayout();
  title_container_layout->setContentsMargins(64, 16, 0, 16);
  title_container_layout->setSpacing(0);
  title_container->setLayout(title_container_layout);

  title_label_ = new QLabel(title_);
  title_label_->setFont(QFont("Segoe UI Semibold", 36));

  title_container_layout->addWidget(title_label_);

  layout_->addWidget(title_container, 0, 0);
}

void XCard::Update() {
  if (!title_.isEmpty()) {
    if (!title_label_) {
      BuildTitle();
    }
    title_label_->setText(title_);
  }
}

void XCard::AddWidget(QWidget* widget) {
  // Wrap a widget in a QListWidgetItem to add to container list
  QListWidgetItem* item = new QListWidgetItem();
  item->setBackground(Qt::transparent);
  item->setForeground(Qt::transparent);
  item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
  item->setSizeHint(widget->sizeHint());

  container_->addItem(item);
  container_->setItemWidget(item, widget);
}

}  // namespace qt
}  // namespace ui
}  // namespace xe