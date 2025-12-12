#pragma once
#include <QtNodes/NodeData>
#include <QImage>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class ImageData : public NodeData
{
public:
    ImageData() {}
    ImageData(QImage const& image) : _image(image) {}

    NodeDataType type() const override { return NodeDataType{"image", "Image"}; }

    QImage const& image() const { return _image; }

private:
    QImage _image;
};
