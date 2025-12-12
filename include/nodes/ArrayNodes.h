#pragma once
#include "ComplexData.h"
#include <QtNodes/NodeDelegateModel>

class BuildArrayNodeModel : public QtNodes::NodeDelegateModel {
public:
  BuildArrayNodeModel() = default;

  QString caption() const override { return "Build Array"; }
  QString name() const override { return "BuildArray"; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    return (portType == QtNodes::PortType::In)
               ? 2
               : 1; // Fixed 2 inputs for prototype
  }

  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex) const override {
    if (portType == QtNodes::PortType::In)
      return QtNodes::NodeDataType{"any", "Element"};
    return ArrayData().type();
  }

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
    return _result;
  }

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex portIndex) override {
    if (portIndex == 0)
      _input1 = data;
    if (portIndex == 1)
      _input2 = data;

    if (_input1 && _input2) {
      // Check type homogeneity (basic check)
      if (_input1->type().id == _input2->type().id) {
        std::vector<std::shared_ptr<QtNodes::NodeData>> vec = {_input1,
                                                               _input2};
        _result = std::make_shared<ArrayData>(vec, _input1->type());
        Q_EMIT dataUpdated(0);
      }
    }
  }

  QWidget *embeddedWidget() override { return nullptr; }

private:
  std::shared_ptr<QtNodes::NodeData> _input1;
  std::shared_ptr<QtNodes::NodeData> _input2;
  std::shared_ptr<ArrayData> _result;
};

#include "DecimalData.h"

class IndexArrayNodeModel : public QtNodes::NodeDelegateModel {
public:
  IndexArrayNodeModel() = default;

  QString caption() const override { return "Index Array"; }
  QString name() const override { return "IndexArray"; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    // In: Array + Index
    // Out: Element
    return (portType == QtNodes::PortType::In) ? 2 : 1;
  }

  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override {
    if (portType == QtNodes::PortType::In) {
      if (portIndex == 0)
        return ArrayData().type(); // Array
      return QtNodes::NodeDataType{
          "decimal", "Index"}; // Index (using decimal for simplicity)
    }
    return QtNodes::NodeDataType{"any", "Element"};
  }

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
    return _result;
  }

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex portIndex) override {
    if (portIndex == 0) {
      _array = std::dynamic_pointer_cast<ArrayData>(data);
    } else if (portIndex == 1) {
      // Assume decimal for index
      if (auto d = std::dynamic_pointer_cast<DecimalData>(data)) {
        _index = static_cast<int>(d->number());
      }
    }

    if (_array && _index >= 0 &&
        _index < static_cast<int>(_array->elements().size())) {
      _result = _array->elements()[_index];
      Q_EMIT dataUpdated(0);
    } else {
      _result.reset();
      // Emit null update if out of bounds?
    }
  }

  QWidget *embeddedWidget() override { return nullptr; }

private:
  std::shared_ptr<ArrayData> _array;
  int _index = 0;
  std::shared_ptr<QtNodes::NodeData> _result;
};
