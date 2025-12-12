#pragma once
#include <QtNodes/NodeDelegateModel>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QMouseEvent>
#include "IFrontPanelNode.h"
#include "PointCloudData.h"

class PointCloudWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    PointCloudWidget(QWidget* parent = nullptr);
    void setPoints(QVector<Point3D> const& points);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    
    // Simple Interaction
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QVector<Point3D> _points;
    float _xRot = 0;
    float _yRot = 0;
    float _zRot = 0;
    float _zoom = -5.0f;
    QPoint _lastPos;
};

class PointCloudDisplayModel : public QtNodes::NodeDelegateModel, public IFrontPanelNode
{
    Q_OBJECT
public:
    PointCloudDisplayModel();
    virtual ~PointCloudDisplayModel() = default;

    QString caption() const override { return "3D Viewer"; }
    QString name() const override { return "PointCloudDisplay"; }
    bool captionVisible() const override { return false; }

    unsigned int nPorts(QtNodes::PortType portType) const override { return portType == QtNodes::PortType::In ? 1 : 0; }
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override { return PointCloudData().type(); }
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override { return nullptr; }
    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex) override;
    
    QWidget *embeddedWidget() override { return nullptr; }
    QWidget* frontPanelWidget() override { return _widget; }

    QJsonObject save() const override;
    void load(QJsonObject const &p) override;

private:
    PointCloudWidget* _widget;
};
