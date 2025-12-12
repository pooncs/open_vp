#include "NetworkNodes.h"

TCPClientNodeModel::TCPClientNodeModel()
    : _lastData(std::make_shared<StringData>("")) {
  _socket = new QTcpSocket(this);

  _configWidget = new QWidget();
  auto layout = new QVBoxLayout(_configWidget);

  _hostEdit = new QLineEdit("127.0.0.1");
  _portEdit = new QLineEdit("8080");
  _connectButton = new QPushButton("Connect");
  _statusLabel = new QLabel("Disconnected");
  _statusLabel->setStyleSheet("color: red;");

  layout->addWidget(new QLabel("Host:"));
  layout->addWidget(_hostEdit);
  layout->addWidget(new QLabel("Port:"));
  layout->addWidget(_portEdit);
  layout->addWidget(_connectButton);
  layout->addWidget(_statusLabel);

  connect(_connectButton, &QPushButton::clicked, this,
          &TCPClientNodeModel::onConnectClicked);
  connect(_socket, &QTcpSocket::readyRead, this,
          &TCPClientNodeModel::onReadyRead);
  connect(_socket, &QTcpSocket::errorOccurred, this,
          &TCPClientNodeModel::onErrorOccurred);
}

TCPClientNodeModel::~TCPClientNodeModel() {
  if (_socket->isOpen())
    _socket->close();
}

void TCPClientNodeModel::onConnectClicked() {
  if (_connected) {
    _socket->close();
    _connected = false;
    _connectButton->setText("Connect");
    _statusLabel->setText("Disconnected");
    _statusLabel->setStyleSheet("color: red;");
    _hostEdit->setEnabled(true);
    _portEdit->setEnabled(true);
  } else {
    _socket->connectToHost(_hostEdit->text(), _portEdit->text().toUShort());
    if (_socket->waitForConnected(3000)) {
      _connected = true;
      _connectButton->setText("Disconnect");
      _statusLabel->setText("Connected");
      _statusLabel->setStyleSheet("color: green;");
      _hostEdit->setEnabled(false);
      _portEdit->setEnabled(false);
      _errorOut = std::make_shared<ErrorData>();
    } else {
      _statusLabel->setText("Timeout");
      _errorOut = std::make_shared<ErrorData>(true, 6001, "Connection Timeout");
      Q_EMIT dataUpdated(1);
    }
  }
}

void TCPClientNodeModel::onReadyRead() {
  QByteArray data = _socket->readAll();
  _lastData = std::make_shared<StringData>(QString(data));
  Q_EMIT dataUpdated(0);
}

void TCPClientNodeModel::onErrorOccurred(
    QAbstractSocket::SocketError socketError) {
  _statusLabel->setText("Error");
  _errorOut = std::make_shared<ErrorData>(true, 6000 + socketError,
                                          _socket->errorString());
  Q_EMIT dataUpdated(1);

  if (socketError == QAbstractSocket::RemoteHostClosedError) {
    onConnectClicked(); // Reset UI
  }
}

unsigned int TCPClientNodeModel::nPorts(QtNodes::PortType portType) const {
  return portType == QtNodes::PortType::Out ? 2 : 0;
}

QtNodes::NodeDataType
TCPClientNodeModel::dataType(QtNodes::PortType portType,
                             QtNodes::PortIndex portIndex) const {
  if (portIndex == 1)
    return ErrorData().type();
  return StringData().type();
}

std::shared_ptr<QtNodes::NodeData>
TCPClientNodeModel::outData(QtNodes::PortIndex portIndex) {
  if (portIndex == 1)
    return _errorOut;
  return _lastData;
}

// ----------------------------------------------------------------------------
// TCP Server Node
// ----------------------------------------------------------------------------
TCPServerNodeModel::TCPServerNodeModel()
    : _lastData(std::make_shared<StringData>("")) {
  _server = new QTcpServer(this);
  _clientSocket = nullptr;

  _configWidget = new QWidget();
  auto layout = new QVBoxLayout(_configWidget);

  _portEdit = new QLineEdit("8080");
  _listenButton = new QPushButton("Listen");
  _statusLabel = new QLabel("Stopped");
  _statusLabel->setStyleSheet("color: red;");

  layout->addWidget(new QLabel("Port:"));
  layout->addWidget(_portEdit);
  layout->addWidget(_listenButton);
  layout->addWidget(_statusLabel);

  connect(_listenButton, &QPushButton::clicked, this,
          &TCPServerNodeModel::onListenClicked);
  connect(_server, &QTcpServer::newConnection, this,
          &TCPServerNodeModel::onNewConnection);
}

TCPServerNodeModel::~TCPServerNodeModel() {
  if (_server->isListening())
    _server->close();
}

void TCPServerNodeModel::onListenClicked() {
  if (_listening) {
    _server->close();
    if (_clientSocket) {
      _clientSocket->close();
      _clientSocket = nullptr;
    }
    _listening = false;
    _listenButton->setText("Listen");
    _statusLabel->setText("Stopped");
    _statusLabel->setStyleSheet("color: red;");
    _portEdit->setEnabled(true);
  } else {
    if (_server->listen(QHostAddress::Any, _portEdit->text().toUShort())) {
      _listening = true;
      _listenButton->setText("Stop");
      _statusLabel->setText("Listening...");
      _statusLabel->setStyleSheet("color: orange;");
      _portEdit->setEnabled(false);
      _errorOut = std::make_shared<ErrorData>();
    } else {
      _statusLabel->setText("Error");
      _errorOut =
          std::make_shared<ErrorData>(true, 6002, _server->errorString());
      Q_EMIT dataUpdated(1);
    }
  }
}

void TCPServerNodeModel::onNewConnection() {
  if (_clientSocket) {
    _clientSocket->close();
    _clientSocket->deleteLater();
  }
  _clientSocket = _server->nextPendingConnection();
  connect(_clientSocket, &QTcpSocket::readyRead, this,
          &TCPServerNodeModel::onReadyRead);
  connect(_clientSocket, &QTcpSocket::disconnected, [this]() {
    _statusLabel->setText("Listening...");
    _statusLabel->setStyleSheet("color: orange;");
    _clientSocket = nullptr;
  });

  _statusLabel->setText("Client Connected");
  _statusLabel->setStyleSheet("color: green;");
}

void TCPServerNodeModel::onReadyRead() {
  if (_clientSocket) {
    QByteArray data = _clientSocket->readAll();
    _lastData = std::make_shared<StringData>(QString(data));
    Q_EMIT dataUpdated(0);

    // Echo back
    _clientSocket->write(data);
  }
}

unsigned int TCPServerNodeModel::nPorts(QtNodes::PortType portType) const {
  return portType == QtNodes::PortType::Out ? 2 : 0;
}

QtNodes::NodeDataType
TCPServerNodeModel::dataType(QtNodes::PortType portType,
                             QtNodes::PortIndex portIndex) const {
  if (portIndex == 1)
    return ErrorData().type();
  return StringData().type();
}

std::shared_ptr<QtNodes::NodeData>
TCPServerNodeModel::outData(QtNodes::PortIndex portIndex) {
  if (portIndex == 1)
    return _errorOut;
  return _lastData;
}
