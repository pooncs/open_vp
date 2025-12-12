#pragma once
#include "DecimalData.h"
#include "ErrorData.h"
#include <QtNodes/NodeDelegateModel>
#include <cmath>

// Unary Operation Base Class
class UnaryMathModel : public QtNodes::NodeDelegateModel {
  // Q_OBJECT // Removed Q_OBJECT from base class to avoid linkage issues in
  // tests if not compiled separately
public:
  unsigned int nPorts(QtNodes::PortType portType) const override {
    // Input: 1 Number + 1 ErrorIn
    // Output: 1 Number + 1 ErrorOut
    return 2;
  }
  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override {
    if (portType == QtNodes::PortType::In) {
      if (portIndex == 1)
        return ErrorData().type();
      return DecimalData().type();
    } else {
      if (portIndex == 1)
        return ErrorData().type();
      return DecimalData().type();
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
    if (portIndex == 1) {
      _errorIn = std::dynamic_pointer_cast<ErrorData>(data);
    } else {
      auto numberData = std::dynamic_pointer_cast<DecimalData>(data);
      if (numberData) {
        _inputVal = numberData->number();
        _inputValid = true;
      } else {
        _inputValid = false;
      }
    }

    // Standard Error Propagation
    if (_errorIn && _errorIn->status()) {
      _errorOut = _errorIn;
      _result.reset();
    } else {
      _errorOut = std::make_shared<ErrorData>();
      if (_inputValid) {
        _result = std::make_shared<DecimalData>(compute(_inputVal));
      } else {
        _result.reset();
      }
    }
    Q_EMIT dataUpdated(0);
    Q_EMIT dataUpdated(1);
  }
  QWidget *embeddedWidget() override { return nullptr; }

protected:
  virtual double compute(double input) = 0;
  std::shared_ptr<DecimalData> _result;
  std::shared_ptr<ErrorData> _errorIn;
  std::shared_ptr<ErrorData> _errorOut;
  double _inputVal = 0.0;
  bool _inputValid = false;
};

class SinModel : public UnaryMathModel {
public:
  QString caption() const override { return "Sine"; }
  QString name() const override { return "Sin"; }

protected:
  double compute(double input) override { return std::sin(input); }
};

class CosModel : public UnaryMathModel {
public:
  QString caption() const override { return "Cosine"; }
  QString name() const override { return "Cos"; }

protected:
  double compute(double input) override { return std::cos(input); }
};

class TanModel : public UnaryMathModel {
public:
  QString caption() const override { return "Tangent"; }
  QString name() const override { return "Tan"; }

protected:
  double compute(double input) override { return std::tan(input); }
};

class SqrtModel : public UnaryMathModel {
public:
  QString caption() const override { return "Square Root"; }
  QString name() const override { return "Sqrt"; }

protected:
  double compute(double input) override { return std::sqrt(input); }
};

class AbsModel : public UnaryMathModel {
public:
  QString caption() const override { return "Absolute"; }
  QString name() const override { return "Abs"; }

protected:
  double compute(double input) override { return std::abs(input); }
};
