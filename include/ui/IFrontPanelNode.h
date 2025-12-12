#pragma once
#include <QWidget>

#include <QPoint>

class IFrontPanelNode {
public:
  virtual ~IFrontPanelNode() = default;

  // Returns the widget to be displayed on the Front Panel.
  // The ownership of the widget is transferred to the caller (Front Panel).
  // Or, simpler: The NodeModel owns the widget, but allows FrontPanel to
  // reparent it.
  virtual QWidget *frontPanelWidget() = 0;

  void setFrontPanelPosition(QPoint p) { _pos = p; }
  QPoint frontPanelPosition() const { return _pos; }

protected:
  QPoint _pos = QPoint(-1, -1);
};
