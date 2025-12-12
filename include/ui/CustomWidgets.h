#pragma once
#include <QMouseEvent>
#include <QPainter>
#include <QWidget>

class QKnob : public QWidget {
  Q_OBJECT
public:
  QKnob(QWidget *parent = nullptr) : QWidget(parent) { setFixedSize(60, 60); }

  double value() const { return _value; }
  void setValue(double v) {
    if (_value != v) {
      _value = v;
      update();
      emit valueChanged(_value);
    }
  }

signals:
  void valueChanged(double value);

protected:
  void paintEvent(QPaintEvent *) override {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    // Background
    p.setBrush(QColor(200, 200, 200));
    p.drawEllipse(2, 2, w - 4, h - 4);

    // Indicator
    p.save();
    p.translate(w / 2, h / 2);
    // Map 0..100 to -135..135 degrees
    double angle = -135.0 + (_value / 100.0) * 270.0;
    p.rotate(angle);

    p.setBrush(Qt::black);
    p.drawEllipse(-3, -h / 2 + 8, 6, 6);
    p.restore();
  }

  void mouseMoveEvent(QMouseEvent *e) override {
    if (e->buttons() & Qt::LeftButton) {
      // Simple logic: vertical drag changes value
      double dy = _lastY - e->y();
      double newValue = _value + dy;
      newValue = std::max(0.0, std::min(100.0, newValue));
      setValue(newValue);
      _lastY = e->y();
    }
  }

  void mousePressEvent(QMouseEvent *e) override { _lastY = e->y(); }

private:
  double _value = 0.0;
  int _lastY = 0;
};

class QThermometer : public QWidget {
  Q_OBJECT
public:
  QThermometer(QWidget *parent = nullptr) : QWidget(parent) {
    setFixedSize(30, 100);
  }

  double value() const { return _value; }
  void setValue(double v) {
    if (_value != v) {
      _value = v;
      update();
    }
  }

protected:
  void paintEvent(QPaintEvent *) override {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    // Background
    p.setBrush(Qt::white);
    p.setPen(Qt::black);
    p.drawRect(0, 0, w - 1, h - 1);

    // Fill
    // Map 0..100 to h..0
    double ratio = std::max(0.0, std::min(1.0, _value / 100.0));
    int fillHeight = static_cast<int>(ratio * h);

    p.setBrush(Qt::red);
    p.drawRect(1, h - fillHeight, w - 2, fillHeight);

    // Ticks
    p.setPen(Qt::black);
    for (int i = 0; i <= 4; ++i) {
      int y = h - (i * h / 4);
      p.drawLine(w - 5, y, w, y);
    }
  }

private:
  double _value = 0.0;
};
