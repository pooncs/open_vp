#pragma once
#include "AbstractControl.h"
#include <vector>

namespace OpenFlow::UI::Controls {

class WaveformGraph : public AbstractControl {
    Q_OBJECT
public:
    explicit WaveformGraph(QWidget* parent = nullptr) : AbstractControl(parent) {
        setMinimumSize(200, 150);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    
    void setData(const std::vector<double>& data) {
        _data = data;
        update();
    }
    
    void setRange(double minY, double maxY) {
        _minY = minY;
        _maxY = maxY;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        
        paintBackground(p);
        
        if (_data.empty()) return;
        
        auto s = skin();
        int w = width();
        int h = height();
        
        // Grid
        p.setPen(QPen(s->colors().border.lighter(110), 1, Qt::DotLine));
        for(int i=1; i<4; ++i) {
            int y = i * h / 4;
            p.drawLine(0, y, w, y);
            int x = i * w / 4;
            p.drawLine(x, 0, x, h);
        }
        
        // Plot
        p.setPen(QPen(s->colors().accent, 2));
        
        double xStep = (double)w / (_data.size() - 1);
        double rangeY = _maxY - _minY;
        if (rangeY == 0) rangeY = 1.0;
        
        QPolygonF points;
        for(size_t i=0; i<_data.size(); ++i) {
            double x = i * xStep;
            double normalizedY = (_data[i] - _minY) / rangeY;
            double y = h - (normalizedY * h);
            points << QPointF(x, y);
        }
        
        p.drawPolyline(points);
    }

private:
    std::vector<double> _data;
    double _minY = -10.0;
    double _maxY = 10.0;
};

} // namespace OpenFlow::UI::Controls
