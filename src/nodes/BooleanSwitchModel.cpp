#include "BooleanSwitchModel.h"

BooleanSwitchModel::BooleanSwitchModel() : _switch(new ToggleSwitch()) {
  connect(_switch, &ToggleSwitch::valueChanged, this,
          &BooleanSwitchModel::onToggled);
  _data = std::make_shared<BoolData>(false);
}

void BooleanSwitchModel::onToggled(bool checked) {
  _data = std::make_shared<BoolData>(checked);
  Q_EMIT dataUpdated(0);
}

QJsonObject BooleanSwitchModel::save() const {
  QJsonObject modelJson;
  modelJson["value"] = _switch->value();
  modelJson["fp_x"] = _pos.x();
  modelJson["fp_y"] = _pos.y();
  return modelJson;
}

void BooleanSwitchModel::load(QJsonObject const &p) {
  if (p.contains("value")) {
    _switch->setValue(p["value"].toBool());
    // Trigger update?
    onToggled(_switch->value());
  }
  if (p.contains("fp_x") && p.contains("fp_y")) {
    _pos.setX(p["fp_x"].toInt());
    _pos.setY(p["fp_y"].toInt());
  }
}
