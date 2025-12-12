#pragma once
#include <QtNodes/Definitions>
#include <QtNodes/NodeData>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace OpenFlow::Engine {

class DataStore {
public:
  void setNodeData(QtNodes::NodeId id, QtNodes::PortIndex port,
                   std::shared_ptr<QtNodes::NodeData> data);
  std::shared_ptr<QtNodes::NodeData> nodeData(QtNodes::NodeId id,
                                              QtNodes::PortIndex port);
  void clear();

  // Consume data (move semantics if unique)
  std::shared_ptr<QtNodes::NodeData> takeNodeData(QtNodes::NodeId id,
                                                  QtNodes::PortIndex port);

private:
  std::unordered_map<QtNodes::NodeId,
                     std::unordered_map<QtNodes::PortIndex,
                                        std::shared_ptr<QtNodes::NodeData>>>
      _data;
  std::mutex _mutex;
};

} // namespace OpenFlow::Engine
