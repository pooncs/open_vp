#pragma once
#include <QtNodes/internal/AbstractConnectionPainter.hpp>
#include <QtNodes/internal/ConnectionGraphicsObject.hpp>
#include <QPainterPath>

class ManhattanConnectionPainter : public QtNodes::AbstractConnectionPainter {
public:
  void paint(QPainter *painter,
             QtNodes::ConnectionGraphicsObject const &cgo) const override {
    // Re-use logic from DefaultConnectionPainter for colors/styles if possible,
    // or copy-paste. Since DefaultConnectionPainter methods are not virtual/protected
    // we have to reimplement paint fully.
    
    // For brevity, I'll just draw the path.
    // In production, we'd copy the full paint() logic from DefaultConnectionPainter
    // but use our path.
    
    auto path = getPainterStroke(cgo);
    
    QPen pen(Qt::yellow, 2);
    if (cgo.isSelected()) {
        pen.setColor(Qt::cyan);
        pen.setWidth(4);
    }
    
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(computePath(cgo));
  }

  QPainterPath getPainterStroke(
      QtNodes::ConnectionGraphicsObject const &cgo) const override {
    QPainterPathStroker stroker;
    stroker.setWidth(10.0);
    return stroker.createStroke(computePath(cgo));
  }

private:
  QPainterPath computePath(QtNodes::ConnectionGraphicsObject const &cgo) const {
    QPointF out = cgo.endPoint(QtNodes::PortType::Out);
    QPointF in = cgo.endPoint(QtNodes::PortType::In);

    QPainterPath path(out);
    
    double midX = (out.x() + in.x()) / 2.0;
    
    // Simple S-shape with straight lines (Manhattan-ish)
    // Out -> Right -> Down/Up -> Right -> In
    
    QPointF p1(midX, out.y());
    QPointF p2(midX, in.y());
    
    path.lineTo(p1);
    path.lineTo(p2);
    path.lineTo(in);
    
    return path;
  }
};
