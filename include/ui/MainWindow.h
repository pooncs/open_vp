#pragma once
#include "ControlsPalette.h"
#include <QMainWindow>
#include <QToolBar>
#include <QtNodes/Definitions>
#include <map>
#include <stack>

namespace QtNodes {
class DataFlowGraphicsScene;
class GraphicsView;
class DataFlowGraphModel;
} // namespace QtNodes

class FlowGraphicsView;

namespace OpenFlow::Engine {
class ExecutionEngine;
}

class FrontPanel;
class ContainerNodeModel;
class ContextHelp;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow(QWidget *parent = nullptr);

private slots:
  void onSave();
  void onLoad();
  void onLoadSubVI();
  void onShowPalette();
  void onRun();
  void onStop();
  void onHighlightNode(QtNodes::NodeId nodeId);
  void onNodeStatusUpdate(QtNodes::NodeId nodeId, bool error, QString msg);

  void onNodeCreated(QtNodes::NodeId const nodeId);
  void onNodeDeleted(QtNodes::NodeId const nodeId);
  void onEnterContainer(ContainerNodeModel *container);
  void onNavigateUp();

private:
  void setGraphModel(std::shared_ptr<QtNodes::DataFlowGraphModel> model);

  // Probes & Tools
  void setupProbeTool();
  bool eventFilter(QObject *obj, QEvent *event) override;

  // Usability Tools
  void showQuickDrop();
  void showContextHelp();

  std::shared_ptr<QtNodes::DataFlowGraphModel> _currentGraphModel;
  QtNodes::DataFlowGraphicsScene *_scene;
  FlowGraphicsView *_view;
  FrontPanel *_frontPanel;
  ContextHelp *_contextHelp;
  ControlsPalette *_palette;

  std::map<QtNodes::NodeId, QWidget *> _nodeWidgets;

  // Engine
  std::shared_ptr<OpenFlow::Engine::ExecutionEngine> _engine;
  bool _highlightExecution = false;
  QAction *_resumeAction;
  QAction *_runAction;

  // Navigation Stack
  struct NavigationLevel {
    std::shared_ptr<QtNodes::DataFlowGraphModel> graphModel;
    QString name;
  };
  std::stack<NavigationLevel> _navigationStack;

  QToolBar *_navToolBar;
  QAction *_backAction;
};
