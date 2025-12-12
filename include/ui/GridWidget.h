#pragma once
#include <QWidget>
#include <QPainter>

// Widget that draws a grid in background
class GridWidget : public QWidget {
  Q_OBJECT
public:
  GridWidget(QWidget *parent = nullptr) : QWidget(parent) {}
  void setGridEnabled(bool enabled) {
    _gridEnabled = enabled;
    update();
  }
  void setGridSize(int size) {
    _gridSize = size;
    update();
  }

protected:
  void paintEvent(QPaintEvent *event) override {
    QWidget::paintEvent(event); // Draw standard background (stylesheet)

    if (_gridEnabled) {
      QPainter painter(this);
      painter.setPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));

      int w = width();
      int h = height();

      for (int x = 0; x < w; x += _gridSize) {
        painter.drawLine(x, 0, x, h);
      }
      for (int y = 0; y < h; y += _gridSize) {
        painter.drawLine(0, y, w, y);
      }
    }
  }

private:
  bool _gridEnabled = false;
  int _gridSize = 10;
};
