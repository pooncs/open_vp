#include "SignalGeneratorDataModel.h"

SignalGeneratorDataModel::SignalGeneratorDataModel()
{
    // Generate a simple sine wave
    QVector<QPointF> points;
    for (int i = 0; i < 100; ++i)
    {
        double x = i / 10.0;
        double y = std::sin(x);
        points.append(QPointF(x, y));
    }
    _waveform = std::make_shared<WaveformData>(points);
}
