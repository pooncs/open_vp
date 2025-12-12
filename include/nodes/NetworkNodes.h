#pragma once
#include "ErrorData.h"
#include "IFrontPanelNode.h"
#include "StringData.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QUdpSocket>
#include <QtNodes/NodeDelegateModel>

class TCPClientNodeModel : public QtNodes::NodeDelegateModel,
                           public IFrontPanelNode {
  Q_OBJECT
public:
  TCPClientNodeModel();
  virtual ~TCPClientNodeModel();

  QString caption() const override { return "TCP Client"; }
  QString name() const override { return "TCPClient"; }
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
  void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
  QWidget *_configWidget;
  QLineEdit *_hostEdit;
  QLineEdit *_portEdit;
  QPushButton *_connectButton;
  QLabel *_statusLabel;

  QTcpSocket *_socket;
  std::shared_ptr<StringData> _lastData;
  std::shared_ptr<ErrorData> _errorOut;
  bool _connected = false;
};

class TCPServerNodeModel : public QtNodes::NodeDelegateModel,
                           public IFrontPanelNode {
  Q_OBJECT
public:
  TCPServerNodeModel();
  virtual ~TCPServerNodeModel();

  QString caption() const override { return "TCP Server"; }
  QString name() const override { return "TCPServer"; }
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
  void onListenClicked();
  void onNewConnection();
  void onReadyRead();

private:
  QWidget *_configWidget;
  QLineEdit *_portEdit;
  QPushButton *_listenButton;
  QLabel *_statusLabel;

  QTcpServer *_server;
  QTcpSocket *_clientSocket;
  std::shared_ptr<StringData> _lastData;
  std::shared_ptr<ErrorData> _errorOut;
  bool _listening = false;
};
