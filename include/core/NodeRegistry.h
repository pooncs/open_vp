#pragma once
#include <QtNodes/NodeDelegateModelRegistry>
#include <memory>

namespace OpenFlow::Core {

std::shared_ptr<QtNodes::NodeDelegateModelRegistry> createNodeRegistry();

}
