#include "WaveformDisplayDataModel.h"
#include "WaveformData.h"
#include <limits>
#include <vector>

WaveformDisplayDataModel::WaveformDisplayDataModel()
    : _graph(new WaveformGraph()) {}

unsigned int
WaveformDisplayDataModel::nPorts(QtNodes::PortType portType) const {
  return portType == QtNodes::PortType::In ? 1 : 0;
}

QtNodes::NodeDataType
WaveformDisplayDataModel::dataType(QtNodes::PortType,
                                   QtNodes::PortIndex) const {
  return WaveformData().type();
}

void WaveformDisplayDataModel::setInData(
    std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex) {
  auto waveData = std::dynamic_pointer_cast<WaveformData>(data);
  if (waveData) {
    const auto &points = waveData->data();
    std::vector<double> yValues;
    yValues.reserve(points.size());

    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::lowest();

    for (const auto &p : points) {
      yValues.push_back(p.y());
      if (p.y() < min)
        min = p.y();
      if (p.y() > max)
        max = p.y();
    }

    if (yValues.empty()) {
      min = -1.0;
      max = 1.0;
    } else if (min == max) {
      min -= 1.0;
      max += 1.0;
    }

    _graph->setData(yValues);
    _graph->setRange(min, max);
  } else {
    _graph->setData({});
  }
}

QJsonObject WaveformDisplayDataModel::save() const {
  QJsonObject modelJson;
  modelJson["fp_x"] = _pos.x();
  modelJson["fp_y"] = _pos.y();
  return modelJson;
}

void WaveformDisplayDataModel::load(QJsonObject const &p) {
  if (p.contains("fp_x") && p.contains("fp_y")) {
    _pos.setX(p["fp_x"].toInt());
    _pos.setY(p["fp_y"].toInt());
  }
}
