#pragma once
#include "ContainerNodeModel.h"
#include "DecimalData.h"
#include "GraphTunnelNodes.h"
#include "core/DataStore.h"
#include "core/ExecutionEngine.h"

class ForLoopNodeModel : public ContainerNodeModel {
public:
  ForLoopNodeModel() : ContainerNodeModel() {}
  ForLoopNodeModel(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry)
      : ContainerNodeModel(registry) {}

  QString caption() const override { return "For Loop"; }
  QString name() const override { return "ForLoop"; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    // 1 Input (N iterations)
    // 1 Output (Tunnel Result)
    return 1;
  }

  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override {
    return DecimalData().type();
  }

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override {
    return _resultData;
  }

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex portIndex) override {
    _N = std::dynamic_pointer_cast<DecimalData>(data);
  }

  void execute(OpenFlow::Engine::DataStore &parentStore) {
    OpenFlow::Engine::ExecutionEngine engine(_internalGraph);
    engine.compile();

    // 2. Find Tunnel Nodes
    auto allNodes = _internalGraph->allNodeIds();
    QtNodes::NodeId inputTunnelId = QtNodes::InvalidNodeId;
    QtNodes::NodeId outputTunnelId = QtNodes::InvalidNodeId;

    for (auto id : allNodes) {
      auto model =
          _internalGraph->delegateModel<QtNodes::NodeDelegateModel>(id);
      if (dynamic_cast<GraphInputNodeModel *>(model))
        inputTunnelId = id;
      if (dynamic_cast<GraphOutputNodeModel *>(model))
        outputTunnelId = id;
    }

    int iterations = 0;
    int limit = 1; // Default
    if (_N)
      limit = static_cast<int>(_N->number());

    while (iterations < limit) {
      // 3. Auto-Indexing Logic (Mocked for now)
      // If input tunnel is array, take element [i]

      // 4. Run One Iteration
      engine.execute();

      // 5. Auto-Indexing Output Logic (Mocked)
      if (outputTunnelId != QtNodes::InvalidNodeId) {
        auto outputModel = dynamic_cast<GraphOutputNodeModel *>(
            _internalGraph->delegateModel<QtNodes::NodeDelegateModel>(
                outputTunnelId));
        _resultData = outputModel->getInternalData(); // Just Last Value for now
      }

      iterations++;
    }
  }

private:
  std::shared_ptr<DecimalData> _N;
  std::shared_ptr<QtNodes::NodeData> _resultData;
};
