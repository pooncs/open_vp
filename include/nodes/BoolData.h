#pragma once
#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class BoolData : public NodeData
{
public:
    BoolData() : _value(false) {}
    BoolData(bool const value) : _value(value) {}

    NodeDataType type() const override { return NodeDataType{"bool", "Bool"}; }

    bool value() const { return _value; }
    QString valueAsText() const { return _value ? "True" : "False"; }

private:
    bool _value;
};
