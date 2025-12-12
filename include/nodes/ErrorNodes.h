#pragma once
#include <QtNodes/NodeDelegateModel>
#include "ErrorData.h"
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "IFrontPanelNode.h"

// ----------------------------------------------------------------------------
// Clear Error Node
// ----------------------------------------------------------------------------
class ClearErrorNodeModel : public QtNodes::NodeDelegateModel
{
    Q_OBJECT
public:
    ClearErrorNodeModel() = default;
    virtual ~ClearErrorNodeModel() = default;

    QString caption() const override { return "Clear Error"; }
    QString name() const override { return "ClearError"; }
    bool captionVisible() const override { return true; }

    unsigned int nPorts(QtNodes::PortType portType) const override {
        return 1; // 1 In, 1 Out (Error Cluster)
    }

    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override {
        return ErrorData().type();
    }

    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
        return _outputError; // Always "OK" unless we want to pass specific cleared info?
        // Usually Clear Error resets status to false.
    }

    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex) override {
        // We receive error, but we output a clean one (or specific logic)
        _inputError = std::dynamic_pointer_cast<ErrorData>(data);
        
        // Reset output to No Error
        _outputError = std::make_shared<ErrorData>(false, 0, "");
        Q_EMIT dataUpdated(0);
    }

    QWidget *embeddedWidget() override { return nullptr; }

private:
    std::shared_ptr<ErrorData> _inputError;
    std::shared_ptr<ErrorData> _outputError;
};

// ----------------------------------------------------------------------------
// General Error Handler Node
// ----------------------------------------------------------------------------
class GeneralErrorHandlerNodeModel : public QtNodes::NodeDelegateModel, public IFrontPanelNode
{
    Q_OBJECT
public:
    GeneralErrorHandlerNodeModel() {
        _panel = new QWidget();
        auto layout = new QVBoxLayout(_panel);
        _statusLabel = new QLabel("No Error");
        _statusLabel->setStyleSheet("color: green; font-weight: bold;");
        layout->addWidget(_statusLabel);
    }
    
    virtual ~GeneralErrorHandlerNodeModel() = default;

    QString caption() const override { return "General Error Handler"; }
    QString name() const override { return "GeneralErrorHandler"; }
    bool captionVisible() const override { return false; }

    unsigned int nPorts(QtNodes::PortType portType) const override {
        return portType == QtNodes::PortType::In ? 1 : 1;
    }

    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override {
        return ErrorData().type();
    }

    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
        return _outputError;
    }

    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex) override {
        _inputError = std::dynamic_pointer_cast<ErrorData>(data);
        _outputError = _inputError; // Pass through
        
        if (_inputError && _inputError->status()) {
            _statusLabel->setText(QString("Error %1: %2").arg(_inputError->code()).arg(_inputError->source()));
            _statusLabel->setStyleSheet("color: red; font-weight: bold;");
            // Optionally show dialog if configured
        } else {
            _statusLabel->setText("No Error");
            _statusLabel->setStyleSheet("color: green; font-weight: bold;");
        }
        
        Q_EMIT dataUpdated(0);
    }

    QWidget *embeddedWidget() override { return nullptr; }
    QWidget* frontPanelWidget() override { return _panel; }

private:
    QWidget* _panel;
    QLabel* _statusLabel;
    std::shared_ptr<ErrorData> _inputError;
    std::shared_ptr<ErrorData> _outputError;
};
