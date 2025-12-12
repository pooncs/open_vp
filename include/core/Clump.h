#pragma once
#include <vector>
#include <QtNodes/Definitions>

namespace OpenFlow::Engine {

using QtNodes::NodeId;

/**
 * @brief A Clump is a sequence of nodes that can be executed serially in a single thread.
 * It represents a dependency chain.
 */
struct Clump {
    int id; // Unique ID for the clump
    std::vector<NodeId> nodes; // Ordered list of nodes to execute
    std::vector<int> dependentClumps; // IDs of clumps that depend on this one
    std::vector<int> dependencyClumps; // IDs of clumps this one depends on
    int incomingCount = 0; // Runtime counter for dependencies satisfied
};

} // namespace OpenFlow::Engine
