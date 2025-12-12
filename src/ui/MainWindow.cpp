#include "MainWindow.h"
#include "ContainerNodeModel.h"
#include "FrontPanel.h"
#include "IFrontPanelNode.h"
#include "ProjectExplorer.h"
#include "core/NodeRegistry.h"
#include "ui/FlowGraphicsView.h"
#include "ui/QuickDropDialog.h"

#include "ui/ContextHelp.h"
#include "ui/ManhattanConnectionPainter.h"

#include "ui/ContextHelp.h"
#include "ui/ManhattanConnectionPainter.h"

#include <QAction>
#include <QDockWidget>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>

#include "SubVINodeModel.h"

#include "core/ExecutionEngine.h"
#include "ui/GraphLayout.h"
#include "ui/controls/Skin.h"
#include "ui/controls/StyleManager.h"
#include <QGraphicsColorizeEffect>
#include <QTimer>
#include <QtNodes/internal/NodeGraphicsObject.hpp>

using namespace OpenFlow::UI::Controls;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), _scene(nullptr), _view(nullptr) {
  // Setup Front Panel
  _frontPanel = new FrontPanel();
  connect(
      _frontPanel, &FrontPanel::widgetMoved, this,
      [this](QWidget *w, QPoint p) {
        for (auto const &[id, widget] : _nodeWidgets) {
          if (widget == w) {
            auto model =
                _currentGraphModel->delegateModel<QtNodes::NodeDelegateModel>(
                    id);
            if (auto fpNode = dynamic_cast<IFrontPanelNode *>(model)) {
              fpNode->setFrontPanelPosition(p);
            }
            break;
          }
        }
      });

  // Add Front Panel Dock
  auto frontPanelDock = new QDockWidget("Front Panel", this);
  frontPanelDock->setWidget(_frontPanel);
  addDockWidget(Qt::RightDockWidgetArea, frontPanelDock);

  // Add Context Help Dock
  _contextHelp = new ContextHelp(this);
  addDockWidget(Qt::RightDockWidgetArea, _contextHelp);

  // Add Project Explorer Dock
  auto projectExplorer = new ProjectExplorer(this);
  addDockWidget(Qt::LeftDockWidgetArea, projectExplorer);

  setWindowTitle("OpenFlow IDE");
  resize(1200, 800);

  // Setup Navigation Toolbar
  _navToolBar = addToolBar("Navigation");
  _backAction = _navToolBar->addAction("Back");
  _backAction->setEnabled(false);
  connect(_backAction, &QAction::triggered, this, &MainWindow::onNavigateUp);

  _runAction = _navToolBar->addAction("Run");
  connect(_runAction, &QAction::triggered, this, &MainWindow::onRun);

  _resumeAction = _navToolBar->addAction("Resume");
  _resumeAction->setEnabled(false);
  connect(_resumeAction, &QAction::triggered, [this]() {
    _engine->resume();
    _resumeAction->setEnabled(false);
  });

  QAction *stopAction = _navToolBar->addAction("Stop");
  connect(stopAction, &QAction::triggered, this, &MainWindow::onStop);

  QAction *cleanUpAction = _navToolBar->addAction("Clean Up");
  connect(cleanUpAction, &QAction::triggered,
          [this]() { OpenFlow::UI::GraphLayout::arrange(_currentGraphModel); });

  QAction *highlightAction = _navToolBar->addAction("Highlight");
  highlightAction->setCheckable(true);
  connect(highlightAction, &QAction::triggered,
          [this](bool checked) { _highlightExecution = checked; });

  // Setup Menu
  QMenu *fileMenu = menuBar()->addMenu("File");
  QAction *saveAction = fileMenu->addAction("Save");
  QAction *loadAction = fileMenu->addAction("Load");

  QMenu *viewMenu = menuBar()->addMenu("View");
  QAction *toggleViewAction =
      viewMenu->addAction("Toggle Front Panel / Block Diagram");
  toggleViewAction->setShortcut(QKeySequence("Ctrl+E"));
  connect(toggleViewAction, &QAction::triggered, [this, frontPanelDock]() {
    if (frontPanelDock->isVisible()) {
      frontPanelDock->hide();
    } else {
      frontPanelDock->show();
      frontPanelDock->raise();
    }
  });

  QAction *showFrontPanelAction = viewMenu->addAction("Show Front Panel");
  connect(showFrontPanelAction, &QAction::triggered, _frontPanel,
          &FrontPanel::show);
  connect(showFrontPanelAction, &QAction::triggered, _frontPanel,
          &FrontPanel::raise);

  QAction *quickDropAction = viewMenu->addAction("Quick Drop");
  quickDropAction->setShortcut(QKeySequence("Ctrl+Space"));
  connect(quickDropAction, &QAction::triggered, this,
          &MainWindow::showQuickDrop);

  QAction *contextHelpAction = viewMenu->addAction("Context Help");
  contextHelpAction->setShortcut(QKeySequence("Ctrl+H"));
  connect(contextHelpAction, &QAction::triggered, this,
          &MainWindow::showContextHelp);

  QAction *paletteAction = viewMenu->addAction("Controls Palette");
  connect(paletteAction, &QAction::triggered, this, &MainWindow::onShowPalette);

  // Setup Themes Menu
  QMenu *themeMenu = menuBar()->addMenu("Themes");
  QAction *modernTheme = themeMenu->addAction("Modern");
  connect(modernTheme, &QAction::triggered, []() {
    StyleManager::instance().setSkin(std::make_shared<ModernSkin>());
  });
  QAction *silverTheme = themeMenu->addAction("Silver");
  connect(silverTheme, &QAction::triggered, []() {
    StyleManager::instance().setSkin(std::make_shared<SilverSkin>());
  });
  QAction *darkTheme = themeMenu->addAction("Dark");
  connect(darkTheme, &QAction::triggered, []() {
    StyleManager::instance().setSkin(std::make_shared<DarkSkin>());
  });

  connect(saveAction, &QAction::triggered, this, &MainWindow::onSave);
  connect(loadAction, &QAction::triggered, this, &MainWindow::onLoad);

  QAction *subVIAction = fileMenu->addAction("Import SubVI");
  connect(subVIAction, &QAction::triggered, this, &MainWindow::onLoadSubVI);

  // Setup Registry
  auto registry = OpenFlow::Core::createNodeRegistry();

  _palette = new ControlsPalette(registry, this);

  auto initialModel = std::make_shared<DataFlowGraphModel>(registry);
  setGraphModel(initialModel);
}

