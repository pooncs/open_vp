#pragma once
#include "ErrorData.h"
#include "PointCloudData.h"
#include <QFile>
#include <QFileDialog>
#include <QPushButton>
#include <QTextStream>
#include <QtNodes/NodeDelegateModel>

class PlyLoaderModel : public QtNodes::NodeDelegateModel {
  Q_OBJECT
public:
  PlyLoaderModel();
  virtual ~PlyLoaderModel() = default;

  QString caption() const override { return "Load PLY"; }
  QString name() const override { return "PlyLoader"; }
  bool captionVisible() const override { return true; }

  unsigned int nPorts(QtNodes::PortType portType) const override {
    return portType == QtNodes::PortType::Out ? 2 : 0;
  }
  QtNodes::NodeDataType dataType(QtNodes::PortType,
                                 QtNodes::PortIndex portIndex) const override {
    if (portIndex == 0)
      return PointCloudData().type();
    return ErrorData().type();
  }
  std::shared_ptr<QtNodes::NodeData>
  outData(QtNodes::PortIndex portIndex) override {
    if (portIndex == 0)
      return _points;
    return _errorOut;
  }
  void setInData(std::shared_ptr<QtNodes::NodeData>,
                 QtNodes::PortIndex) override {}
  QWidget *embeddedWidget() override { return _button; }

private slots:
  void onLoadClicked();

private:
  bool parsePly(QString const &filePath, QVector<Point3D> &points,
                QString &errorMsg);

  std::shared_ptr<PointCloudData> _points;
  std::shared_ptr<ErrorData> _errorOut;
  QPushButton *_button;

  // Make available for testing or subclassing
  friend class TestPlyLoader;
  friend class PlyLoaderTestWrapper;
};
