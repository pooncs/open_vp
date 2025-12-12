#include "WhileLoopNodeModel.h"
#include "BoolData.h"
#include "DecimalData.h"
#include "GraphTunnelNodes.h"
#include "LoopConditionNodeModel.h"
#include "core/ExecutionEngine.h"
#include <iostream>

WhileLoopNodeModel::WhileLoopNodeModel() : ContainerNodeModel() {}

WhileLoopNodeModel::WhileLoopNodeModel(
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry)
    : ContainerNodeModel(registry) {}

QString WhileLoopNodeModel::caption() const { return "While Loop"; }
QString WhileLoopNodeModel::name() const { return "WhileLoop"; }

unsigned int WhileLoopNodeModel::nPorts(QtNodes::PortType portType) const {
  if (portType == QtNodes::PortType::In)
    return 1;
  return 1;
}

QtNodes::NodeDataType
WhileLoopNodeModel::dataType(QtNodes::PortType portType,
                             QtNodes::PortIndex portIndex) const {
  return DecimalData().type();
}

std::shared_ptr<QtNodes::NodeData>
WhileLoopNodeModel::outData(QtNodes::PortIndex port) {
  return _resultData;
}

void WhileLoopNodeModel::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                   QtNodes::PortIndex portIndex) {
  _inputData = data;
}

void WhileLoopNodeModel::execute(OpenFlow::Engine::ExecutionContext &context) {
  // 1. Create Engine for internal graph
  OpenFlow::Engine::ExecutionEngine engine(_internalGraph);
  engine.compile();

  // 2. Find Tunnel Nodes & Condition Node
  auto allNodes = _internalGraph->allNodeIds();
  QtNodes::NodeId inputTunnelId = QtNodes::InvalidNodeId;
  QtNodes::NodeId outputTunnelId = QtNodes::InvalidNodeId;
  LoopConditionNodeModel *conditionNode = nullptr;

  for (auto id : allNodes) {
    auto model = _internalGraph->delegateModel<QtNodes::NodeDelegateModel>(id);
    if (dynamic_cast<GraphInputNodeModel *>(model))
      inputTunnelId = id;
    if (dynamic_cast<GraphOutputNodeModel *>(model))
      outputTunnelId = id;
    if (auto cond = dynamic_cast<LoopConditionNodeModel *>(model))
      conditionNode = cond;
  }

  int iterations = 0;

  while (true) {
    // Check Global Stop
    if (context.isStopRequested()) {
      break;
    }

    // 3. Transfer Input Tunnels
    if (inputTunnelId != QtNodes::InvalidNodeId) {
      auto inputModel = dynamic_cast<GraphInputNodeModel *>(
          _internalGraph->delegateModel<QtNodes::NodeDelegateModel>(
              inputTunnelId));

      if (iterations == 0) {
        if (_inputData)
          inputModel->setExternalData(_inputData);
      } else {
        // Feedback from previous iteration (Shift Register would go here)
        // For now, we just keep the initial data or update if we had feedback
        // logic
      }
    }

    // 4. Run One Iteration
    // This executes the sub-graph synchronously
    engine.execute();

    // 5. Transfer Output Tunnels
    if (outputTunnelId != QtNodes::InvalidNodeId) {
      auto outputModel = dynamic_cast<GraphOutputNodeModel *>(
          _internalGraph->delegateModel<QtNodes::NodeDelegateModel>(
              outputTunnelId));
      _resultData = outputModel->getInternalData();
    }

    // 6. Check Loop Condition
    if (conditionNode && conditionNode->shouldStop()) {
      break;
    }

    iterations++;
  }
}
