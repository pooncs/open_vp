#include "FrontPanel.h"
#include "CanvasLayout.h"
#include "ui/DraggableGroup.h"
#include "ui/GridWidget.h"
#include "ui/controls/StyleManager.h"
#include <QGroupBox>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

// DraggableGroup is now in header

FrontPanel::FrontPanel(QWidget *parent) : QWidget(parent) {
  setWindowTitle("Front Panel");
  resize(400, 600);

  auto mainLayout = new QVBoxLayout(this);

  // Toolbar
  auto toolbar = new QWidget(this);
  auto toolbarLayout = new QHBoxLayout(toolbar);
  _editButton = new QPushButton("Edit Mode", toolbar);
  _editButton->setCheckable(true);
  connect(_editButton, &QPushButton::clicked, this,
          &FrontPanel::toggleEditMode);
  toolbarLayout->addWidget(_editButton);

  auto alignLeftBtn = new QPushButton("Left", toolbar);
  connect(alignLeftBtn, &QPushButton::clicked, this, &FrontPanel::alignLeft);
  toolbarLayout->addWidget(alignLeftBtn);

  auto alignTopBtn = new QPushButton("Top", toolbar);
  connect(alignTopBtn, &QPushButton::clicked, this, &FrontPanel::alignTop);
  toolbarLayout->addWidget(alignTopBtn);

  auto alignRightBtn = new QPushButton("Right", toolbar);
  connect(alignRightBtn, &QPushButton::clicked, this, &FrontPanel::alignRight);
  toolbarLayout->addWidget(alignRightBtn);

  auto alignBottomBtn = new QPushButton("Bottom", toolbar);
  connect(alignBottomBtn, &QPushButton::clicked, this,
          &FrontPanel::alignBottom);
  toolbarLayout->addWidget(alignBottomBtn);

  toolbarLayout->addStretch();
  mainLayout->addWidget(toolbar);

  auto scrollArea = new QScrollArea(this);
  scrollArea->setWidgetResizable(true);

  _container = new GridWidget(scrollArea);
  // Use Canvas Layout
  _layout = new CanvasLayout(_container);
  _layout->setGridSize(10);

  scrollArea->setWidget(_container);
  mainLayout->addWidget(scrollArea);
}

void FrontPanel::toggleEditMode() { setEditMode(_editButton->isChecked()); }

void FrontPanel::setEditMode(bool edit) {
  _editMode = edit;
  _editButton->setChecked(edit);

  if (auto grid = qobject_cast<GridWidget *>(_container)) {
    grid->setGridEnabled(edit);
  }

  if (_editMode) {
    _container->setStyleSheet(
        "background-color: #f8f8f8;"); // Removed border to look cleaner with
                                       // grid
    _layout->setSnapToGrid(true);
  } else {
    _container->setStyleSheet("background-color: white;");
    _layout->setSnapToGrid(false);
  }

  // Disable interaction with child widgets in edit mode (to allow dragging)
  // For now, DraggableGroup handles mouse events.
  // In a real implementation, we would install an event filter.

  emit editModeChanged(edit);
}

void FrontPanel::addNodeWidget(QWidget *widget, QString const &label) {
  if (!widget)
    return;

  auto group = new DraggableGroup(
      label, _container,
      [this, widget](QPoint p) {
        // Update layout first
        if (auto layout = qobject_cast<CanvasLayout *>(_layout)) {
          if (auto group = dynamic_cast<DraggableGroup *>(sender())) {
            // This is tricky because sender() might not work in lambda called
            // directly But we know the widget's parent is the group
          }
          // Actually we need to find the group wrapping this widget
          if (widget->parentWidget()) {
            if (auto group =
                    qobject_cast<DraggableGroup *>(widget->parentWidget())) {
              // Find item in layout
              int index = _layout->indexOf(group);
              if (index != -1) {
                _layout->setItemPosition(_layout->itemAt(index), p);
              }
            }
          }
        }
        emit widgetMoved(widget, p);
      },
      [this](DraggableGroup *g, bool ctrl) { selectWidget(g, ctrl); });
  auto groupLayout = new QVBoxLayout(group);
  groupLayout->addWidget(widget);

  // Default position: cascade
  static int offset = 10;
  QPoint pos(offset, offset);
  offset += 20;

  _layout->addWidget(group, pos);
}

void FrontPanel::setWidgetPosition(QWidget *widget, QPoint pos) {
  for (int i = 0; i < _layout->count(); ++i) {
    auto item = _layout->itemAt(i);
    if (auto group = qobject_cast<QGroupBox *>(item->widget())) {
      if (group->findChild<QWidget *>() == widget ||
          widget->parent() == group) {
        _layout->setItemPosition(item, pos);
        return;
      }
    }
  }
}

