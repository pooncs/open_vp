#include "NumberSourceDataModel.h"
#include "DecimalData.h"
#include <QDoubleValidator>

NumberSourceDataModel::NumberSourceDataModel()
    : _lineEdit(new QLineEdit("0.0")) {
  _lineEdit->setValidator(new QDoubleValidator());
  _lineEdit->setMaximumSize(_lineEdit->sizeHint());
  connect(_lineEdit, &QLineEdit::textEdited, this,
          &NumberSourceDataModel::onTextEdited);
  _number = std::make_shared<DecimalData>(0.0);
}

unsigned int NumberSourceDataModel::nPorts(QtNodes::PortType portType) const {
  return portType == QtNodes::PortType::Out ? 1 : 0;
}

QtNodes::NodeDataType
NumberSourceDataModel::dataType(QtNodes::PortType, QtNodes::PortIndex) const {
  return DecimalData().type();
}

std::shared_ptr<QtNodes::NodeData>
NumberSourceDataModel::outData(QtNodes::PortIndex) {
  return _number;
}

void NumberSourceDataModel::onTextEdited(QString const &string) {
  bool ok = false;
  double number = string.toDouble(&ok);
  if (ok) {
    _number = std::make_shared<DecimalData>(number);
    Q_EMIT dataUpdated(0);
  }
}

void NumberSourceDataModel::setNumber(double value) {
  _number = std::make_shared<DecimalData>(value);
  _lineEdit->setText(QString::number(value));
  Q_EMIT dataUpdated(0);
}

QJsonObject NumberSourceDataModel::save() const {
  QJsonObject modelJson;
  modelJson["number"] = _number->number();
  modelJson["fp_x"] = _pos.x();
  modelJson["fp_y"] = _pos.y();
  return modelJson;
}

void NumberSourceDataModel::load(QJsonObject const &p) {
  if (p.contains("number")) {
    double val = p["number"].toDouble();
    setNumber(val);
  }
  if (p.contains("fp_x") && p.contains("fp_y")) {
    _pos.setX(p["fp_x"].toInt());
    _pos.setY(p["fp_y"].toInt());
  }
}
