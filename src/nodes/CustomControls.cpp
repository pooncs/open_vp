#include "CustomControls.h"

KnobModel::KnobModel() {
  _container = new QWidget();
  auto layout = new QVBoxLayout(_container);

  _knob = new DigitalGauge();

  _label = new QLabel("0.0");
  _label->setAlignment(Qt::AlignCenter);

  layout->addWidget(_knob);
  layout->addWidget(_label);
  layout->setAlignment(_knob, Qt::AlignHCenter);

  connect(_knob, &DigitalGauge::valueChanged, this, &KnobModel::onValueChanged);
  _number = std::make_shared<DecimalData>(0.0);
}

void KnobModel::onValueChanged(double value) {
  _number = std::make_shared<DecimalData>(value);
  _label->setText(QString::number(value, 'f', 1));
  Q_EMIT dataUpdated(0);
}

QJsonObject KnobModel::save() const {
  QJsonObject modelJson;
  modelJson["value"] = _knob->value();
  modelJson["fp_x"] = _pos.x();
  modelJson["fp_y"] = _pos.y();
  return modelJson;
}

void KnobModel::load(QJsonObject const &p) {
  if (p.contains("value")) {
    double val = p["value"].toDouble();
    _knob->setValue(val);
    onValueChanged(val);
  }
  if (p.contains("fp_x") && p.contains("fp_y")) {
    _pos.setX(p["fp_x"].toInt());
    _pos.setY(p["fp_y"].toInt());
  }
}

SlideModel::SlideModel() {
  _container = new QWidget();
  auto layout = new QVBoxLayout(_container);

  _slider = new QSlider(Qt::Vertical);
  _slider->setRange(0, 100);

  _label = new QLabel("0.0");
  _label->setAlignment(Qt::AlignCenter);

  layout->addWidget(_slider);
  layout->addWidget(_label);
  layout->setAlignment(_slider, Qt::AlignHCenter);

  connect(_slider, &QSlider::valueChanged, this, &SlideModel::onValueChanged);
  _number = std::make_shared<DecimalData>(0.0);
}

void SlideModel::onValueChanged(int value) {
  double val = static_cast<double>(value);
  _number = std::make_shared<DecimalData>(val);
  _label->setText(QString::number(val, 'f', 1));
  Q_EMIT dataUpdated(0);
}

QJsonObject SlideModel::save() const {
  QJsonObject modelJson;
  modelJson["value"] = _slider->value();
  modelJson["fp_x"] = _pos.x();
  modelJson["fp_y"] = _pos.y();
  return modelJson;
}

void SlideModel::load(QJsonObject const &p) {
  if (p.contains("value")) {
    int val = p["value"].toInt();
    _slider->setValue(val);
    onValueChanged(val);
  }
  if (p.contains("fp_x") && p.contains("fp_y")) {
    _pos.setX(p["fp_x"].toInt());
    _pos.setY(p["fp_y"].toInt());
  }
}
