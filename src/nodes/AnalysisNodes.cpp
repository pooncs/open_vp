#include "AnalysisNodes.h"
#include <cmath>

// Simple DFT implementation (O(N^2)) - Slow but sufficient for small N demo
// For production, use FFTW or KissFFT
void simpleDFT(const QVector<QPointF>& input, QVector<QPointF>& output)
{
    int N = input.size();
    output.resize(N/2); // Return magnitude of first half (Nyquist)

    for (int k = 0; k < N/2; ++k)
    {
        std::complex<double> sum(0, 0);
        for (int n = 0; n < N; ++n)
        {
            double angle = 2 * 3.14159265358979323846 * k * n / N;
            std::complex<double> w(std::cos(angle), -std::sin(angle));
            sum += input[n].y() * w;
        }
        // Magnitude
        output[k] = QPointF(k, std::abs(sum)); // X axis = bin index
    }
}

void FFTNodeModel::setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex)
{
    auto waveData = std::dynamic_pointer_cast<WaveformData>(data);
    if (waveData && !waveData->data().isEmpty())
    {
        QVector<QPointF> spectrum;
        simpleDFT(waveData->data(), spectrum);
        _result = std::make_shared<WaveformData>(spectrum);
    }
    else
    {
        _result.reset();
    }
    Q_EMIT dataUpdated(0);
}
