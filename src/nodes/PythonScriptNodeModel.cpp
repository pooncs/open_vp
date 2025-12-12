#include "PythonScriptNodeModel.h"
#include "DecimalData.h"
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>

unsigned int PythonScriptNodeModel::nPorts(QtNodes::PortType portType) const {
  // In: Script Path (0), Arguments (1)
  // Out: Output (0), Error (1)
  return 2;
}

QtNodes::NodeDataType
PythonScriptNodeModel::dataType(QtNodes::PortType portType,
                                QtNodes::PortIndex portIndex) const {
  if (portType == QtNodes::PortType::In) {
    // Port 0: Script Path (String)
    // Port 1: Arguments (String or Decimal)
    if (portIndex == 0)
      return StringData().type();
    return QtNodes::NodeDataType{
        "StringData",
        "String"}; // We default to String, but logic handles others
  } else {
    if (portIndex == 0)
      return StringData().type();
    return ErrorData().type();
  }
}

std::shared_ptr<QtNodes::NodeData>
PythonScriptNodeModel::outData(QtNodes::PortIndex portIndex) {
  if (portIndex == 0)
    return _output;
  return _errorOut;
}

void PythonScriptNodeModel::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                      QtNodes::PortIndex portIndex) {
  if (portIndex == 0) {
    _scriptPath = std::dynamic_pointer_cast<StringData>(data);
  } else if (portIndex == 1) {
    if (auto strData = std::dynamic_pointer_cast<StringData>(data)) {
      _args = strData;
    } else if (auto decData = std::dynamic_pointer_cast<DecimalData>(data)) {
      _args = std::make_shared<StringData>(QString::number(decData->number()));
    } else {
      // Try generic to string if possible or reset
      _args = nullptr;
    }
  }

  // Only execute if we have a script path.
  if (_scriptPath) {
    executeScript();
  }
}

void PythonScriptNodeModel::executeScript() {
  if (!_scriptPath || _scriptPath->value().isEmpty()) {
    _errorOut = std::make_shared<ErrorData>(true, 1, "Script path is empty");
    _output = std::make_shared<StringData>("");
    Q_EMIT dataUpdated(0);
    Q_EMIT dataUpdated(1);
    return;
  }

  QString scriptFile = _scriptPath->value();
  if (!QFileInfo::exists(scriptFile)) {
    _errorOut = std::make_shared<ErrorData>(
        true, 1, "Script file does not exist: " + scriptFile);
    _output = std::make_shared<StringData>("");
    Q_EMIT dataUpdated(0);
    Q_EMIT dataUpdated(1);
    return;
  }

  QProcess process;
  // Attempt to find python. On Windows "python" or "py" or "python3".
  // Use "python" as default, but allow overriding via environment variable
  QString program = qEnvironmentVariable("QTLV_PYTHON_PATH", "python");

  QStringList arguments;
  arguments << scriptFile;

  if (_args && !_args->value().isEmpty()) {
    arguments << parseArguments(_args->value());
  }

  // Set working directory to script directory
  process.setWorkingDirectory(QFileInfo(scriptFile).absolutePath());

  process.start(program, arguments);
  if (!process.waitForStarted()) {
    _errorOut = std::make_shared<ErrorData>(
        true, 1, "Failed to start python process. Check PATH.");
    _output = std::make_shared<StringData>("");
  } else {
    // Wait up to 10 seconds for script to finish
    if (!process.waitForFinished(10000)) {
      _errorOut = std::make_shared<ErrorData>(
          true, 2, "Python script timed out (10s limit).");
      process.kill();
      _output = std::make_shared<StringData>("");
    } else {
      QString stdOut = process.readAllStandardOutput();
      QString stdErr = process.readAllStandardError();

      if (process.exitCode() != 0) {
        QString errorMsg = stdErr.isEmpty()
                               ? "Process exited with code " +
                                     QString::number(process.exitCode())
                               : stdErr;
        _errorOut =
            std::make_shared<ErrorData>(true, process.exitCode(), errorMsg);
      } else {
        _errorOut = std::make_shared<ErrorData>(false, 0, "");
      }
      _output = std::make_shared<StringData>(stdOut);
    }
  }

  Q_EMIT dataUpdated(0);
  Q_EMIT dataUpdated(1);
}

QStringList PythonScriptNodeModel::parseArguments(const QString &args) {
  QStringList result;
  // Simple parser that handles quotes
  // Regex to match quoted strings or non-whitespace sequences
  QRegularExpression re("(\"[^\"]*\"|'[^']*'|[^\\s]+)");
  QRegularExpressionMatchIterator i = re.globalMatch(args);
  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();
    QString arg = match.captured(1);
    // Remove quotes if present
    if ((arg.startsWith('"') && arg.endsWith('"')) ||
        (arg.startsWith('\'') && arg.endsWith('\''))) {
      arg = arg.mid(1, arg.length() - 2);
    }
    result << arg;
  }
  return result;
}
