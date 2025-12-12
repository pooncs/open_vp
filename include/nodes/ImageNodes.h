#pragma once
#include <QtNodes/NodeDelegateModel>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include "IFrontPanelNode.h"
#include "ImageData.h"

// Image Loader Node
class ImageLoaderModel : public QtNodes::NodeDelegateModel
{
    Q_OBJECT
public:
    ImageLoaderModel();
    virtual ~ImageLoaderModel() = default;

    QString caption() const override { return "Load Image"; }
    QString name() const override { return "ImageLoader"; }
    bool captionVisible() const override { return true; }

    unsigned int nPorts(QtNodes::PortType portType) const override { return portType == QtNodes::PortType::Out ? 1 : 0; }
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override { return ImageData().type(); }
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override { return _image; }
    void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex) override {}
    QWidget *embeddedWidget() override { return _button; }

private slots:
    void onLoadClicked();

private:
    std::shared_ptr<ImageData> _image;
    QPushButton* _button;
};

// Image Display Node
class ImageDisplayModel : public QtNodes::NodeDelegateModel, public IFrontPanelNode
{
    Q_OBJECT
public:
    ImageDisplayModel();
    virtual ~ImageDisplayModel() = default;

    QString caption() const override { return "Image Display"; }
    QString name() const override { return "ImageDisplay"; }
    bool captionVisible() const override { return false; }

    unsigned int nPorts(QtNodes::PortType portType) const override { return portType == QtNodes::PortType::In ? 1 : 0; }
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override { return ImageData().type(); }
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override { return nullptr; }
    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex) override;
    
    QWidget *embeddedWidget() override { return nullptr; }
    QWidget* frontPanelWidget() override { return _label; }

    QJsonObject save() const override;
    void load(QJsonObject const &p) override;

private:
    QLabel* _label;
};
