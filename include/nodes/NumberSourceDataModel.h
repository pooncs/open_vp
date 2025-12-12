#pragma once
#include "IFrontPanelNode.h"
#include <QLineEdit>
#include <QtNodes/NodeDelegateModel>

class DecimalData;

class NumberSourceDataModel : public QtNodes::NodeDelegateModel,
                              public IFrontPanelNode {
  Q_OBJECT
public:
  NumberSourceDataModel();
  ~NumberSourceDataModel() = default;

  QString caption() const override { return "Number Source"; }
  QString name() const override { return "NumberSource"; }
  bool captionVisible() const override { return false; }

  unsigned int nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const override;
  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;
  void setInData(std::shared_ptr<QtNodes::NodeData>,
                 QtNodes::PortIndex) override {}

  // Disable embedded widget to mimic LabVIEW terminal
  QWidget *embeddedWidget() override { return nullptr; }

  // IFrontPanelNode interface
  QWidget *frontPanelWidget() override { return _lineEdit; }

  QJsonObject save() const override;
  void load(QJsonObject const &p) override;

  void setNumber(double value);

private slots:
  void onTextEdited(QString const &string);

private:
  std::shared_ptr<DecimalData> _number;
  QLineEdit *_lineEdit;
};
