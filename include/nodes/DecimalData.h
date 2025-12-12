#pragma once
#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class DecimalData : public NodeData
{
public:
  DecimalData() : _number(0.0) {}
  DecimalData(double const number) : _number(number) {}

  QtNodes::NodeDataType type() const override {
    return QtNodes::NodeDataType{"decimal", "Decimal"};
  }

  double number() const { return _number; }
  
  // In-Place modification support
  void setNumber(double number) { _number = number; }

  QString numberAsText() const { return QString::number(_number); }

private:
  double _number;
};
