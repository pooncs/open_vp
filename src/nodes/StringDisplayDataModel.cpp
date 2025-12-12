#include "StringDisplayDataModel.h"

StringDisplayDataModel::StringDisplayDataModel() : _label(new QLabel("...")) {
  _label->setMargin(3);
  _label->setWordWrap(true);
}

unsigned int StringDisplayDataModel::nPorts(QtNodes::PortType portType) const {
  return portType == QtNodes::PortType::In ? 1 : 0;
}

QtNodes::NodeDataType
StringDisplayDataModel::dataType(QtNodes::PortType, QtNodes::PortIndex) const {
  return StringData().type();
}

void StringDisplayDataModel::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                       QtNodes::PortIndex) {
  auto strData = std::dynamic_pointer_cast<StringData>(data);
  if (strData) {
    _label->setText(strData->value());
  } else {
    _label->setText("...");
  }
}

QJsonObject StringDisplayDataModel::save() const {
  QJsonObject modelJson;
  modelJson["fp_x"] = _pos.x();
  modelJson["fp_y"] = _pos.y();
  return modelJson;
}

void StringDisplayDataModel::load(QJsonObject const &p) {
  if (p.contains("fp_x") && p.contains("fp_y")) {
    _pos.setX(p["fp_x"].toInt());
    _pos.setY(p["fp_y"].toInt());
  }
}
