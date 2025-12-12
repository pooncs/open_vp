#pragma once
#include "BoolData.h"
#include "IFrontPanelNode.h"
#include "ui/controls/ToggleSwitch.h"
#include <QtNodes/NodeDelegateModel>

using namespace OpenFlow::UI::Controls;

class BooleanSwitchModel : public QtNodes::NodeDelegateModel,
                           public IFrontPanelNode {
  Q_OBJECT
public:
  BooleanSwitchModel();
  virtual ~BooleanSwitchModel() = default;

  QString caption() const override { return "Boolean Switch"; }
  QString name() const override { return "BooleanSwitch"; }
  bool captionVisible() const override { return false; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    return portType == QtNodes::PortType::Out ? 1 : 0;
  }
  QtNodes::NodeDataType dataType(QtNodes::PortType,
                                 QtNodes::PortIndex) const override {
    return BoolData().type();
  }
  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
    return _data;
  }
  void setInData(std::shared_ptr<QtNodes::NodeData>,
                 QtNodes::PortIndex) override {}

  QWidget *embeddedWidget() override { return nullptr; }
  QWidget *frontPanelWidget() override { return _switch; }

  QJsonObject save() const override;
  void load(QJsonObject const &p) override;

private slots:
  void onToggled(bool checked);

private:
  std::shared_ptr<BoolData> _data;
  ToggleSwitch *_switch;
};
