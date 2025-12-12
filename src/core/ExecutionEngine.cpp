#include "ExecutionEngine.h"
#include "TypePropagator.h"
#include "nodes/ErrorData.h"
#include <QtNodes/NodeDelegateModel>
#include <iostream>
#include <queue>
#include <set>
#include <unordered_map>

using namespace OpenFlow::Core;

namespace OpenFlow::Engine {

ExecutionEngine::ExecutionEngine(
    std::shared_ptr<QtNodes::DataFlowGraphModel> graphModel)
    : _graphModel(graphModel), _running(false) {
  // Initialize ThreadPool with hardware concurrency
  unsigned int threads = std::thread::hardware_concurrency();
  if (threads == 0)
    threads = 2;
  _workerPool = std::make_unique<ThreadPool>(threads);
}

ExecutionEngine::~ExecutionEngine() { stop(); }

void ExecutionEngine::compile() {
  // 1. Type Propagation (Compiler Pass)
  TypePropagator propagator(_graphModel);
  if (!propagator.propagate()) {
    std::cerr << "Compilation Failed: Type Errors Detected" << std::endl;
    // In a real app, we should throw an exception or return a status
    // For now, we clear the graph or set a flag so execute() does nothing
    _clumps.clear();
    return;
  }

  // 2. Build Execution Graph
  buildClumps();
}

void ExecutionEngine::stop() {
  _running = false;
  // ThreadPool destructor will join threads
  // But we might want to clear pending tasks if possible, but ThreadPool
  // doesn't support that easily. The executeClump check for _running will
  // handle early exit.
}

std::future<void> ExecutionEngine::executeAsync() {
  stop(); // Ensure clean state
  _running = true;

  _executionPromise = std::make_shared<std::promise<void>>();

  if (_clumps.empty()) {
    _executionPromise->set_value();
    return _executionPromise->get_future();
  }

  _activeClumpsCount = _clumps.size();

  // Initialize Runtime State
  _clumpStates.clear();
  for (const auto &clump : _clumps) {
    auto state = std::make_unique<ClumpState>();
    state->dependenciesRemaining = clump.dependencyClumps.size();
    _clumpStates.push_back(std::move(state));
  }

  // Find and schedule roots (clumps with 0 dependencies)
  bool anyScheduled = false;
  for (const auto &clump : _clumps) {
    if (clump.dependencyClumps.empty()) {
      _workerPool->enqueue([this, id = clump.id]() { this->executeClump(id); });
      anyScheduled = true;
    }
  }

  // Handle case where graph has cycles or is weird (shouldn't happen with valid
  // DAG)
  if (!anyScheduled && !_clumps.empty()) {
    // Just force run first one? Or fail.
    // For now, assume DAG.
  }

  return _executionPromise->get_future();
}

// Legacy synchronous execute
void ExecutionEngine::execute() {
  _running = true;
  // This is now broken because we changed architecture,
  // but for backward compatibility we could implement a poor man's sync run
  // by calling executeAsync().wait();
  auto fut = executeAsync();
  fut.wait();
  _running = false;
}

void ExecutionEngine::buildClumps() {
  _clumps.clear();
  if (!_graphModel)
    return;

  auto nodeIds = _graphModel->allNodeIds();

  // Topological Sort (Kahn's Algorithm)
  std::unordered_map<QtNodes::NodeId, int> inDegree;
  std::unordered_map<QtNodes::NodeId, std::vector<QtNodes::NodeId>> adjList;

  // Initialize in-degree
  for (auto nodeId : nodeIds) {
    inDegree[nodeId] = 0;
  }

  // Build Graph
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

  // Queue for 0 in-degree
  std::queue<QtNodes::NodeId> q;
  for (auto nodeId : nodeIds) {
    if (inDegree[nodeId] == 0) {
      q.push(nodeId);
    }
  }

  std::vector<QtNodes::NodeId> sortedNodes;
  while (!q.empty()) {
    auto u = q.front();
    q.pop();
    sortedNodes.push_back(u);

    for (auto v : adjList[u]) {
      inDegree[v]--;
      if (inDegree[v] == 0) {
        q.push(v);
      }
    }
  }

  // Handle Cycles (Nodes not in sortedNodes)
  if (sortedNodes.size() < nodeIds.size()) {
    for (auto nodeId : nodeIds) {
      bool found = false;
      for (auto s : sortedNodes)
        if (s == nodeId)
          found = true;
      if (!found)
        sortedNodes.push_back(nodeId);
    }
  }

  // Build Clumps from Sorted Nodes
  // 1 Clump per Node for now
  int idCounter = 0;
  std::unordered_map<QtNodes::NodeId, int> nodeToClump;

  for (auto nodeId : sortedNodes) {
    Clump c;
    c.id = idCounter;
    c.nodes.push_back(nodeId);
    _clumps.push_back(c);
    nodeToClump[nodeId] = idCounter;
    idCounter++;
  }

  // Populate Clump Dependencies
  for (auto &clump : _clumps) {
    for (auto nodeId : clump.nodes) {
      auto model =
          _graphModel->delegateModel<QtNodes::NodeDelegateModel>(nodeId);
      unsigned int nOut = model->nPorts(QtNodes::PortType::Out);
      for (unsigned int i = 0; i < nOut; ++i) {
        auto conns =
            _graphModel->connections(nodeId, QtNodes::PortType::Out, i);
        for (const auto &conn : conns) {
          if (nodeToClump.find(conn.inNodeId) != nodeToClump.end()) {
            int targetClumpId = nodeToClump[conn.inNodeId];
            if (targetClumpId != clump.id) {
              // Add to dependentClumps (outputs)
              bool exists = false;
              for (int dep : clump.dependentClumps)
                if (dep == targetClumpId)
                  exists = true;
              if (!exists)
                clump.dependentClumps.push_back(targetClumpId);

              // Add to dependencyClumps (inputs) of target
              // Note: modifying _clumps[targetClumpId] is safe as we iterate by
              // reference but accessing other elements vector reallocation
              // shouldn't happen as we reserved/pushed all already. Actually
              // it's safer to just do it on the target clump directly or double
              // loop. Let's rely on the fact that we have all clumps.

              auto &targetClump = _clumps[targetClumpId];
              exists = false;
              for (int dep : targetClump.dependencyClumps)
                if (dep == clump.id)
                  exists = true;
              if (!exists)
                targetClump.dependencyClumps.push_back(clump.id);
            }
          }
        }
      }
    }
  }
}

void ExecutionEngine::executeClump(int clumpId) {
  if (!_running)
    return;

  // Use reference to avoid copy, but be careful if _clumps is modified (it
  // shouldn't be during exec)
  const auto &clump = _clumps[clumpId];

  for (auto nodeId : clump.nodes) {
    if (!_running)
      return;
    executeNode(nodeId);
  }

  // Decrement dependencies of children
  for (int depId : clump.dependentClumps) {
    int remaining = --_clumpStates[depId]->dependenciesRemaining;
    if (remaining == 0) {
      _workerPool->enqueue([this, depId]() { this->executeClump(depId); });
    }
  }

  // Check if all clumps finished
  int remainingActive = --_activeClumpsCount;
  if (remainingActive == 0) {
    _running = false;
    _executionPromise->set_value();
  }
}

void ExecutionEngine::executeNode(QtNodes::NodeId nodeId) {
  // Check Breakpoint
  bool isBreakpoint = false;
  {
    std::lock_guard<std::mutex> lock(_breakpointMutex);
    isBreakpoint = _breakpoints.count(nodeId) > 0;
  }

  if (isBreakpoint) {
    _paused = true;
    if (_pausedCallback)
      _pausedCallback();

    std::unique_lock<std::mutex> lock(_pauseMutex);
    _pauseCv.wait(lock, [this] { return !_paused || !_running; });
  }

  if (_nodeExecutionCallback) {
    _nodeExecutionCallback(nodeId);

    // Debug Highlight Delay
    if (_highlightExecution && _executionDelay > 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(_executionDelay));
    }
  }

