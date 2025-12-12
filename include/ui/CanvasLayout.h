#pragma once
#include <QLayout>
#include <QMap>
#include <QRect>
#include <QVector>

/**
 * @brief Absolute positioning layout manager.
 * Allows items to be placed at specific x,y coordinates.
 * Ignores size hints unless forced.
 */
class CanvasLayout : public QLayout {
  Q_OBJECT
public:
  explicit CanvasLayout(QWidget *parent, int margin = 0);
  ~CanvasLayout();

  // Standard QLayout API
  void addItem(QLayoutItem *item) override;
  int count() const override;
  QLayoutItem *itemAt(int index) const override;
  QLayoutItem *takeAt(int index) override;
  QSize sizeHint() const override;
  QSize minimumSize() const override;
  void setGeometry(const QRect &rect) override;

  // Absolute Positioning API
  void setItemPosition(QLayoutItem *item, const QPoint &pos);
  QPoint itemPosition(QLayoutItem *item) const;

  void addWidget(QWidget *w, const QPoint &pos);

  // Grid Snapping
  void setGridSize(int size) { _gridSize = size; }
  int gridSize() const { return _gridSize; }
  void setSnapToGrid(bool enabled) { _snapEnabled = enabled; }
  bool snapToGrid() const { return _snapEnabled; }

private:
  struct ItemInfo {
    QLayoutItem *item;
    QPoint pos;
  };

  QVector<ItemInfo> _items;
  int _gridSize = 10;
  bool _snapEnabled = false;
};
