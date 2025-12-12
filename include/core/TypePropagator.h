#pragma once
#include <QtNodes/DataFlowGraphModel>
#include <memory>
#include <vector>

namespace OpenFlow::Core {

class TypePropagator {
public:
  TypePropagator(std::shared_ptr<QtNodes::DataFlowGraphModel> graphModel);

  // Run type propagation across the entire graph
  // Returns true if graph is valid, false if type errors found
  bool propagate();

  // Check a specific connection
  bool checkConnection(QtNodes::ConnectionId connectionId);

private:
  void resolveNodeType(QtNodes::NodeId nodeId);
  std::shared_ptr<QtNodes::DataFlowGraphModel> _graphModel;
};

} // namespace OpenFlow::Core
