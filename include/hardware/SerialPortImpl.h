#pragma once
#include "IHardwareInterface.h"

// Check if we have SerialPort support
#ifdef QT_SERIALPORT_LIB
#include <QSerialPort>
#include <QSerialPortInfo>
#endif

namespace OpenFlow::Hardware {

class SerialPortImpl : public ISerialPort {
public:
  SerialPortImpl(const std::string &portName);
  ~SerialPortImpl();

  std::string name() const override { return _portName; }
  bool isOpen() const override;
  void open() override;
  void close() override;

  void configure(int baudRate, int dataBits, int stopBits, int parity) override;
  void write(const std::vector<uint8_t> &data) override;
  std::vector<uint8_t> read(size_t maxBytes) override;
  int bytesAvailable() const override;

  static std::vector<std::string> availablePorts();

private:
  std::string _portName;
#ifdef QT_SERIALPORT_LIB
  QSerialPort _serial;
#endif
};

} // namespace OpenFlow::Hardware
