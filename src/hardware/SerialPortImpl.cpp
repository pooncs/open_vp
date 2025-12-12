#include "hardware/SerialPortImpl.h"
#include <QDebug>

namespace OpenFlow::Hardware {

SerialPortImpl::SerialPortImpl(const std::string &portName)
    : _portName(portName) {
#ifdef QT_SERIALPORT_LIB
  _serial.setPortName(QString::fromStdString(portName));
#endif
}

SerialPortImpl::~SerialPortImpl() { close(); }

bool SerialPortImpl::isOpen() const {
#ifdef QT_SERIALPORT_LIB
  return _serial.isOpen();
#else
  return false;
#endif
}

void SerialPortImpl::open() {
#ifdef QT_SERIALPORT_LIB
  if (!isOpen()) {
    if (!_serial.open(QIODevice::ReadWrite)) {
      throw HardwareException("Failed to open serial port: " + _portName);
    }
  }
#else
  throw HardwareException("Serial Port support not compiled");
#endif
}

void SerialPortImpl::close() {
#ifdef QT_SERIALPORT_LIB
  if (isOpen()) {
    _serial.close();
  }
#endif
}

void SerialPortImpl::configure(int baudRate, int dataBits, int stopBits,
                               int parity) {
#ifdef QT_SERIALPORT_LIB
  if (!isOpen())
    open();

  _serial.setBaudRate(baudRate);
  _serial.setDataBits(static_cast<QSerialPort::DataBits>(dataBits));

  QSerialPort::StopBits stop = QSerialPort::OneStop;
  if (stopBits == 2)
    stop = QSerialPort::TwoStop;
  _serial.setStopBits(stop);

  QSerialPort::Parity par = QSerialPort::NoParity;
  if (parity == 1)
    par = QSerialPort::OddParity;
  else if (parity == 2)
    par = QSerialPort::EvenParity;
  _serial.setParity(par);
#endif
}

void SerialPortImpl::write(const std::vector<uint8_t> &data) {
#ifdef QT_SERIALPORT_LIB
  if (!isOpen())
    throw HardwareException("Port not open");
  _serial.write(reinterpret_cast<const char *>(data.data()), data.size());
  _serial.waitForBytesWritten(100);
#endif
}

std::vector<uint8_t> SerialPortImpl::read(size_t maxBytes) {
#ifdef QT_SERIALPORT_LIB
  if (!isOpen())
    throw HardwareException("Port not open");

  if (_serial.waitForReadyRead(100)) {
    QByteArray ba = _serial.read(maxBytes);
    return std::vector<uint8_t>(ba.begin(), ba.end());
  }
#endif
  return {};
}

int SerialPortImpl::bytesAvailable() const {
#ifdef QT_SERIALPORT_LIB
  return _serial.bytesAvailable();
#else
  return 0;
#endif
}

std::vector<std::string> SerialPortImpl::availablePorts() {
  std::vector<std::string> ports;
#ifdef QT_SERIALPORT_LIB
  for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
    ports.push_back(info.portName().toStdString());
  }
#endif
  return ports;
}

} // namespace OpenFlow::Hardware