void MainWindow::setGraphModel(
    std::shared_ptr<QtNodes::DataFlowGraphModel> model) {
  // Disconnect old model signals if needed
  if (_currentGraphModel) {
    disconnect(_currentGraphModel.get(), &DataFlowGraphModel::nodeCreated, this,
               &MainWindow::onNodeCreated);
    disconnect(_currentGraphModel.get(), &DataFlowGraphModel::nodeDeleted, this,
               &MainWindow::onNodeDeleted);
  }

  _currentGraphModel = model;

  // Connect new model signals
  connect(_currentGraphModel.get(), &DataFlowGraphModel::nodeCreated, this,
          &MainWindow::onNodeCreated);
  connect(_currentGraphModel.get(), &DataFlowGraphModel::nodeDeleted, this,
          &MainWindow::onNodeDeleted);

  // Recreate Scene and View
  if (_scene)
    delete _scene;
  if (_view)
    delete _view;

  _scene = new DataFlowGraphicsScene(*_currentGraphModel, this);

  // Use Orthogonal Routing
  _scene->setConnectionPainter(std::make_unique<ManhattanConnectionPainter>());

  _view = new FlowGraphicsView(_scene);
  connect(_view, &FlowGraphicsView::nodeDropped, this,
          [this](QString nodeType, QPointF pos) {
            auto id = _currentGraphModel->addNode(nodeType);
            if (id != QtNodes::InvalidNodeId) {
              _currentGraphModel->setNodeData(id, QtNodes::NodeRole::Position,
                                              pos);
            }
          });
  setCentralWidget(_view);
  setupProbeTool();

  // Scan existing nodes in new model to populate front panel
  for (auto nodeId : _currentGraphModel->allNodeIds()) {
    onNodeCreated(nodeId);
  }
}

void MainWindow::onSave() {
  if (_scene) {
    _scene->save();
  }
}

void MainWindow::onLoad() {
  if (_scene) {
    _scene->load();
  }
}

void MainWindow::onLoadSubVI() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "Load SubVI", "", "OpenFlow Projects (*.ofproj)");
  if (fileName.isEmpty())
    return;

  // Create a SubVI node
  auto nodeId = _currentGraphModel->addNode("SubVI");
  auto model = _currentGraphModel->delegateModel<SubVINodeModel>(nodeId);
  if (model) {
    model->setPath(fileName);
    // Note: Real implementation needs to load the graph content now to update
    // ports
  }
}

void MainWindow::onRun() {
  // Create Engine
  _engine =
      std::make_shared<OpenFlow::Engine::ExecutionEngine>(_currentGraphModel);
  _engine->compile();

  _engine->setNodeExecutionCallback([this](QtNodes::NodeId nodeId) {
    QMetaObject::invokeMethod(this,
                              [this, nodeId]() { onHighlightNode(nodeId); });
  });

  _engine->setNodeStatusCallback(
      [this](QtNodes::NodeId nodeId, bool error, QString msg) {
        QMetaObject::invokeMethod(this, [this, nodeId, error, msg]() {
          onNodeStatusUpdate(nodeId, error, msg);
        });
      });

  _engine->setPausedCallback([this]() {
    QMetaObject::invokeMethod(this,
                              [this]() { _resumeAction->setEnabled(true); });
  });

  _engine->setHighlightExecution(_highlightExecution);
  _engine->setExecutionDelay(500); // 500ms delay for highlight

  auto future = _engine->executeAsync();
}

