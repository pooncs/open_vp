#pragma once
#include "CustomWidgets.h"
#include "DecimalData.h"
#include "IFrontPanelNode.h"
#include "ui/controls/DigitalGauge.h"
#include <QDial>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>
#include <QtNodes/NodeDelegateModel>

using namespace OpenFlow::UI::Controls;

class KnobModel : public QtNodes::NodeDelegateModel, public IFrontPanelNode {
  Q_OBJECT
public:
  KnobModel();
  virtual ~KnobModel() = default;

  QString caption() const override { return "Knob"; }
  QString name() const override { return "Knob"; }
  bool captionVisible() const override { return false; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    return portType == QtNodes::PortType::Out ? 1 : 0;
  }

  QtNodes::NodeDataType dataType(QtNodes::PortType,
                                 QtNodes::PortIndex) const override {
    return DecimalData().type();
  }

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
    return _number;
  }

  void setInData(std::shared_ptr<QtNodes::NodeData>,
                 QtNodes::PortIndex) override {}

  QWidget *embeddedWidget() override { return nullptr; }
  QWidget *frontPanelWidget() override { return _container; }

  QJsonObject save() const override;
  void load(QJsonObject const &p) override;

private slots:
  void onValueChanged(double value);

private:
  QWidget *_container;
  DigitalGauge *_knob;
  QLabel *_label;
  std::shared_ptr<DecimalData> _number;
};

class SlideModel : public QtNodes::NodeDelegateModel, public IFrontPanelNode {
  Q_OBJECT
public:
  SlideModel();
  virtual ~SlideModel() = default;

  QString caption() const override { return "Slide"; }
  QString name() const override { return "Slide"; }
  bool captionVisible() const override { return false; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    return portType == QtNodes::PortType::Out ? 1 : 0;
  }

  QtNodes::NodeDataType dataType(QtNodes::PortType,
                                 QtNodes::PortIndex) const override {
    return DecimalData().type();
  }

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override {
    return _number;
  }

  void setInData(std::shared_ptr<QtNodes::NodeData>,
                 QtNodes::PortIndex) override {}

  QWidget *embeddedWidget() override { return nullptr; }
  QWidget *frontPanelWidget() override { return _container; }

  QJsonObject save() const override;
  void load(QJsonObject const &p) override;

private slots:
  void onValueChanged(int value);

private:
  QWidget *_container;
  QSlider *_slider;
  QLabel *_label;
  std::shared_ptr<DecimalData> _number;
};
