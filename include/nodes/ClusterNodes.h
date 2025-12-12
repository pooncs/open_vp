#pragma once
#include "ComplexData.h"
#include <QtNodes/NodeDelegateModel>

class BundleNodeModel : public QtNodes::NodeDelegateModel {
public:
  BundleNodeModel() = default;
  virtual ~BundleNodeModel() = default;

  QString caption() const override { return "Bundle"; }
  QString name() const override { return "Bundle"; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    if (portType == QtNodes::PortType::In)
      return _inputs.size() > 0 ? _inputs.size() : 2; // Default 2 inputs
    return 1;
  }

  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override {
    if (portType == QtNodes::PortType::In) {
      // Dynamic typing: accept anything
      return QtNodes::NodeDataType{"any", "Any"};
    }
    return ClusterData().type();
  }

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
    return _result;
  }

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex portIndex) override {
    if (_inputs.size() <= portIndex) {
      _inputs.resize(portIndex + 1);
    }
    _inputs[portIndex] = data;

    // Compute Cluster
    bool allSet = true;
    for (const auto &input : _inputs) {
      if (!input)
        allSet = false;
    }

    if (allSet) {
      _result = std::make_shared<ClusterData>(_inputs);
      Q_EMIT dataUpdated(0);
    }
  }

  QWidget *embeddedWidget() override { return nullptr; }

private:
  std::vector<std::shared_ptr<QtNodes::NodeData>> _inputs;
  std::shared_ptr<ClusterData> _result;
};

class UnbundleNodeModel : public QtNodes::NodeDelegateModel {
public:
  UnbundleNodeModel() = default;

  QString caption() const override { return "Unbundle"; }
  QString name() const override { return "Unbundle"; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    if (portType == QtNodes::PortType::In)
      return 1;
    // Dynamic output ports based on cluster size?
    // For simplicity, we assume fixed size or update dynamically if we knew the
    // type Let's default to 2 for now, similar to Bundle default
    return _outputs.size() > 0 ? _outputs.size() : 2;
  }

  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override {
    if (portType == QtNodes::PortType::In) {
      return ClusterData().type();
    }
    // Output type depends on input cluster element
    // Since we don't propagate types in edit time easily without connection,
    // return Any
    return QtNodes::NodeDataType{"any", "Element"};
  }

  std::shared_ptr<QtNodes::NodeData>
  outData(QtNodes::PortIndex portIndex) override {
    if (portIndex < _outputs.size())
      return _outputs[portIndex];
    return nullptr;
  }

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex) override {
    auto cluster = std::dynamic_pointer_cast<ClusterData>(data);
    if (cluster) {
      _outputs = cluster->elements();
      // Emit updates for all outputs
      for (size_t i = 0; i < _outputs.size(); ++i) {
        Q_EMIT dataUpdated(i);
      }
    } else {
      _outputs.clear();
    }
  }

  QWidget *embeddedWidget() override { return nullptr; }

private:
  std::vector<std::shared_ptr<QtNodes::NodeData>> _outputs;
};
