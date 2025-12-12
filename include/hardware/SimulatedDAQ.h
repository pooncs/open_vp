#pragma once
#include "IHardwareInterface.h"
#include <map>
#include <random>

namespace OpenFlow::Hardware {

// A simulated DAQ device for testing/prototyping without real hardware
class SimulatedDAQ : public IDAQDevice {
public:
    SimulatedDAQ(const std::string& name) : _name(name) {}

    std::string name() const override { return _name; }
    bool isOpen() const override { return _isOpen; }
    
    void open() override { 
        _isOpen = true; 
        // Reset state
        for(int i=0; i<8; ++i) _analogOut[i] = 0.0;
        for(int i=0; i<8; ++i) _digitalOut[i] = false;
    }
    
    void close() override { _isOpen = false; }

    double readAnalog(int channel) override {
        if (!_isOpen) throw HardwareException("Device not open");
        // Simulate a sine wave mixed with noise based on channel
        static std::default_random_engine gen;
        static std::normal_distribution<double> dist(0.0, 0.1);
        
        // Loop back output if set? Or just generate data
        double noise = dist(gen);
        // Simple generator: sin(t) where t is just call count
        static int callCount = 0;
        callCount++;
        return std::sin(callCount * 0.1 + channel) * 5.0 + noise; 
    }

    void writeAnalog(int channel, double voltage) override {
        if (!_isOpen) throw HardwareException("Device not open");
        _analogOut[channel] = voltage;
    }

    bool readDigital(int channel) override {
        if (!_isOpen) throw HardwareException("Device not open");
        // Loopback digital out
        return _digitalOut[channel];
    }

    void writeDigital(int channel, bool value) override {
        if (!_isOpen) throw HardwareException("Device not open");
        _digitalOut[channel] = value;
    }

private:
    std::string _name;
    bool _isOpen = false;
    std::map<int, double> _analogOut;
    std::map<int, bool> _digitalOut;
};

} // namespace OpenFlow::Hardware
