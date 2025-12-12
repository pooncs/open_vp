#pragma once
#include <QtNodes/NodeDelegateModel>
#include "BoolData.h"

class LoopConditionNodeModel : public QtNodes::NodeDelegateModel {
public:
    QString caption() const override { return "Loop Condition"; }
    QString name() const override { return "LoopCondition"; }
    
    unsigned int nPorts(QtNodes::PortType portType) const override {
        return (portType == QtNodes::PortType::In) ? 1 : 0;
    }
    
    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override {
        return BoolData().type();
    }
    
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
        return nullptr;
    }
    
    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex) override {
        _input = std::dynamic_pointer_cast<BoolData>(data);
    }
    
    QWidget* embeddedWidget() override { return nullptr; }

    bool shouldStop() const {
        // LabVIEW "Stop if True" logic:
        // If input is True, stop.
        // If input is disconnected, default is False (Loop forever).
        return _input && _input->value();
    }

private:
    std::shared_ptr<BoolData> _input;
};
