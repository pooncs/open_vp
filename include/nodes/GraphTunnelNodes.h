#pragma once
#include "DecimalData.h"
#include <QtNodes/NodeDelegateModel>

class GraphInputNodeModel : public QtNodes::NodeDelegateModel {
  Q_OBJECT
public:
  GraphInputNodeModel() = default;
  virtual ~GraphInputNodeModel() = default;

  QString caption() const override { return "Input Tunnel"; }
  QString name() const override { return "GraphInput"; }
  bool captionVisible() const override { return true; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    // Output port ONLY (inside the subgraph, it acts as a source)
    return portType == QtNodes::PortType::Out ? 1 : 0;
  }

  QtNodes::NodeDataType dataType(QtNodes::PortType,
                                 QtNodes::PortIndex) const override {
    return DecimalData().type();
  }

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
    return _data;
  }

  void setInData(std::shared_ptr<QtNodes::NodeData>,
                 QtNodes::PortIndex) override {}

  QWidget *embeddedWidget() override { return nullptr; }

  // Helper to set data from the outside world (Parent Graph)
  void setExternalData(std::shared_ptr<QtNodes::NodeData> data) {
    _data = std::dynamic_pointer_cast<DecimalData>(data);
    Q_EMIT dataUpdated(0);
  }

private:
  std::shared_ptr<DecimalData> _data;
};

class GraphOutputNodeModel : public QtNodes::NodeDelegateModel {
  Q_OBJECT
public:
  GraphOutputNodeModel() = default;
  virtual ~GraphOutputNodeModel() = default;

  QString caption() const override { return "Output Tunnel"; }
  QString name() const override { return "GraphOutput"; }
  bool captionVisible() const override { return true; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    // Input port ONLY (inside the subgraph, it acts as a sink)
    return portType == QtNodes::PortType::In ? 1 : 0;
  }

  QtNodes::NodeDataType dataType(QtNodes::PortType,
                                 QtNodes::PortIndex) const override {
    return DecimalData().type();
  }

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
    return nullptr;
  }

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex) override {
    _data = std::dynamic_pointer_cast<DecimalData>(data);
  }

  QWidget *embeddedWidget() override { return nullptr; }

  // Helper to get data for the outside world
  std::shared_ptr<QtNodes::NodeData> getInternalData() const { return _data; }

private:
  std::shared_ptr<DecimalData> _data;
};
