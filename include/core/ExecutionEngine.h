#pragma once
#include <QtNodes/DataFlowGraphModel>
#include <atomic>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "Clump.h"
#include "DataStore.h"
#include "IEngineInterface.h"
#include "IExecutableNode.h"
#include "ThreadPool.h"

namespace OpenFlow::Engine {

class ExecutionEngine : public IEngineInterface, public ExecutionContext {
public:
  ExecutionEngine(std::shared_ptr<QtNodes::DataFlowGraphModel> graphModel);
  ~ExecutionEngine();

  // IEngineInterface implementation
  void compile() override;
  std::future<void> executeAsync() override;
  void stop() override;
  bool isRunning() const override { return _running; }

  // ExecutionContext implementation
  bool isStopRequested() const override { return !_running; }

  // Observability
  void setNodeExecutionCallback(
      std::function<void(QtNodes::NodeId)> callback) override {
    _nodeExecutionCallback = callback;
  }

  void setNodeStatusCallback(
      std::function<void(QtNodes::NodeId, bool, QString)> callback) {
    _nodeStatusCallback = callback;
  }

  // Debugging
  void setHighlightExecution(bool enabled) { _highlightExecution = enabled; }
  void setExecutionDelay(int ms) { _executionDelay = ms; }

  // Probes
  std::shared_ptr<QtNodes::NodeData> getNodeData(QtNodes::NodeId nodeId,
                                                 QtNodes::PortIndex portIndex) {
    return _dataStore.nodeData(nodeId, portIndex);
  }

  // Breakpoints
  void toggleBreakpoint(QtNodes::NodeId nodeId);
  void resume();
  void setPausedCallback(std::function<void()> callback) {
    _pausedCallback = callback;
  }

  // Legacy synchronous execute (for tests)
  void execute();

private:
  // Helper to traverse graph and build clumps
  void buildClumps();

  // Execute a single clump
  void executeClump(int clumpId);

  // Execute a single node (wraps the NodeDelegateModel)
  void executeNode(QtNodes::NodeId nodeId);

private:
  std::shared_ptr<QtNodes::DataFlowGraphModel> _graphModel;
  std::vector<Clump> _clumps;
  DataStore _dataStore;

  // Observability
  std::function<void(QtNodes::NodeId)> _nodeExecutionCallback;
  std::function<void(QtNodes::NodeId, bool, QString)> _nodeStatusCallback;

  // Threading
  std::atomic<bool> _running;
  std::unique_ptr<ThreadPool> _workerPool;

  // Runtime State
  struct ClumpState {
    std::atomic<int> dependenciesRemaining;
  };
  std::vector<std::unique_ptr<ClumpState>> _clumpStates;
  std::atomic<int> _activeClumpsCount; // Count of clumps not yet finished
  std::shared_ptr<std::promise<void>> _executionPromise;

  // Debug State
  std::atomic<bool> _highlightExecution = false;
  std::atomic<int> _executionDelay = 0;

  // Breakpoint State
  std::set<QtNodes::NodeId> _breakpoints;
  std::mutex _breakpointMutex;
  std::atomic<bool> _paused = false;
  std::mutex _pauseMutex;
  std::condition_variable _pauseCv;
  std::function<void()> _pausedCallback;
};

} // namespace OpenFlow::Engine
