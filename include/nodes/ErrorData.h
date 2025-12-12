#pragma once
#include <QtNodes/NodeData>
#include <QString>

/**
 * @brief Standard Error Cluster Data Type ("The Yellow Wire")
 * Represents runtime status, code, and source of errors.
 */
class ErrorData : public QtNodes::NodeData
{
public:
    ErrorData() : _status(false), _code(0), _source("") {}
    
    ErrorData(bool status, int code, QString source)
        : _status(status), _code(code), _source(source) {}

    QtNodes::NodeDataType type() const override
    {
        return QtNodes::NodeDataType {"error", "Error"};
    }

    bool status() const { return _status; }
    int code() const { return _code; }
    QString source() const { return _source; }

    QString toString() const {
        if (!_status) return "OK";
        return QString("Error %1: %2").arg(_code).arg(_source);
    }

private:
    bool _status;    // True if error occurred
    int _code;       // Error code (non-zero usually)
    QString _source; // Source description
};
