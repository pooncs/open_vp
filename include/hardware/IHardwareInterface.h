#pragma once
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace OpenFlow::Hardware {

class HardwareException : public std::runtime_error {
public:
    HardwareException(const std::string& msg) : std::runtime_error(msg) {}
};

class IDevice {
public:
    virtual ~IDevice() = default;
    virtual std::string name() const = 0;
    virtual bool isOpen() const = 0;
    virtual void open() = 0;
    virtual void close() = 0;
};

class ISerialPort : public IDevice {
public:
    virtual void configure(int baudRate, int dataBits, int stopBits, int parity) = 0;
    virtual void write(const std::vector<uint8_t>& data) = 0;
    virtual std::vector<uint8_t> read(size_t maxBytes) = 0;
    virtual int bytesAvailable() const = 0;
};

class IDAQDevice : public IDevice {
public:
    virtual double readAnalog(int channel) = 0;
    virtual void writeAnalog(int channel, double voltage) = 0;
    virtual bool readDigital(int channel) = 0;
    virtual void writeDigital(int channel, bool value) = 0;
};

} // namespace OpenFlow::Hardware
