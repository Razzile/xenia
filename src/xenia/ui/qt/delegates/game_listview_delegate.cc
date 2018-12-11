#include "xenia/ui/qt/delegates/game_listview_delegate.h"
#include "xenia/ui/qt/models/game_library_model.h"

#include <QBitmap>
#include <QImage>
#include <QPainter>

namespace xe {
namespace ui {
namespace qt {

XGameListViewDelegate::XGameListViewDelegate(QWidget* parent)
    : QStyledItemDelegate(parent) {
  QImage mask_image(":resources/graphics/GameIconMask.png");
  icon_mask_ = QPixmap::fromImage(mask_image.createAlphaMask());
}

void XGameListViewDelegate::paint(QPainter* painter,
                                  const QStyleOptionViewItem& option,
                                  const QModelIndex& index) const {
  // Override options
  auto options = QStyleOptionViewItem(option);
  options.state &= (~QStyle::State_HasFocus);

  switch (index.column()) {
    case XGameLibraryModel::kIconColumn: {
      QStyledItemDelegate::paint(painter, options, index);
      QImage icon = index.data().value<QImage>();
      QPixmap pixmap = QPixmap::fromImage(icon);

      pixmap.setDevicePixelRatio(painter->device()->devicePixelRatioF());
      paintIcon(pixmap, painter, options, index);
    } break;
    default:
      QStyledItemDelegate::paint(painter, options, index);
  }
}

void XGameListViewDelegate::paintIcon(QPixmap& icon, QPainter* painter,
                                      const QStyleOptionViewItem& options,
                                      const QModelIndex& index) const {
  // Get the column bounds
  double width = options.rect.width();
  double height = options.rect.height();
  double icon_size = options.rect.height() * 0.8;

  icon.setMask(icon_mask_);

  // Calculate the Icon position
  QRectF rect = icon.rect();
  QRectF icon_rect = QRectF(rect.x(), rect.y(), icon_size, icon_size);
  double shift_x = (width - icon_size) / 2;
  double shift_y = (height - icon_size) / 2 + options.rect.y();
  icon_rect.translate(shift_x, shift_y);

  // adding QPainter::Antialiasing here smoothes masked edges 
  // but makes the image look slightly blurry
  painter->setRenderHints(QPainter::SmoothPixmapTransform);

  painter->drawPixmap(icon_rect, icon, icon.rect());
}

QSize XGameListViewDelegate::sizeHint(const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const {
  switch (index.column()) {
    case XGameLibraryModel::kIconColumn:
      return QSize(58, 48);
    default:
      return QSize();
  }
}

}  // namespace qt
}  // namespace ui
}  // namespace xe