void FrontPanel::clearSelection() {
  for (auto w : _selectedWidgets) {
    if (auto g = qobject_cast<DraggableGroup *>(w)) {
      g->setSelected(false);
    }
  }
  _selectedWidgets.clear();
}

void FrontPanel::selectWidget(QWidget *widget, bool addToSelection) {
  if (!widget)
    return;

  // Find wrapper
  DraggableGroup *group = qobject_cast<DraggableGroup *>(widget);
  if (!group) {
    // Try parent
    group = qobject_cast<DraggableGroup *>(widget->parentWidget());
  }

  if (!group)
    return; // Can't select non-group widgets

  if (!addToSelection) {
    clearSelection();
  }

  if (!_selectedWidgets.contains(group)) {
    _selectedWidgets.append(group);
    group->setSelected(true);
  } else if (addToSelection) {
    // Toggle off if already selected and ctrl pressed
    _selectedWidgets.removeOne(group);
    group->setSelected(false);
  }
}

void FrontPanel::alignLeft() {
  if (_selectedWidgets.size() < 2)
    return;

  int minX = 100000;
  for (auto w : _selectedWidgets)
    minX = std::min(minX, w->pos().x());

  for (auto w : _selectedWidgets) {
    QPoint p(minX, w->pos().y());
    _layout->setItemPosition(
        dynamic_cast<QLayoutItem *>(_layout->itemAt(_layout->indexOf(w))), p);

    // Notify persistence
    if (auto g = qobject_cast<DraggableGroup *>(w)) {
      if (g->layout() && g->layout()->count() > 0) {
        if (auto inner = g->layout()->itemAt(0)->widget()) {
          emit widgetMoved(inner, p);
        }
      }
    }
  }
}

void FrontPanel::alignTop() {
  if (_selectedWidgets.size() < 2)
    return;

  int minY = 100000;
  for (auto w : _selectedWidgets)
    minY = std::min(minY, w->pos().y());

  for (auto w : _selectedWidgets) {
    QPoint p(w->pos().x(), minY);
    _layout->setItemPosition(
        dynamic_cast<QLayoutItem *>(_layout->itemAt(_layout->indexOf(w))), p);

    // Notify persistence
    if (auto g = qobject_cast<DraggableGroup *>(w)) {
      if (g->layout() && g->layout()->count() > 0) {
        if (auto inner = g->layout()->itemAt(0)->widget()) {
          emit widgetMoved(inner, p);
        }
      }
    }
  }
}

void FrontPanel::alignRight() {
  if (_selectedWidgets.size() < 2)
    return;

  int maxRight = 0;
  for (auto w : _selectedWidgets)
    maxRight = std::max(maxRight, w->pos().x() + w->width());

  for (auto w : _selectedWidgets) {
    int x = maxRight - w->width();
    QPoint p(x, w->pos().y());
    _layout->setItemPosition(
        dynamic_cast<QLayoutItem *>(_layout->itemAt(_layout->indexOf(w))), p);

    // Notify persistence
    if (auto g = qobject_cast<DraggableGroup *>(w)) {
      if (g->layout() && g->layout()->count() > 0) {
        if (auto inner = g->layout()->itemAt(0)->widget()) {
          emit widgetMoved(inner, p);
        }
      }
    }
  }
}

void FrontPanel::alignBottom() {
  if (_selectedWidgets.size() < 2)
    return;

  int maxBottom = 0;
  for (auto w : _selectedWidgets)
    maxBottom = std::max(maxBottom, w->pos().y() + w->height());

  for (auto w : _selectedWidgets) {
    int y = maxBottom - w->height();
    QPoint p(w->pos().x(), y);
    _layout->setItemPosition(
        dynamic_cast<QLayoutItem *>(_layout->itemAt(_layout->indexOf(w))), p);

    // Notify persistence
    if (auto g = qobject_cast<DraggableGroup *>(w)) {
      if (g->layout() && g->layout()->count() > 0) {
        if (auto inner = g->layout()->itemAt(0)->widget()) {
          emit widgetMoved(inner, p);
        }
      }
    }
  }
}

void FrontPanel::removeNodeWidget(QWidget *widget) {
  if (!widget)
    return;

  for (int i = 0; i < _layout->count(); ++i) {
    auto item = _layout->itemAt(i);
    if (auto group = qobject_cast<QGroupBox *>(item->widget())) {
      if (group->findChild<QWidget *>() == widget ||
          widget->parent() == group) {
        _layout->removeItem(item);
        delete group;
        widget->setParent(nullptr);
        return;
      }
    }
  }
}
