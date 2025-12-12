#pragma once
#include "StyleManager.h"
#include <QAccessible>
#include <QAccessibleWidget>
#include <QPainter>
#include <QWidget>

namespace OpenFlow::UI::Controls {

class AbstractControl : public QWidget {
  Q_OBJECT
public:
  explicit AbstractControl(QWidget *parent = nullptr) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus); // Enable keyboard focus
  }

protected:
  std::shared_ptr<ISkin> skin() const {
    return StyleManager::instance().skin();
  }

  void paintBackground(QPainter &p) {
    auto s = skin();
    p.setPen(QPen(s->colors().border, s->borderThickness()));
    p.setBrush(s->colors().background);
    p.drawRoundedRect(rect().adjusted(1, 1, -1, -1), s->cornerRadius(),
                      s->cornerRadius());

    if (hasFocus()) {
      p.setPen(QPen(s->colors().highlight, 2));
      p.setBrush(Qt::NoBrush);
      p.drawRoundedRect(rect().adjusted(2, 2, -2, -2), s->cornerRadius(),
                        s->cornerRadius());
    }
  }

  // Keyboard navigation stub
  void keyPressEvent(QKeyEvent *event) override {
    // Derived classes should implement arrow key logic
    QWidget::keyPressEvent(event);
  }
};

} // namespace OpenFlow::UI::Controls
