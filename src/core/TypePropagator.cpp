#include "TypePropagator.h"
#include "nodes/ArrayNodes.h"
#include "nodes/MathOperationDataModel.h"
#include <QtNodes/NodeDelegateModel>
#include <iostream>
#include <queue>
#include <set>
#include <unordered_map>

namespace OpenFlow::Core {

TypePropagator::TypePropagator(
    std::shared_ptr<QtNodes::DataFlowGraphModel> graphModel)
    : _graphModel(graphModel) {}

bool TypePropagator::propagate() {
  if (!_graphModel)
    return false;

  // Topological Sort / Traversal (Source -> Sink)
  // To correctly propagate types, we must visit nodes in dependency order.
  // This is similar to ExecutionEngine::buildClumps logic.

  auto nodeIds = _graphModel->allNodeIds();
  std::unordered_map<QtNodes::NodeId, int> inDegree;
  std::unordered_map<QtNodes::NodeId, std::vector<QtNodes::NodeId>> adjList;

  for (auto nodeId : nodeIds)
    inDegree[nodeId] = 0;

  for (auto nodeId : nodeIds) {
    auto model = _graphModel->delegateModel<QtNodes::NodeDelegateModel>(nodeId);
    unsigned int nOut = model->nPorts(QtNodes::PortType::Out);
    for (unsigned int i = 0; i < nOut; ++i) {
      auto conns = _graphModel->connections(nodeId, QtNodes::PortType::Out, i);
      for (const auto &conn : conns) {
        adjList[nodeId].push_back(conn.inNodeId);
        inDegree[conn.inNodeId]++;
      }
    }
  }

  std::queue<QtNodes::NodeId> q;
  for (auto nodeId : nodeIds) {
    if (inDegree[nodeId] == 0)
      q.push(nodeId);
  }

  while (!q.empty()) {
    auto u = q.front();
    q.pop();

    // 1. Resolve Output Type for Node 'u' based on its Inputs
    resolveNodeType(u);

    // 2. Validate Outgoing Connections
    auto model = _graphModel->delegateModel<QtNodes::NodeDelegateModel>(u);
    unsigned int nOut = model->nPorts(QtNodes::PortType::Out);
    for (unsigned int i = 0; i < nOut; ++i) {
      auto conns = _graphModel->connections(u, QtNodes::PortType::Out, i);
      for (const auto &conn : conns) {
        if (!checkConnection(conn)) {
          std::cerr << "Type Mismatch on connection from Node " << u << " Port "
                    << i << std::endl;
          return false;
        }
      }
    }

    for (auto v : adjList[u]) {
      inDegree[v]--;
      if (inDegree[v] == 0)
        q.push(v);
    }
  }

  return true;
}

void TypePropagator::resolveNodeType(QtNodes::NodeId nodeId) {
  auto model = _graphModel->delegateModel<QtNodes::NodeDelegateModel>(nodeId);
  if (!model)
    return;

  // Check if it's a polymorphic node (MathOperationDataModel)
  if (auto mathNode = dynamic_cast<MathOperationDataModel *>(model)) {
    // Check Inputs
    // MathOperation has 2 data inputs (0, 1) and 1 error input (2)
    // We only care about 0 and 1 for type resolution

    QtNodes::NodeDataType type1 = {"decimal", "Decimal"}; // Default
    QtNodes::NodeDataType type2 = {"decimal", "Decimal"};

    // Get Input Type from Connected Node
    // Note: model->dataType(In, i) returns what the node EXPECTS, which is
    // "any". We need to find what is CONNECTED to it.

    auto getConnectedType = [&](int portIndex) -> QtNodes::NodeDataType {
      auto conns =
          _graphModel->connections(nodeId, QtNodes::PortType::In, portIndex);
      if (conns.empty())
        return {"decimal", "Decimal"}; // Default if unconnected

      auto conn = *conns.begin();
      auto outNode = _graphModel->delegateModel<QtNodes::NodeDelegateModel>(
          conn.outNodeId);
      return outNode->dataType(QtNodes::PortType::Out, conn.outPortIndex);
    };

    type1 = getConnectedType(0);
    type2 = getConnectedType(1);

    // Logic: Array dominates Scalar
    if (type1.id == "array" || type2.id == "array") {
      mathNode->setResolvedOutputType({"array", "Array"});
    } else {
      mathNode->setResolvedOutputType({"decimal", "Decimal"});
    }
  }
  // Add other polymorphic nodes here (e.g., Array Builders)
}

bool TypePropagator::checkConnection(QtNodes::ConnectionId connectionId) {
  auto outNodeId = connectionId.outNodeId;
  auto inNodeId = connectionId.inNodeId;

  auto outModel =
      _graphModel->delegateModel<QtNodes::NodeDelegateModel>(outNodeId);
  auto inModel =
      _graphModel->delegateModel<QtNodes::NodeDelegateModel>(inNodeId);

  if (!outModel || !inModel)
    return false;

  auto outType =
      outModel->dataType(QtNodes::PortType::Out, connectionId.outPortIndex);
  auto inType =
      inModel->dataType(QtNodes::PortType::In, connectionId.inPortIndex);

  if (inType.id == "any")
    return true; // Polymorphic input accepts anything
  if (outType.id == inType.id)
    return true;

  // Strict Check: No implicit casting for now except Decimal <-> Decimal
  // LabVIEW does Coercion, we enforce strictness for prototype to demonstrate
  // safety.

  return false;
}

} // namespace OpenFlow::Core
