#pragma once
#include "ContainerNodeModel.h"
#include "DecimalData.h"
#include "core/DataStore.h"
#include "core/ExecutionEngine.h"

class CaseStructureNodeModel : public ContainerNodeModel {
public:
  CaseStructureNodeModel() : ContainerNodeModel() {}
  CaseStructureNodeModel(
      std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry)
      : ContainerNodeModel(registry) {
    // Note: Full Case Structure support requires multiple subgraphs (Cases).
    // Current implementation supports a single subgraph executed if selector is
    // True. Future work: Implement map<CaseValue, Graph> for multi-case
    // support.
  }

  QString caption() const override { return "Case Structure"; }
  QString name() const override { return "CaseStructure"; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    // 1 Selector Input
    return portType == QtNodes::PortType::In ? 1 : 0;
  }

  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override {
    return DecimalData().type(); // Selector (0=False, 1=True)
  }

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override {
    return nullptr;
  }

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex portIndex) override {
    _selector = std::dynamic_pointer_cast<DecimalData>(data);
  }

  void execute(OpenFlow::Engine::DataStore &parentStore) {
    bool condition = false;
    if (_selector && _selector->number() != 0.0)
      condition = true;

    // For prototype: Only execute internal graph if True
    if (condition) {
      OpenFlow::Engine::ExecutionEngine engine(_internalGraph);
      engine.compile();
      engine.execute();
    }
  }

private:
  std::shared_ptr<DecimalData> _selector;
};
