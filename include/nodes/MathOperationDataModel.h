#pragma once
#include "ComplexData.h"
#include "DecimalData.h"
#include "ErrorData.h"
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModel>

class MathOperationDataModel : public QtNodes::NodeDelegateModel {
  Q_OBJECT
public:
  unsigned int nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override;
  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;
  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex portIndex) override;
  QWidget *embeddedWidget() override { return nullptr; }

protected:
  virtual void compute() = 0;

  // Polymorphic Inputs
  std::shared_ptr<QtNodes::NodeData> _inData1;
  std::shared_ptr<QtNodes::NodeData> _inData2;
  std::shared_ptr<QtNodes::NodeData> _resultData;

  // Helpers to get value as double, or 0.0 if not number
  double getScalar(std::shared_ptr<QtNodes::NodeData> data) const;

  // Helpers for Polymorphic Operations
  // If both scalars: execute Op
  // If Scalar + Array: execute Op on each element
  // If Array + Array: execute Op pair-wise
  void executePolymorphic(std::function<double(double, double)> op);

  // Error Handling
  std::shared_ptr<ErrorData> _errorIn;
  std::shared_ptr<ErrorData> _errorOut;
  
  // Compile-Time Type Resolution
public:
  void setResolvedOutputType(QtNodes::NodeDataType type) { _resolvedOutputType = type; }
  
private:
  QtNodes::NodeDataType _resolvedOutputType = QtNodes::NodeDataType{"decimal", "Decimal"};
};
