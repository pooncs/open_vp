#pragma once
#include "IFrontPanelNode.h"
#include <QLabel>
#include <QtNodes/NodeDelegateModel>

class NumberDisplayDataModel : public QtNodes::NodeDelegateModel,
                               public IFrontPanelNode {
  Q_OBJECT
public:
  NumberDisplayDataModel();
  ~NumberDisplayDataModel() = default;

  QString caption() const override { return "Number Display"; }
  QString name() const override { return "NumberDisplay"; }
  bool captionVisible() const override { return false; }

  unsigned int nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override;
  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
    return nullptr;
  }
  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex portIndex) override;

  // Disable embedded widget
  QWidget *embeddedWidget() override { return nullptr; }

  // IFrontPanelNode interface
  QWidget *frontPanelWidget() override { return _label; }

  QJsonObject save() const override;
  void load(QJsonObject const &p) override;

private:
  QLabel *_label;
};
