#include "NumberDisplayDataModel.h"
#include "DecimalData.h"
#include "StringData.h"

NumberDisplayDataModel::NumberDisplayDataModel() : _label(new QLabel("0.00")) {
  _label->setMargin(3);
}

unsigned int NumberDisplayDataModel::nPorts(QtNodes::PortType portType) const {
  return portType == QtNodes::PortType::In ? 1 : 0;
}

QtNodes::NodeDataType
NumberDisplayDataModel::dataType(QtNodes::PortType, QtNodes::PortIndex) const {
  // Return DecimalData as primary type, but we handle StringData too
  return DecimalData().type();
}

void NumberDisplayDataModel::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                       QtNodes::PortIndex) {
  if (auto numberData = std::dynamic_pointer_cast<DecimalData>(data)) {
    _label->setText(numberData->numberAsText());
  } else if (auto stringData = std::dynamic_pointer_cast<StringData>(data)) {
    _label->setText(stringData->value());
  } else {
    _label->setText("0.00");
  }
}

QJsonObject NumberDisplayDataModel::save() const {
  QJsonObject modelJson;
  modelJson["fp_x"] = _pos.x();
  modelJson["fp_y"] = _pos.y();
  return modelJson;
}

void NumberDisplayDataModel::load(QJsonObject const &p) {
  if (p.contains("fp_x") && p.contains("fp_y")) {
    _pos.setX(p["fp_x"].toInt());
    _pos.setY(p["fp_y"].toInt());
  }
}