void MainWindow::onStop() {
  if (_engine) {
    _engine->stop();
  }
}

#include "BoolData.h"
#include "DecimalData.h"
#include <QGraphicsSceneMouseEvent>
#include <QToolTip>
#include <QtNodes/GraphicsView>

void MainWindow::setupProbeTool() { _view->installEventFilter(this); }

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
  if (obj == _view && event->type() == QEvent::ToolTip) {
    auto helpEvent = static_cast<QHelpEvent *>(event);
    auto item = _view->itemAt(helpEvent->pos());
    if (item) {
      // Try to find a NodeGraphicsObject
      auto nodeGraphics = dynamic_cast<QtNodes::NodeGraphicsObject *>(item);
      if (!nodeGraphics) {
        nodeGraphics =
            dynamic_cast<QtNodes::NodeGraphicsObject *>(item->parentItem());
      }

      if (nodeGraphics) {
        auto model =
            _currentGraphModel->delegateModel<QtNodes::NodeDelegateModel>(
                nodeGraphics->nodeId());
        if (model) {
          _contextHelp->setHelpText(model->caption(),
                                    "Node Name: " + model->name() +
                                        "\n\nDescription: This node performs " +
                                        model->name() + " operation.");
        }
      } else {
        _contextHelp->clearHelp();
      }
    } else {
      _contextHelp->clearHelp();
    }

    // Continue with existing ToolTip logic if needed, or suppress it.
    // Let's keep existing logic for other things.
  }

  // Mouse Click Probe & Breakpoint
  if (obj == _view->viewport() && event->type() == QEvent::MouseButtonPress) {
    auto mouseEvent = static_cast<QMouseEvent *>(event);

    // Probe (Right Click) - IMPROVED
    if (mouseEvent->button() == Qt::RightButton) {
      // Use mapToScene to get accurate scene coordinates
      QPointF scenePos = _view->mapToScene(mouseEvent->pos());

      // Find items at scene pos
      auto items = _scene->items(scenePos);

      for (auto item : items) {
        // Try to find a NodeGraphicsObject
        auto nodeGraphics = dynamic_cast<QtNodes::NodeGraphicsObject *>(item);
        if (!nodeGraphics) {
          // It might be a child item (Port, etc)
          nodeGraphics =
              dynamic_cast<QtNodes::NodeGraphicsObject *>(item->parentItem());
        }

        if (nodeGraphics) {
          auto nodeId = nodeGraphics->nodeId();

          // Determine which port we are closest to, or just show all output
          // data For now, let's show all outputs

          QString tip = "<b>Node Data:</b><br>";
          auto model =
              _currentGraphModel->delegateModel<QtNodes::NodeDelegateModel>(
                  nodeId);
          if (model) {
            unsigned int nOut = model->nPorts(QtNodes::PortType::Out);
            if (nOut == 0)
              tip += "(No Outputs)";

            for (unsigned int i = 0; i < nOut; ++i) {
              auto data = _engine->getNodeData(nodeId, i);
              tip += QString("Port %1: ").arg(i);
              if (data) {
                if (auto d = std::dynamic_pointer_cast<DecimalData>(data)) {
                  tip += "<b>" + d->numberAsText() + "</b>";
                } else if (auto b = std::dynamic_pointer_cast<BoolData>(data)) {
                  tip +=
                      "<b>" + QString(b->value() ? "True" : "False") + "</b>";
                } else {
                  tip += "<i>" + data->type().name + "</i>";
                }
              } else {
                tip += "<i>(null)</i>";
              }
              tip += "<br>";
            }
          }

          QToolTip::showText(mouseEvent->globalPosition().toPoint(), tip,
                             _view);
          return true; // Consumed
        }
      }
    }

    // Breakpoint (Control + Left Click)
    if (mouseEvent->button() == Qt::LeftButton &&
        (mouseEvent->modifiers() & Qt::ControlModifier)) {
      auto items = _view->items(mouseEvent->pos());
      for (auto item : items) {
        auto nodeGraphics = dynamic_cast<QtNodes::NodeGraphicsObject *>(item);
        if (!nodeGraphics)
          nodeGraphics =
              dynamic_cast<QtNodes::NodeGraphicsObject *>(item->parentItem());

        if (nodeGraphics) {
          auto nodeId = nodeGraphics->nodeId();
          _engine->toggleBreakpoint(nodeId);

          // Visual Feedback
          auto effect = new QGraphicsColorizeEffect();
          effect->setColor(Qt::red);
          effect->setStrength(0.5);
          // Toggle visual not easy without state tracking in UI.
          // For prototype, we just flash red.
          nodeGraphics->setGraphicsEffect(effect);
          QTimer::singleShot(200, [nodeGraphics]() {
            nodeGraphics->setGraphicsEffect(nullptr);
          });

          return true;
        }
      }
    }
  }

  return QMainWindow::eventFilter(obj, event);
}

