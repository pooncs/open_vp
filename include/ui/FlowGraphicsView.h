#pragma once
#include <QtNodes/GraphicsView>
#include <QtNodes/DataFlowGraphModel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

class FlowGraphicsView : public QtNodes::GraphicsView {
  Q_OBJECT
public:
  FlowGraphicsView(QtNodes::BasicGraphicsScene *scene, QWidget *parent = nullptr)
      : QtNodes::GraphicsView(scene, parent) {
    setAcceptDrops(true);
  }

protected:
  void dragEnterEvent(QDragEnterEvent *event) override {
    if (event->mimeData()->hasFormat("application/x-qtlv-node")) {
      event->acceptProposedAction();
    } else {
      QtNodes::GraphicsView::dragEnterEvent(event);
    }
  }

  void dragMoveEvent(QDragMoveEvent *event) override {
    if (event->mimeData()->hasFormat("application/x-qtlv-node")) {
      event->acceptProposedAction();
    } else {
      QtNodes::GraphicsView::dragMoveEvent(event);
    }
  }

  void dropEvent(QDropEvent *event) override {
    if (event->mimeData()->hasFormat("application/x-qtlv-node")) {
      QString nodeType =
          QString::fromUtf8(event->mimeData()->data("application/x-qtlv-node"));

      QPointF pos = mapToScene(event->position().toPoint());
      
      emit nodeDropped(nodeType, pos);
      
      event->acceptProposedAction();
    } else {
      QtNodes::GraphicsView::dropEvent(event);
    }
  }

signals:
  void nodeDropped(QString nodeType, QPointF pos);
};
