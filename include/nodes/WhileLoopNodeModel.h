#pragma once
#include "ContainerNodeModel.h"
#include "core/DataStore.h"
#include "core/IExecutableNode.h"
#include <memory>

class WhileLoopNodeModel : public ContainerNodeModel,
                           public OpenFlow::Engine::IExecutableNode {
public:
  WhileLoopNodeModel();
  WhileLoopNodeModel(
      std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry);

  QString caption() const override;
  QString name() const override;

  unsigned int nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;
  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex portIndex) override;

  // IExecutableNode implementation
  void execute(OpenFlow::Engine::ExecutionContext &context) override;

private:
  std::shared_ptr<QtNodes::NodeData> _inputData;
  std::shared_ptr<QtNodes::NodeData> _resultData;
};
