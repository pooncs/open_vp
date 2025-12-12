#pragma once
#include "ErrorData.h"
#include "StringData.h"
#include <QProcess>
#include <QtNodes/NodeDelegateModel>
#include <memory>

class PythonScriptNodeModel : public QtNodes::NodeDelegateModel {
public:
  PythonScriptNodeModel() = default;
  ~PythonScriptNodeModel() = default;

  QString caption() const override { return "Python Script"; }
  QString name() const override { return "PythonScript"; }

  unsigned int nPorts(QtNodes::PortType portType) const override;

  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override;

  std::shared_ptr<QtNodes::NodeData>
  outData(QtNodes::PortIndex portIndex) override;

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex portIndex) override;

  QWidget *embeddedWidget() override { return nullptr; }

private:
  void executeScript();

  // Helper to parse arguments
  QStringList parseArguments(const QString &args);

  std::shared_ptr<StringData> _scriptPath;
  std::shared_ptr<StringData> _args;

  std::shared_ptr<StringData> _output;
  std::shared_ptr<ErrorData> _errorOut;
};
