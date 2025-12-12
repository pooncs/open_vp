#pragma once
#include "ContainerNodeModel.h"
#include "GraphTunnelNodes.h"
#include "core/ExecutionEngine.h"
#include "core/IExecutableNode.h"
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

class SubVINodeModel : public ContainerNodeModel,
                       public OpenFlow::Engine::IExecutableNode {
public:
  SubVINodeModel() : ContainerNodeModel() {
    _editButton->setText("Open SubVI");
  }

  QString caption() const override { return QFileInfo(_path).fileName(); }
  QString name() const override { return "SubVI"; }

  void setPath(const QString &path);

  // Dynamic ports based on Graph Input/Output nodes in the loaded graph
  unsigned int nPorts(QtNodes::PortType portType) const override {
    // Need to scan internal graph
    // This is expensive to do every frame, should be cached
    return countTerminals(portType);
  }

  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override {
    // Find corresponding terminal and get its type
    return getTerminalType(portType, portIndex);
  }

  std::shared_ptr<QtNodes::NodeData>
  outData(QtNodes::PortIndex portIndex) override {
    // Return cached result from last execution
    if (portIndex < _outputs.size())
      return _outputs[portIndex];
    return nullptr;
  }

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex portIndex) override {
    if (_inputs.size() <= portIndex)
      _inputs.resize(portIndex + 1);
    _inputs[portIndex] = data;
  }

  void execute(OpenFlow::Engine::ExecutionContext &context) override {
    // 1. Transfer Inputs to GraphInputNodes
    auto inputNodes = getTerminals(QtNodes::PortType::In);
    for (size_t i = 0; i < inputNodes.size() && i < _inputs.size(); ++i) {
      if (auto inputNode = dynamic_cast<GraphInputNodeModel *>(inputNodes[i])) {
        inputNode->setExternalData(_inputs[i]);
      }
    }

    // 2. Execute Internal Graph
    OpenFlow::Engine::ExecutionEngine engine(_internalGraph);
    engine.compile();
    engine.execute(); // Synchronous execution for SubVI

    // 3. Transfer Outputs from GraphOutputNodes
    auto outputNodes = getTerminals(QtNodes::PortType::Out);
    _outputs.resize(outputNodes.size());
    for (size_t i = 0; i < outputNodes.size(); ++i) {
      if (auto outputNode =
              dynamic_cast<GraphOutputNodeModel *>(outputNodes[i])) {
        _outputs[i] = outputNode->getInternalData();
      }
    }
  }

private:
  QString _path;
  std::vector<std::shared_ptr<QtNodes::NodeData>> _inputs;
  std::vector<std::shared_ptr<QtNodes::NodeData>> _outputs;

  // Helper to find Input/Output nodes in order (e.g., by Y position)
  std::vector<QtNodes::NodeDelegateModel *>
  getTerminals(QtNodes::PortType type) const {
    std::vector<QtNodes::NodeDelegateModel *> terminals;
    if (!_internalGraph)
      return terminals;

    auto ids = _internalGraph->allNodeIds();
    for (auto id : ids) {
      auto model =
          _internalGraph->delegateModel<QtNodes::NodeDelegateModel>(id);
      if (type == QtNodes::PortType::In) {
        if (dynamic_cast<GraphInputNodeModel *>(model))
          terminals.push_back(model);
      } else {
        if (dynamic_cast<GraphOutputNodeModel *>(model))
          terminals.push_back(model);
      }
    }
    // Sort by Y position (Top to Bottom)
    // Note: This requires access to geometry which is in the Scene, not Model
    // usually. For prototype, we use creation order (ID order).
    return terminals;
  }

  unsigned int countTerminals(QtNodes::PortType type) const {
    return getTerminals(type).size();
  }

  QtNodes::NodeDataType getTerminalType(QtNodes::PortType type,
                                        QtNodes::PortIndex index) const {
    auto terminals = getTerminals(type);
    if (index < terminals.size()) {
      return terminals[index]->dataType(type == QtNodes::PortType::In
                                            ? QtNodes::PortType::Out
                                            : QtNodes::PortType::In,
                                        0);
      // Note: InputNode has Out port, OutputNode has In port
    }
    return QtNodes::NodeDataType();
  }
};