  auto model = _graphModel->delegateModel<QtNodes::NodeDelegateModel>(nodeId);
  if (!model)
    return;

  // 1. Set Inputs from DataStore
  unsigned int nIn = model->nPorts(QtNodes::PortType::In);
  for (unsigned int i = 0; i < nIn; ++i) {
    auto conns = _graphModel->connections(nodeId, QtNodes::PortType::In, i);
    if (!conns.empty()) {
      auto conn = *conns.begin();
      auto otherNodeId = conn.outNodeId;
      auto portIndex = conn.outPortIndex;
      auto data = _dataStore.nodeData(otherNodeId, portIndex);
      model->setInData(data, i);
    }
  }

  // 2. Compute (Explicit if supported)
  if (auto execNode = dynamic_cast<IExecutableNode *>(model)) {
    execNode->execute(*this);
  }

  // 3. Get Outputs & Store & Check Status
  bool hasError = false;
  QString errorMessage = "";

  unsigned int nOut = model->nPorts(QtNodes::PortType::Out);
  for (unsigned int i = 0; i < nOut; ++i) {
    auto data = model->outData(i);
    _dataStore.setNodeData(nodeId, i, data);

    // Check for ErrorData
    if (auto errorData = std::dynamic_pointer_cast<ErrorData>(data)) {
      if (errorData->status()) {
        hasError = true;
        errorMessage = errorData->toString();
      }
    }
  }

  // Notify Status
  if (_nodeStatusCallback) {
    _nodeStatusCallback(nodeId, hasError, errorMessage);
  }
}

void ExecutionEngine::toggleBreakpoint(QtNodes::NodeId nodeId) {
  std::lock_guard<std::mutex> lock(_breakpointMutex);
  if (_breakpoints.count(nodeId)) {
    _breakpoints.erase(nodeId);
  } else {
    _breakpoints.insert(nodeId);
  }
}

void ExecutionEngine::resume() {
  {
    std::unique_lock<std::mutex> lock(_pauseMutex);
    _paused = false;
  }
  _pauseCv.notify_all();
}

} // namespace OpenFlow::Engine
