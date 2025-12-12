#pragma once
#include "AbstractControl.h"

namespace OpenFlow::UI::Controls {

class BooleanLED : public AbstractControl {
    Q_OBJECT
public:
    explicit BooleanLED(QWidget* parent = nullptr) : AbstractControl(parent) {
        setFixedSize(24, 24);
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
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        
        auto s = skin();
        int m = 2; // margin
        
        // On color
        QColor onColor = s->colors().accent;
        // Off color (darker version of background or gray)
        QColor offColor = s->colors().border; // Simple choice
        
        p.setPen(QPen(s->colors().foreground, 1));
        
        if (_value) {
            // Glow effect
            QRadialGradient g(width()/2, height()/2, width()/2);
            g.setColorAt(0, onColor.lighter(150));
            g.setColorAt(1, onColor);
            p.setBrush(g);
        } else {
            QRadialGradient g(width()/2, height()/2, width()/2);
            g.setColorAt(0, offColor.lighter(110));
            g.setColorAt(1, offColor);
            p.setBrush(g);
        }
        
        p.drawEllipse(m, m, width()-2*m, height()-2*m);
        
        // Specular highlight (glass look)
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255,255,255, 100));
        p.drawEllipse(width()/3, height()/4, width()/4, height()/4);
    }

private:
    bool _value = false;
};

} // namespace OpenFlow::UI::Controls
