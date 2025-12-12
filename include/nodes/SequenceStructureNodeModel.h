#pragma once
#include "ContainerNodeModel.h"
#include "core/DataStore.h"
#include "core/ExecutionEngine.h"

class SequenceStructureNodeModel : public ContainerNodeModel {
public:
  SequenceStructureNodeModel() : ContainerNodeModel() {}
  SequenceStructureNodeModel(
      std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry)
      : ContainerNodeModel(registry) {}

  QString caption() const override { return "Sequence"; }
  QString name() const override { return "SequenceStructure"; }

  unsigned int nPorts(QtNodes::PortType) const override { return 0; }
  QtNodes::NodeDataType dataType(QtNodes::PortType,
                                 QtNodes::PortIndex) const override {
    return QtNodes::NodeDataType();
  }
  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
    return nullptr;
  }
  void setInData(std::shared_ptr<QtNodes::NodeData>,
                 QtNodes::PortIndex) override {}

  void execute(OpenFlow::Engine::DataStore &parentStore) {
    // Just execute the internal graph once
    // In real Sequence, we have multiple frames executed sequentially.
    OpenFlow::Engine::ExecutionEngine engine(_internalGraph);
    engine.compile();
    engine.execute();
  }
};
