#pragma once
#include "ErrorData.h"
#include "StringData.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QtNodes/NodeDelegateModel>

class WriteFileNodeModel : public QtNodes::NodeDelegateModel {
public:
  QString caption() const override { return "Write to Text File"; }
  QString name() const override { return "WriteFile"; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    return (portType == QtNodes::PortType::In) ? 2
                                               : 1; // Path, Content -> Error
  }

  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override {
    if (portType == QtNodes::PortType::In) {
      return StringData().type(); // Path or Content
    }
    return ErrorData().type();
  }

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
    return _errorOut;
  }

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex portIndex) override {
    auto strData = std::dynamic_pointer_cast<StringData>(data);
    if (portIndex == 0)
      _path = strData;
    if (portIndex == 1)
      _content = strData;

    if (_path && _content) {
      QFile file(_path->value());
      if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << _content->value();
        file.close();
        _errorOut = std::make_shared<ErrorData>(false, 0, "");
      } else {
        _errorOut = std::make_shared<ErrorData>(true, 1, file.errorString());
      }
      Q_EMIT dataUpdated(0);
    }
  }

  QWidget *embeddedWidget() override { return nullptr; }

private:
  std::shared_ptr<StringData> _path;
  std::shared_ptr<StringData> _content;
  std::shared_ptr<ErrorData> _errorOut;
};

class ReadFileNodeModel : public QtNodes::NodeDelegateModel {
public:
  QString caption() const override { return "Read from Text File"; }
  QString name() const override { return "ReadFile"; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    return (portType == QtNodes::PortType::In) ? 1
                                               : 2; // Path -> Content, Error
  }

  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override {
    if (portType == QtNodes::PortType::In)
      return StringData().type();
    if (portIndex == 0)
      return StringData().type();
    return ErrorData().type();
  }

  std::shared_ptr<QtNodes::NodeData>
  outData(QtNodes::PortIndex portIndex) override {
    if (portIndex == 0)
      return _content;
    return _errorOut;
  }

  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex) override {
    auto strData = std::dynamic_pointer_cast<StringData>(data);
    if (strData) {
      QFile file(strData->value());
      if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        _content = std::make_shared<StringData>(in.readAll());
        _errorOut = std::make_shared<ErrorData>(false, 0, "");
        file.close();
      } else {
        _content = std::make_shared<StringData>("");
        _errorOut = std::make_shared<ErrorData>(true, 1, file.errorString());
      }
      Q_EMIT dataUpdated(0);
      Q_EMIT dataUpdated(1);
    }
  }

  QWidget *embeddedWidget() override { return nullptr; }

private:
  std::shared_ptr<StringData> _content;
  std::shared_ptr<ErrorData> _errorOut;
};
