#pragma once
#include <QtNodes/NodeData>
#include <QVector>
#include <QVector3D>
#include <QColor>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

struct Point3D {
    QVector3D position;
    QColor color;
};

class PointCloudData : public NodeData
{
public:
    PointCloudData() {}
    PointCloudData(QVector<Point3D> const& points) : _points(points) {}

    NodeDataType type() const override { return NodeDataType{"pointcloud", "PointCloud"}; }

    QVector<Point3D> const& points() const { return _points; }

private:
    QVector<Point3D> _points;
};
