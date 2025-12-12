#pragma once
#include "IFrontPanelNode.h"
#include "WaveformData.h"
#include "ui/controls/WaveformGraph.h"
#include <QtNodes/NodeDelegateModel>

using namespace OpenFlow::UI::Controls;

class WaveformDisplayDataModel : public QtNodes::NodeDelegateModel,
                                 public IFrontPanelNode {
  Q_OBJECT
public:
  WaveformDisplayDataModel();
  virtual ~WaveformDisplayDataModel() = default;

  QString caption() const override { return "Waveform Chart"; }
  QString name() const override { return "WaveformChart"; }
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
  QWidget *frontPanelWidget() override { return _graph; }

  QJsonObject save() const override;
  void load(QJsonObject const &p) override;

private:
  WaveformGraph *_graph;
};
