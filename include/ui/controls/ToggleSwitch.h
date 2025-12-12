#pragma once
#include "AbstractControl.h"
#include <QMouseEvent>

namespace OpenFlow::UI::Controls {

class ToggleSwitch : public AbstractControl {
  Q_OBJECT
public:
  explicit ToggleSwitch(QWidget *parent = nullptr) : AbstractControl(parent) {
    setFixedSize(40, 60);
  }

  void setValue(bool val) {
    if (_value != val) {
      _value = val;
      update();
      emit valueChanged(val);
    }
  }

  bool value() const { return _value; }

signals:
  void valueChanged(bool val);

protected:
  void paintEvent(QPaintEvent *) override {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    auto s = skin();
    int w = width();
    int h = height();

    // Base plate
    p.setPen(QPen(s->colors().border, 1));
    p.setBrush(s->colors().background.darker(110));
    p.drawRoundedRect(2, 2, w - 4, h - 4, 4, 4);

    // Slot
    p.setBrush(Qt::black);
    p.drawRoundedRect(10, 10, w - 20, h - 20, 10, 10);

    // Lever
    int leverH = h / 2;
    int leverY = _value ? 5 : h - leverH - 5;

    QLinearGradient grad(0, leverY, w, leverY);
    grad.setColorAt(0, s->colors().foreground.lighter(150));
    grad.setColorAt(0.5, s->colors().foreground);
    grad.setColorAt(1, s->colors().foreground.darker(150));

    p.setPen(Qt::NoPen);
    p.setBrush(grad);
    p.drawRoundedRect(5, leverY, w - 10, leverH, 5, 5);

    // Active indicator color if on
    if (_value) {
      p.setBrush(s->colors().accent);
      p.drawEllipse(w / 2 - 3, leverY + leverH / 2 - 3, 6, 6);
    }
  }

  void mousePressEvent(QMouseEvent *e) override {
    setFocus();
    if (e->button() == Qt::LeftButton) {
      setValue(!_value);
    }
  }

  void keyPressEvent(QKeyEvent *e) override {
    if (e->key() == Qt::Key_Space || e->key() == Qt::Key_Enter ||
        e->key() == Qt::Key_Return) {
      setValue(!_value);
    } else {
      AbstractControl::keyPressEvent(e);
    }
  }

private:
  bool _value = false;
};

} // namespace OpenFlow::UI::Controls
