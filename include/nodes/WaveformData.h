#pragma once
#include <QtNodes/NodeData>
#include <QVector>
#include <QPointF>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class WaveformData : public NodeData
{
public:
    WaveformData() {}
    WaveformData(QVector<QPointF> const& data) : _data(data) {}

    NodeDataType type() const override { return NodeDataType{"waveform", "Waveform"}; }

    QVector<QPointF> const& data() const { return _data; }

private:
    QVector<QPointF> _data;
};
