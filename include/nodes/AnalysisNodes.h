#pragma once
#include <QtNodes/NodeDelegateModel>
#include "WaveformData.h"
#include <complex>
#include <vector>

// ----------------------------------------------------------------------------
// FFT Node
// ----------------------------------------------------------------------------
class FFTNodeModel : public QtNodes::NodeDelegateModel
{
    Q_OBJECT
public:
    FFTNodeModel() = default;
    virtual ~FFTNodeModel() = default;

    QString caption() const override { return "FFT (Magnitude)"; }
    QString name() const override { return "FFT"; }
    bool captionVisible() const override { return true; }

    unsigned int nPorts(QtNodes::PortType portType) const override {
        return (portType == QtNodes::PortType::In) ? 1 : 1;
    }

    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override {
        return WaveformData().type();
    }

    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
        return _result;
    }

    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex) override;

    QWidget *embeddedWidget() override { return nullptr; }

private:
    std::shared_ptr<WaveformData> _result;
};
