#pragma once
#include <QtNodes/Definitions>
#include <functional>
#include <future>

namespace OpenFlow::Engine {

class IEngineInterface {
public:
  virtual ~IEngineInterface() = default;
  virtual void compile() = 0;
  virtual std::future<void> executeAsync() = 0;
  virtual void stop() = 0;
  virtual bool isRunning() const = 0;

  // Observability
  virtual void
  setNodeExecutionCallback(std::function<void(QtNodes::NodeId)> callback) = 0;
};

} // namespace OpenFlow::Engine
