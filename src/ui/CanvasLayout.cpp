#include "CanvasLayout.h"
#include <QWidget>

CanvasLayout::CanvasLayout(QWidget *parent, int margin) : QLayout(parent) {
  setContentsMargins(margin, margin, margin, margin);
}

CanvasLayout::~CanvasLayout() {
  QLayoutItem *item;
  while ((item = takeAt(0)))
    delete item;
}

void CanvasLayout::addItem(QLayoutItem *item) {
  // Default position 0,0 if not specified
  _items.append({item, QPoint(0, 0)});
}

void CanvasLayout::addWidget(QWidget *w, const QPoint &pos) {
  QLayout::addWidget(w); // Adds to layout via addItem
  // Update position of the last added item
  if (!_items.isEmpty() && _items.last().item->widget() == w) {
    _items.last().pos = pos;
  }
}

int CanvasLayout::count() const { return _items.size(); }

QLayoutItem *CanvasLayout::itemAt(int index) const {
  if (index >= 0 && index < _items.size())
    return _items.at(index).item;
  return nullptr;
}

QLayoutItem *CanvasLayout::takeAt(int index) {
  if (index >= 0 && index < _items.size()) {
    auto info = _items.takeAt(index);
    return info.item;
  }
  return nullptr;
}

QSize CanvasLayout::sizeHint() const {
  // Calculate bounding box of all items
  int maxX = 0;
  int maxY = 0;
  for (const auto &info : _items) {
    QSize sz = info.item->sizeHint();
    maxX = std::max(maxX, info.pos.x() + sz.width());
    maxY = std::max(maxY, info.pos.y() + sz.height());
  }
  return QSize(maxX + contentsMargins().right(),
               maxY + contentsMargins().bottom());
}

QSize CanvasLayout::minimumSize() const {
  // Same as sizeHint for absolute layout
  return sizeHint();
}

void CanvasLayout::setGeometry(const QRect &rect) {
  QLayout::setGeometry(rect);

  for (const auto &info : _items) {
    QSize sz = info.item->sizeHint();
    // Position relative to parent rect
    int x = rect.x() + info.pos.x();
    int y = rect.y() + info.pos.y();
    info.item->setGeometry(QRect(QPoint(x, y), sz));
  }
}

void CanvasLayout::setItemPosition(QLayoutItem *item, const QPoint &pos) {
  QPoint newPos = pos;
  if (_snapEnabled && _gridSize > 0) {
      // Round to nearest grid point
      int x = qRound(double(pos.x()) / _gridSize) * _gridSize;
      int y = qRound(double(pos.y()) / _gridSize) * _gridSize;
      newPos = QPoint(x, y);
  }

  for (auto &info : _items) {
    if (info.item == item) {
      info.pos = newPos;
      update(); // Trigger relayout
      return;
    }
  }
}

QPoint CanvasLayout::itemPosition(QLayoutItem *item) const {
  for (const auto &info : _items) {
    if (info.item == item)
      return info.pos;
  }
  return QPoint(0, 0);
}
