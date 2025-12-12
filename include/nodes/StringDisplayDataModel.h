#pragma once
#include "IFrontPanelNode.h"
#include "StringData.h"
#include <QLabel>
#include <QtNodes/NodeDelegateModel>

class StringDisplayDataModel : public QtNodes::NodeDelegateModel,
                               public IFrontPanelNode {
  Q_OBJECT
public:
  StringDisplayDataModel();
  ~StringDisplayDataModel() = default;

  QString caption() const override { return "String Display"; }
  QString name() const override { return "StringDisplay"; }
  bool captionVisible() const override { return false; }

  unsigned int nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override;
  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
    return nullptr;
  }
  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex portIndex) override;

  QWidget *embeddedWidget() override { return nullptr; }
  QWidget *frontPanelWidget() override { return _label; }

  QJsonObject save() const override;
  void load(QJsonObject const &p) override;

private:
  QLabel *_label;
};