void MainWindow::onHighlightNode(QtNodes::NodeId nodeId) {
  if (!_scene)
    return;
  auto graphicsObj = _scene->nodeGraphicsObject(nodeId);
  if (graphicsObj) {
    auto effect = new QGraphicsColorizeEffect();
    effect->setColor(Qt::yellow);
    effect->setStrength(0.8);
    graphicsObj->setGraphicsEffect(effect);

    QTimer::singleShot(
        200, [graphicsObj]() { graphicsObj->setGraphicsEffect(nullptr); });
  }
}

#include <QCompleter>
#include <QInputDialog>
#include <QStringListModel>

void MainWindow::showQuickDrop() {
  if (!_currentGraphModel)
    return;

  auto registry = _currentGraphModel->dataModelRegistry();
  QuickDropDialog dialog(registry, this);

  // Center on mouse
  dialog.move(QCursor::pos());

  if (dialog.exec() == QDialog::Accepted) {
    QString text = dialog.selectedNodeName();
    if (!text.isEmpty()) {
      // Find center of view
      auto center = _view->mapToScene(_view->viewport()->rect().center());

      auto id = _currentGraphModel->addNode(text);
      if (id != QtNodes::InvalidNodeId) {
        _currentGraphModel->setNodeData(id, QtNodes::NodeRole::Position,
                                        center);
      }
    }
  }
}

void MainWindow::showContextHelp() {
  if (_contextHelp->isVisible())
    _contextHelp->hide();
  else {
    _contextHelp->show();
    _contextHelp->raise();
  }
}

void MainWindow::onNodeStatusUpdate(QtNodes::NodeId nodeId, bool error,
                                    QString msg) {
  if (!_scene)
    return;
  auto graphicsObj = _scene->nodeGraphicsObject(nodeId);
  if (graphicsObj) {
    if (error) {
      auto effect = new QGraphicsColorizeEffect();
      effect->setColor(Qt::red);
      effect->setStrength(0.6);
      graphicsObj->setGraphicsEffect(effect);
      graphicsObj->setToolTip("Error: " + msg);
    } else {
      // Success indicator? Maybe too much visual noise if green everywhere
      // Just clear previous error effects if any (but Highlight might
      // overwrite) For now, only visualize Error
      graphicsObj->setToolTip("Status: OK");
    }
  }
}

void MainWindow::onShowPalette() {
  _palette->show();
  _palette->raise();
}

void MainWindow::onNodeCreated(QtNodes::NodeId const nodeId) {
  auto model =
      _currentGraphModel->delegateModel<QtNodes::NodeDelegateModel>(nodeId);
  if (!model)
    return;

  if (auto fpNode = dynamic_cast<IFrontPanelNode *>(model)) {
    if (QWidget *widget = fpNode->frontPanelWidget()) {
      _frontPanel->addNodeWidget(widget, model->name());
      _nodeWidgets[nodeId] = widget;

      QPoint pos = fpNode->frontPanelPosition();
      if (pos.x() != -1 && pos.y() != -1) {
        _frontPanel->setWidgetPosition(widget, pos);
      }
    }
  }

  // Check if it's a Container Node
  if (auto container = dynamic_cast<ContainerNodeModel *>(model)) {
    connect(container, &ContainerNodeModel::requestEdit, this,
            &MainWindow::onEnterContainer);
  }
}

void MainWindow::onNodeDeleted(QtNodes::NodeId const nodeId) {
  auto it = _nodeWidgets.find(nodeId);
  if (it != _nodeWidgets.end()) {
    _frontPanel->removeNodeWidget(it->second);
    _nodeWidgets.erase(it);
  }
}

void MainWindow::onEnterContainer(ContainerNodeModel *container) {
  if (!container)
    return;

  _navigationStack.push({_currentGraphModel, "Main"});
  _backAction->setEnabled(true);

  setGraphModel(container->internalGraph());
  setWindowTitle("QtLV - Visual Dataflow [SubGraph]");
}

void MainWindow::onNavigateUp() {
  if (_navigationStack.empty())
    return;

  auto prevLevel = _navigationStack.top();
  _navigationStack.pop();

  setGraphModel(prevLevel.graphModel);

  if (_navigationStack.empty()) {
    _backAction->setEnabled(false);
    setWindowTitle("QtLV - Visual Dataflow");
  }
}
