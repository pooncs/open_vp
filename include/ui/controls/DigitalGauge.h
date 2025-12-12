#pragma once
#include "AbstractControl.h"
#include <QMouseEvent>
#include <cmath>

namespace OpenFlow::UI::Controls {

class DigitalGauge : public AbstractControl {
  Q_OBJECT
public:
  explicit DigitalGauge(QWidget *parent = nullptr) : AbstractControl(parent) {
    setFixedSize(100, 100);
  }

  void setValue(double val) {
    double clamped = std::max(_min, std::min(_max, val));
    if (std::abs(_value - clamped) > 0.001) {
      _value = clamped;
      update();
      emit valueChanged(_value);
    }
  }

  double value() const { return _value; }
  void setRange(double min, double max) {
    _min = min;
    _max = max;
    update();
  }

signals:
  void valueChanged(double val);

protected:
  void paintEvent(QPaintEvent *) override {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    auto s = skin();
    int w = width();
    int h = height();
    int cx = w / 2;
    int cy = h / 2;
    int radius = std::min(w, h) / 2 - 5;

    // Background
    p.setPen(QPen(s->colors().border, 2));
    p.setBrush(s->colors().background);
    p.drawEllipse(cx - radius, cy - radius, radius * 2, radius * 2);

    // Arc
    double startAngle = 135.0;
    double spanAngle = 270.0;
    double range = _max - _min;
    double ratio = (_value - _min) / range;

    // Draw Track
    p.setPen(
        QPen(s->colors().border.lighter(120), 8, Qt::SolidLine, Qt::FlatCap));
    p.drawArc(cx - radius + 10, cy - radius + 10, (radius - 10) * 2,
              (radius - 10) * 2, (int)(-startAngle * 16),
              (int)(-spanAngle * 16));

    // Draw Value Arc
    p.setPen(QPen(s->colors().accent, 8, Qt::SolidLine, Qt::FlatCap));
    p.drawArc(cx - radius + 10, cy - radius + 10, (radius - 10) * 2,
              (radius - 10) * 2, (int)((startAngle + 90 + 45) * 16),
              (int)(-ratio * spanAngle *
                    16)); // Coordinate system is weird for drawArc

    // Actually, drawArc uses 1/16th of degree. 0 is 3 o'clock. Positive is
    // counter-clockwise. Let's use rotate logic for needle, it's easier.

    // Needle
    p.save();
    p.translate(cx, cy);
    double needleAngle = 135 + ratio * 270;
    p.rotate(needleAngle);

    p.setPen(Qt::NoPen);
    p.setBrush(s->colors().foreground);
    QPolygon needle;
    needle << QPoint(0, -5) << QPoint(0, 5) << QPoint(radius - 15, 0);
    p.drawPolygon(needle);
    p.restore();

    // Center Cap
    p.setBrush(s->colors().foreground);
    p.drawEllipse(cx - 5, cy - 5, 10, 10);

    // Value Text
    p.setFont(s->font());
    p.setPen(s->colors().foreground);
    p.drawText(QRect(0, h - 25, w, 20), Qt::AlignCenter,
               QString::number(_value, 'f', 1));
  }

  void mouseMoveEvent(QMouseEvent *e) override {
    if (e->buttons() & Qt::LeftButton) {
      // Radial interaction logic could be added here
      // For now, simple vertical drag
      double dy = _lastY - e->y();
      double range = _max - _min;
      double step = range / 100.0;
      setValue(_value + dy * step);
      _lastY = e->y();
    }
  }

  void mousePressEvent(QMouseEvent *e) override {
    setFocus();
    _lastY = e->y();
  }

  void keyPressEvent(QKeyEvent *e) override {
    double range = _max - _min;
    double step = range / 100.0;

    if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Right) {
      setValue(_value + step);
    } else if (e->key() == Qt::Key_Down || e->key() == Qt::Key_Left) {
      setValue(_value - step);
    } else {
      AbstractControl::keyPressEvent(e);
    }
  }

private:
  double _value = 0.0;
  double _min = 0.0;
  double _max = 100.0;
  int _lastY = 0;
};

} // namespace OpenFlow::UI::Controls
