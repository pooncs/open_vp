#include "PlyLoaderModel.h"
#include <QDebug>

PlyLoaderModel::PlyLoaderModel()
    : _button(new QPushButton("Select PLY"))
{
    connect(_button, &QPushButton::clicked, this, &PlyLoaderModel::onLoadClicked);
}

void PlyLoaderModel::onLoadClicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open PLY", "", "PLY Files (*.ply)");
    if (!fileName.isEmpty())
    {
        QVector<Point3D> points;
        QString errorMsg;
        if (parsePly(fileName, points, errorMsg))
        {
            _points = std::make_shared<PointCloudData>(points);
            _errorOut = std::make_shared<ErrorData>(false, 0, "");
        }
        else
        {
            _points.reset();
            _errorOut = std::make_shared<ErrorData>(true, 100, "PLY Parse Error: " + errorMsg);
        }
        Q_EMIT dataUpdated(0);
        Q_EMIT dataUpdated(1);
    }
}

bool PlyLoaderModel::parsePly(QString const& filePath, QVector<Point3D>& points, QString& errorMsg)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMsg = "Cannot open file: " + file.errorString();
        return false;
    }

    QTextStream in(&file);
    QString line;
    bool headerEnded = false;
    int vertexCount = 0;
    
    // Simple ASCII PLY parser
    while (!in.atEnd())
    {
        line = in.readLine();
        if (line.startsWith("element vertex"))
        {
            QStringList parts = line.split(" ");
            if (parts.size() > 2) vertexCount = parts.last().toInt();
        }
        else if (line == "end_header")
        {
            headerEnded = true;
            break;
        }
    }

    if (!headerEnded) {
        errorMsg = "No end_header found";
        return false;
    }
    
    points.reserve(vertexCount);
    
    while (!in.atEnd())
    {
        line = in.readLine();
        QStringList parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.size() >= 3)
        {
            Point3D p;
            bool xOk, yOk, zOk;
            p.position = QVector3D(parts[0].toFloat(&xOk), parts[1].toFloat(&yOk), parts[2].toFloat(&zOk));
            
            if (!xOk || !yOk || !zOk) {
                // Warning or strict fail? Strict for robustness
                errorMsg = "Invalid vertex coordinates in line: " + line;
                return false;
            }
            
            // Try to parse color if available (usually after x,y,z,nx,ny,nz or just after x,y,z)
            // This is a naive assumption that columns 3,4,5 might be colors if available and ints
            // Real parser needs to track property types from header.
            // For now default to white.
            p.color = Qt::white;
            
            if (parts.size() >= 6) {
                 // Check if 3,4,5 look like 0-255 integers
                 // Simple heuristic:
                 bool ok;
                 int r = parts[3].toInt(&ok);
                 if (ok) {
                     int g = parts[4].toInt();
                     int b = parts[5].toInt();
                     p.color = QColor(r, g, b);
                 }
            }
            
            points.append(p);
        }
    }
    
    if (points.size() != vertexCount) {
        // Warning: vertex count mismatch
        // errorMsg = QString("Expected %1 vertices, found %2").arg(vertexCount).arg(points.size());
        // return false; 
        // Allow partial reads but maybe warn?
    }
    
    return true;
}
