#include "DataStore.h"

namespace OpenFlow::Engine {

void DataStore::setNodeData(QtNodes::NodeId id, QtNodes::PortIndex port,
                            std::shared_ptr<QtNodes::NodeData> data) {
  std::lock_guard<std::mutex> lock(_mutex);
  _data[id][port] = data;
}

std::shared_ptr<QtNodes::NodeData>
DataStore::nodeData(QtNodes::NodeId id, QtNodes::PortIndex port) {
  std::lock_guard<std::mutex> lock(_mutex);
  auto it = _data.find(id);
  if (it != _data.end()) {
    auto portIt = it->second.find(port);
    if (portIt != it->second.end()) {
      return portIt->second;
    }
  }
  return nullptr;
}

std::shared_ptr<QtNodes::NodeData>
DataStore::takeNodeData(QtNodes::NodeId id, QtNodes::PortIndex port) {
  std::lock_guard<std::mutex> lock(_mutex);
  auto it = _data.find(id);
  if (it != _data.end()) {
    auto portIt = it->second.find(port);
    if (portIt != it->second.end()) {
      // If we are implementing true buffer reuse, we might want to remove it
      // from the map so the ref count drops. However, in a fan-out scenario,
      // multiple nodes need it. So 'take' implies "I am the last consumer". For
      // now, we return a copy of the shared_ptr. The consumer can check usage
      // count. Ideally, the Scheduler knows if this is the last consumer.
      return portIt->second;
    }
  }
  return nullptr;
}

void DataStore::clear() {
  std::lock_guard<std::mutex> lock(_mutex);
  _data.clear();
}

} // namespace OpenFlow::Engine
