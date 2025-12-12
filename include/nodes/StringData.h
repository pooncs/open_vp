#pragma once
#include <QtNodes/NodeData>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class StringData : public NodeData
{
public:
    StringData() : _value("") {}
    StringData(QString const &value) : _value(value) {}

    NodeDataType type() const override { return NodeDataType{"string", "String"}; }

    QString value() const { return _value; }

private:
    QString _value;
};
