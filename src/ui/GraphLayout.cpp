#include "GraphLayout.h"
#include <queue>
#include <map>
#include <iostream>

namespace OpenFlow::UI {

void GraphLayout::arrange(std::shared_ptr<QtNodes::DataFlowGraphModel> model) {
    if (!model) return;
    layoutLayered(model);
}

void GraphLayout::layoutLayered(std::shared_ptr<QtNodes::DataFlowGraphModel> model) {
    auto nodeIds = model->allNodeIds();
    if (nodeIds.empty()) return;

    // 1. Calculate Ranks (Topological Sort)
    std::unordered_map<QtNodes::NodeId, int> inDegree;
    std::unordered_map<QtNodes::NodeId, std::vector<QtNodes::NodeId>> adjList;
    
    for (auto id : nodeIds) inDegree[id] = 0;

    for (auto id : nodeIds) {
        auto delegate = model->delegateModel<QtNodes::NodeDelegateModel>(id);
        unsigned int nOut = delegate->nPorts(QtNodes::PortType::Out);
        for (unsigned int i = 0; i < nOut; ++i) {
            auto conns = model->connections(id, QtNodes::PortType::Out, i);
            for (const auto& conn : conns) {
                adjList[id].push_back(conn.inNodeId);
                inDegree[conn.inNodeId]++;
            }
        }
    }

    std::queue<QtNodes::NodeId> q;
    std::unordered_map<QtNodes::NodeId, int> rank;
    
    for (auto id : nodeIds) {
        if (inDegree[id] == 0) {
            q.push(id);
            rank[id] = 0;
        }
    }

    int maxRank = 0;
    while(!q.empty()) {
        auto u = q.front();
        q.pop();
        
        if (rank[u] > maxRank) maxRank = rank[u];

        for (auto v : adjList[u]) {
            inDegree[v]--;
            if (inDegree[v] == 0) {
                rank[v] = rank[u] + 1;
                q.push(v);
            }
        }
    }
    
    // Handle cycles or disconnected nodes
    for (auto id : nodeIds) {
        if (rank.find(id) == rank.end()) {
            rank[id] = 0; // Default to 0
        }
    }

    // 2. Assign Positions
    std::map<int, std::vector<QtNodes::NodeId>> layers;
    for (auto const& [id, r] : rank) {
        layers[r].push_back(id);
    }

    int startX = 50;
    int startY = 50;
    int xSpacing = 250; // Horizontal spacing between layers
    int ySpacing = 150; // Vertical spacing within layer

    for (auto& [r, nodes] : layers) {
        int y = startY;
        int x = startX + r * xSpacing;
        
        for (auto id : nodes) {
            model->setNodeData(id, QtNodes::NodeRole::Position, QPointF(x, y));
            
            // Get node height to space nicely
            // We can't easily get widget size here without casting or querying geometry
            // Assuming standard size
            y += ySpacing;
        }
    }
}

} // namespace OpenFlow::UI
