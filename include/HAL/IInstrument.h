#pragma once
#include <QString>
#include <vector>
#include <memory>
#include <QObject>

/**
 * @brief Abstract Interface for Hardware Instruments.
 * Complies with IVI/LXI concepts (Initialize, Read, Write, Close).
 */
class IInstrument : public QObject
{
    Q_OBJECT
public:
    virtual ~IInstrument() = default;

    /**
     * @brief Initialize connection to the instrument.
     * @param resourceString The resource identifier (e.g., "GPIB0::1::INSTR", "ASRL1::INSTR")
     * @return True if connected successfully.
     */
    virtual bool initialize(const QString& resourceString) = 0;

    /**
     * @brief Close the connection.
     */
    virtual void close() = 0;

    /**
     * @brief Write a command string to the instrument.
     */
    virtual void write(const QString& command) = 0;

    /**
     * @brief Read a response string from the instrument.
     */
    virtual QString read() = 0;

    /**
     * @brief Query the instrument (Write followed by Read).
     */
    virtual QString query(const QString& command) {
        write(command);
        return read();
    }

    /**
     * @brief Get the last error message.
     */
    virtual QString lastError() const = 0;
};

/**
 * @brief Factory/Manager for loading Instrument Drivers (Plugins).
 */
class InstrumentDriverFactory
{
public:
    static std::shared_ptr<IInstrument> createDriver(const QString& driverName);
    static std::vector<QString> availableDrivers();
};
