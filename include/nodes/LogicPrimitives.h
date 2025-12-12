#pragma once
#include "BoolData.h"
#include "ErrorData.h"
#include <QtNodes/NodeDelegateModel>

class LogicNodeModel : public QtNodes::NodeDelegateModel {
  // Q_OBJECT
public:
  virtual unsigned int nInputs() const = 0;

  unsigned int nPorts(QtNodes::PortType portType) const override {
    // Inputs: nInputs + 1 ErrorIn
    // Outputs: 1 Result + 1 ErrorOut
    return (portType == QtNodes::PortType::In) ? nInputs() + 1 : 2;
  }
  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override {
    if (portType == QtNodes::PortType::In) {
      if (portIndex == nInputs())
        return ErrorData().type();
      return BoolData().type();
    } else {
      if (portIndex == 1)
        return ErrorData().type();
      return BoolData().type();
    }
  }
  std::shared_ptr<QtNodes::NodeData>
  outData(QtNodes::PortIndex portIndex) override {
    if (portIndex == 1)
      return _errorOut;
    return _result;
  }
  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex portIndex) override {
    if (portIndex == nInputs()) {
      _errorIn = std::dynamic_pointer_cast<ErrorData>(data);
    } else {
      auto boolData = std::dynamic_pointer_cast<BoolData>(data);
      if (portIndex == 0)
        _input1 = boolData;
      if (portIndex == 1)
        _input2 = boolData;
    }

    if (_errorIn && _errorIn->status()) {
      _errorOut = _errorIn;
      _result.reset();
    } else {
      _errorOut = std::make_shared<ErrorData>();
      compute();
    }
    Q_EMIT dataUpdated(0);
    Q_EMIT dataUpdated(1);
  }
  QWidget *embeddedWidget() override { return nullptr; }

protected:
  virtual void compute() = 0;
  std::shared_ptr<BoolData> _input1;
  std::shared_ptr<BoolData> _input2;
  std::shared_ptr<BoolData> _result;
  std::shared_ptr<ErrorData> _errorIn;
  std::shared_ptr<ErrorData> _errorOut;
};

class AndModel : public LogicNodeModel {
public:
  QString caption() const override { return "AND"; }
  QString name() const override { return "And"; }
  unsigned int nInputs() const override { return 2; }

protected:
  void compute() override {
    if (_input1 && _input2)
      _result =
          std::make_shared<BoolData>(_input1->value() && _input2->value());
    else
      _result.reset();
  }
};

class OrModel : public LogicNodeModel {
public:
  QString caption() const override { return "OR"; }
  QString name() const override { return "Or"; }
  unsigned int nInputs() const override { return 2; }

protected:
  void compute() override {
    if (_input1 && _input2)
      _result =
          std::make_shared<BoolData>(_input1->value() || _input2->value());
    else
      _result.reset();
  }
};

class NotModel : public LogicNodeModel {
public:
  QString caption() const override { return "NOT"; }
  QString name() const override { return "Not"; }
  unsigned int nInputs() const override { return 1; }

protected:
  void compute() override {
    if (_input1)
      _result = std::make_shared<BoolData>(!_input1->value());
    else
      _result.reset();
  }
};
