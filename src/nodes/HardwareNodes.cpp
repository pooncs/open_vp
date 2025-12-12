#include "HardwareNodes.h"
#include <QDebug>
#include <random>

// ----------------------------------------------------------------------------
// Serial Config & Read Node
// ----------------------------------------------------------------------------
SerialReadNodeModel::SerialReadNodeModel()
    : _lastData(std::make_shared<StringData>("")) {

#ifdef HAS_SERIALPORT
  _serial = new QSerialPort(this);
  connect(_serial, &QSerialPort::readyRead, this,
          &SerialReadNodeModel::onReadyRead);
#else
  _timer = new QTimer(this);
  connect(_timer, &QTimer::timeout, this, &SerialReadNodeModel::onReadyRead);
#endif

  _configWidget = new QWidget();
  auto layout = new QVBoxLayout(_configWidget);

  _portCombo = new QComboBox();
  _baudCombo = new QComboBox();
  _baudCombo->addItems({"9600", "19200", "38400", "57600", "115200"});

  _connectButton = new QPushButton("Connect");
  _statusLabel = new QLabel("Disconnected");
  _statusLabel->setStyleSheet("color: red;");

  layout->addWidget(new QLabel("Port:"));
  layout->addWidget(_portCombo);
  layout->addWidget(new QLabel("Baud:"));
  layout->addWidget(_baudCombo);
  layout->addWidget(_connectButton);
  layout->addWidget(_statusLabel);

  refreshPorts();

  connect(_connectButton, &QPushButton::clicked, this,
          &SerialReadNodeModel::onConnectClicked);
}

SerialReadNodeModel::~SerialReadNodeModel() {
#ifdef HAS_SERIALPORT
  if (_serial->isOpen())
    _serial->close();
#endif
}

void SerialReadNodeModel::refreshPorts() {
  _portCombo->clear();
#ifdef HAS_SERIALPORT
  const auto infos = QSerialPortInfo::availablePorts();
  for (const QSerialPortInfo &info : infos) {
    _portCombo->addItem(info.portName());
  }
#else
  _portCombo->addItem("COM1 (Sim)");
  _portCombo->addItem("COM2 (Sim)");
#endif
}

void SerialReadNodeModel::onConnectClicked() {
  if (_connected) {
#ifdef HAS_SERIALPORT
    _serial->close();
#else
    _timer->stop();
#endif
    _connected = false;
    _connectButton->setText("Connect");
    _statusLabel->setText("Disconnected");
    _statusLabel->setStyleSheet("color: red;");
    _portCombo->setEnabled(true);
    _baudCombo->setEnabled(true);
  } else {
#ifdef HAS_SERIALPORT
    _serial->setPortName(_portCombo->currentText());
    _serial->setBaudRate(_baudCombo->currentText().toInt());

    if (_serial->open(QIODevice::ReadOnly)) {
      _connected = true;
      _connectButton->setText("Disconnect");
      _statusLabel->setText("Connected");
      _statusLabel->setStyleSheet("color: green;");
      _portCombo->setEnabled(false);
      _baudCombo->setEnabled(false);
      _errorOut = std::make_shared<ErrorData>();
    } else {
      _statusLabel->setText("Error");
      _errorOut =
          std::make_shared<ErrorData>(true, 5001, _serial->errorString());
      Q_EMIT dataUpdated(1); // Error Out
    }
#else
    _connected = true;
    _timer->start(1000);
    _connectButton->setText("Disconnect");
    _statusLabel->setText("Connected (Sim)");
    _statusLabel->setStyleSheet("color: green;");
    _portCombo->setEnabled(false);
    _baudCombo->setEnabled(false);
#endif
  }
}

void SerialReadNodeModel::onReadyRead() {
#ifdef HAS_SERIALPORT
  QByteArray data = _serial->readAll();
  QString str(data);
#else
  QString str = QString("Data: %1").arg(std::rand() % 100);
#endif
  _lastData = std::make_shared<StringData>(str);
  Q_EMIT dataUpdated(0);
}

unsigned int SerialReadNodeModel::nPorts(QtNodes::PortType portType) const {
  // Out: String + Error
  return portType == QtNodes::PortType::Out ? 2 : 0;
}

QtNodes::NodeDataType
SerialReadNodeModel::dataType(QtNodes::PortType portType,
                              QtNodes::PortIndex portIndex) const {
  if (portIndex == 1)
    return ErrorData().type();
  return StringData().type();
}

std::shared_ptr<QtNodes::NodeData>
SerialReadNodeModel::outData(QtNodes::PortIndex portIndex) {
  if (portIndex == 1)
    return _errorOut;
  return _lastData;
}

// ----------------------------------------------------------------------------
// Simulated DAQ
// ----------------------------------------------------------------------------
SimulatedDAQNodeModel::SimulatedDAQNodeModel() {
  _panelWidget = new QWidget();
  auto layout = new QVBoxLayout(_panelWidget);
  _acquireButton = new QPushButton("Acquire Signal");
  layout->addWidget(_acquireButton);

  connect(_acquireButton, &QPushButton::clicked, this,
          &SimulatedDAQNodeModel::onAcquire);

  _waveform = std::make_shared<WaveformData>();
}

void SimulatedDAQNodeModel::onAcquire() {
  QVector<QPointF> points;
  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 0.1); // Noise

  // Simulate 1000 samples of a noisy 10Hz sine wave + 50Hz hum
  double fs = 1000.0;
  for (int i = 0; i < 200; ++i) {
    double t = i / fs;
    double val = 1.0 * std::sin(2 * 3.14159 * 10 * t) +
                 0.2 * std::sin(2 * 3.14159 * 50 * t) + distribution(generator);
    points.append(QPointF(i, val));
  }

  _waveform = std::make_shared<WaveformData>(points);
  Q_EMIT dataUpdated(0);
}
