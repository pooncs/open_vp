#pragma once
#include <QtNodes/NodeDelegateModel>
#include "WaveformData.h"
#include <cmath>

class SignalGeneratorDataModel : public QtNodes::NodeDelegateModel
{
    Q_OBJECT
public:
    SignalGeneratorDataModel();
    virtual ~SignalGeneratorDataModel() = default;

    QString caption() const override { return "Signal Generator"; }
    QString name() const override { return "SignalGenerator"; }
    bool captionVisible() const override { return true; }

    unsigned int nPorts(QtNodes::PortType portType) const override { return portType == QtNodes::PortType::Out ? 1 : 0; }
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override { return WaveformData().type(); }
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override { return _waveform; }
    void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex) override {}
    QWidget *embeddedWidget() override { return nullptr; }

private:
    std::shared_ptr<WaveformData> _waveform;
};
