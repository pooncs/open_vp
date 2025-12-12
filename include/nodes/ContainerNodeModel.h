#pragma once
#include <QPushButton>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

namespace OpenFlow::Engine {
class ExecutionEngine;
}

class ContainerNodeModel : public QtNodes::NodeDelegateModel {
  Q_OBJECT
public:
  static void
  setRegistry(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry) {
    _staticRegistry = registry;
  }

  static std::shared_ptr<QtNodes::NodeDelegateModelRegistry> getRegistry() {
    return _staticRegistry.lock();
  }

  ContainerNodeModel() {
    _editButton = new QPushButton("Edit Subgraph");

    // Use static registry if available
    auto reg = _staticRegistry.lock();
    if (!reg) {
      // Fallback or error? For now, create empty registry
      reg = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
    }

    _internalGraph = std::make_shared<QtNodes::DataFlowGraphModel>(reg);
  }

  ContainerNodeModel(
      std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry) {
    _editButton = new QPushButton("Edit Subgraph");
    _internalGraph = std::make_shared<QtNodes::DataFlowGraphModel>(registry);
  }

  virtual ~ContainerNodeModel() = default;

  QWidget *embeddedWidget() override { return _editButton; }

  std::shared_ptr<QtNodes::DataFlowGraphModel> internalGraph() const {
    return _internalGraph;
  }

signals:
  void requestEdit(ContainerNodeModel *model);

protected:
  std::shared_ptr<QtNodes::DataFlowGraphModel> _internalGraph;
  QPushButton *_editButton;

private:
  static std::weak_ptr<QtNodes::NodeDelegateModelRegistry> _staticRegistry;
};
