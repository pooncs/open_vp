#pragma once
#include "DecimalData.h"
#include "ErrorData.h"
#include "IFrontPanelNode.h"
#include "StringData.h"
#include "WaveformData.h"
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#ifdef HAS_SERIALPORT
#include <QSerialPort>
#include <QSerialPortInfo>
#endif
#include <QTimer>
#include <QVBoxLayout>
#include <QtNodes/NodeDelegateModel>

// ----------------------------------------------------------------------------
// Serial Config & Read Node
// ----------------------------------------------------------------------------
class SerialReadNodeModel : public QtNodes::NodeDelegateModel,
                            public IFrontPanelNode {
  Q_OBJECT
public:
  SerialReadNodeModel();
  virtual ~SerialReadNodeModel();

  QString caption() const override { return "Serial Read"; }
  QString name() const override { return "SerialRead"; }
  bool captionVisible() const override { return false; }

  unsigned int nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override;
  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;
  void setInData(std::shared_ptr<QtNodes::NodeData>,
                 QtNodes::PortIndex) override {}

  QWidget *embeddedWidget() override { return nullptr; }
  QWidget *frontPanelWidget() override { return _configWidget; }

private slots:
  void onConnectClicked();
  void onReadyRead();
  void refreshPorts();

private:
  QWidget *_configWidget;
  QComboBox *_portCombo;
  QComboBox *_baudCombo;
  QPushButton *_connectButton;
  QLabel *_statusLabel;

#ifdef HAS_SERIALPORT
  QSerialPort *_serial;
#else
  QTimer *_timer;
#endif

  std::shared_ptr<StringData> _lastData;
  std::shared_ptr<ErrorData> _errorOut;
  bool _connected = false;
};

// ----------------------------------------------------------------------------
// Simulated DAQ Node (Generates Waveform)
// ----------------------------------------------------------------------------
class SimulatedDAQNodeModel : public QtNodes::NodeDelegateModel,
                              public IFrontPanelNode {
  Q_OBJECT
public:
  SimulatedDAQNodeModel();
  virtual ~SimulatedDAQNodeModel() = default;

  QString caption() const override { return "Simulated DAQ"; }
  QString name() const override { return "SimulatedDAQ"; }
  bool captionVisible() const override { return false; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    return portType == QtNodes::PortType::Out ? 1 : 0;
  }
  QtNodes::NodeDataType dataType(QtNodes::PortType,
                                 QtNodes::PortIndex) const override {
    return WaveformData().type();
  }
  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
    return _waveform;
  }
  void setInData(std::shared_ptr<QtNodes::NodeData>,
                 QtNodes::PortIndex) override {}

  QWidget *embeddedWidget() override { return nullptr; }
  QWidget *frontPanelWidget() override { return _panelWidget; }

private slots:
  void onAcquire();

private:
  QWidget *_panelWidget;
  QPushButton *_acquireButton;
  std::shared_ptr<WaveformData> _waveform;
};
