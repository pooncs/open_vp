#pragma once
#include "CanvasLayout.h"
#include <QLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QWidget>

class FrontPanel : public QWidget {
  Q_OBJECT
public:
  FrontPanel(QWidget *parent = nullptr);

  void addNodeWidget(QWidget *widget, QString const &label);
  void removeNodeWidget(QWidget *widget);

  void setEditMode(bool edit);
  bool isEditMode() const { return _editMode; }

  // Set widget position manually (e.g. from loaded file)
  void setWidgetPosition(QWidget *widget, QPoint pos);

  // Selection & Alignment
  void clearSelection();
  void selectWidget(QWidget *widget, bool addToSelection);

signals:
  void editModeChanged(bool edit);
  void widgetMoved(QWidget *widget, QPoint pos);

private slots:
  void toggleEditMode();
  void alignLeft();
  void alignTop();
  void alignRight();
  void alignBottom();

private:
  QWidget *_container;
  CanvasLayout *_layout;
  bool _editMode = false;
  QPushButton *_editButton;
  QVector<QWidget *> _selectedWidgets;
};
