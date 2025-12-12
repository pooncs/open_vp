#pragma once
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModel>
#include <unordered_map>
#include <vector>

namespace OpenFlow::UI {

class GraphLayout {
public:
    static void arrange(std::shared_ptr<QtNodes::DataFlowGraphModel> model);

private:
    static void layoutLayered(std::shared_ptr<QtNodes::DataFlowGraphModel> model);
};

} // namespace OpenFlow::UI
