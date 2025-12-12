#pragma once
#include <QGroupBox>
#include <functional>
#include <QMouseEvent>

// Draggable GroupBox wrapper
class DraggableGroup : public QGroupBox {
  Q_OBJECT
public:
  using MoveCallback = std::function<void(QPoint)>;
  using SelectCallback = std::function<void(DraggableGroup *, bool)>;

  DraggableGroup(const QString &title, QWidget *parent, MoveCallback moveCb,
                 SelectCallback selectCb)
      : QGroupBox(title, parent), _onMove(moveCb), _onSelect(selectCb) {}

  void setSelected(bool selected) {
    if (selected) {
      setStyleSheet("QGroupBox { border: 2px solid #0078d7; font-weight: "
                    "bold; background-color: rgba(0, 120, 215, 0.1); }");
    } else {
      setStyleSheet("");
    }
  }

protected:
  void mousePressEvent(QMouseEvent *event) override {
    if (event->button() == Qt::LeftButton) {
      _dragStartPos = event->pos();
      _isDragging = true;
      raise(); // Bring to front

      if (_onSelect) {
        bool ctrl = (event->modifiers() & Qt::ControlModifier);
        _onSelect(this, ctrl);
      }
    }
    QGroupBox::mousePressEvent(event);
  }

  void mouseMoveEvent(QMouseEvent *event) override {
    if (_isDragging && (event->buttons() & Qt::LeftButton)) {
      move(mapToParent(event->pos() - _dragStartPos));
      if (_onMove)
        _onMove(pos());
    }
    QGroupBox::mouseMoveEvent(event);
  }

  void mouseReleaseEvent(QMouseEvent *event) override {
    if (event->button() == Qt::LeftButton) {
      _isDragging = false;
      // Snap logic is handled by layout update in onMove or explicit snap call
    }
    QGroupBox::mouseReleaseEvent(event);
  }

private:
  QPoint _dragStartPos;
  bool _isDragging = false;
  MoveCallback _onMove;
  SelectCallback _onSelect;
};
