#include "BooleanDisplayDataModel.h"

BooleanDisplayDataModel::BooleanDisplayDataModel() : _led(new BooleanLED()) {}

void BooleanDisplayDataModel::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                        QtNodes::PortIndex) {
  auto boolData = std::dynamic_pointer_cast<BoolData>(data);
  if (boolData) {
    _led->setValue(boolData->value());
  } else {
    _led->setValue(false);
  }
}

QJsonObject BooleanDisplayDataModel::save() const {
  QJsonObject modelJson;
  modelJson["fp_x"] = _pos.x();
  modelJson["fp_y"] = _pos.y();
  return modelJson;
}

void BooleanDisplayDataModel::load(QJsonObject const &p) {
  if (p.contains("fp_x") && p.contains("fp_y")) {
    _pos.setX(p["fp_x"].toInt());
    _pos.setY(p["fp_y"].toInt());
  }
}
