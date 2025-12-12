#pragma once
#include "BoolData.h"
#include "IFrontPanelNode.h"
#include "ui/controls/BooleanLED.h"
#include <QtNodes/NodeDelegateModel>

using namespace OpenFlow::UI::Controls;

class BooleanDisplayDataModel : public QtNodes::NodeDelegateModel,
                                public IFrontPanelNode {
  Q_OBJECT
public:
  BooleanDisplayDataModel();
  virtual ~BooleanDisplayDataModel() = default;

  QString caption() const override { return "Boolean LED"; }
  QString name() const override { return "BooleanDisplay"; }
  bool captionVisible() const override { return false; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    return portType == QtNodes::PortType::In ? 1 : 0;
  }
  QtNodes::NodeDataType dataType(QtNodes::PortType,
                                 QtNodes::PortIndex) const override {
    return BoolData().type();
  }
  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
    return nullptr;
  }
  void setInData(std::shared_ptr<QtNodes::NodeData> data,
                 QtNodes::PortIndex) override;

  QWidget *embeddedWidget() override { return nullptr; }
  QWidget *frontPanelWidget() override { return _led; }

  QJsonObject save() const override;
  void load(QJsonObject const &p) override;

private:
  BooleanLED *_led;
};
