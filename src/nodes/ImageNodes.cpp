#include "ImageNodes.h"

ImageLoaderModel::ImageLoaderModel()
    : _button(new QPushButton("Select Image"))
{
    connect(_button, &QPushButton::clicked, this, &ImageLoaderModel::onLoadClicked);
}

void ImageLoaderModel::onLoadClicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open Image", "", "Images (*.png *.jpg *.bmp)");
    if (!fileName.isEmpty())
    {
        QImage img(fileName);
        if (!img.isNull())
        {
            _image = std::make_shared<ImageData>(img);
            Q_EMIT dataUpdated(0);
        }
    }
}

ImageDisplayModel::ImageDisplayModel()
    : _label(new QLabel("No Image"))
{
    _label->setMinimumSize(200, 200);
    _label->setAlignment(Qt::AlignCenter);
    _label->setStyleSheet("border: 1px solid gray; background-color: black; color: white;");
}

void ImageDisplayModel::setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex)
{
    auto imgData = std::dynamic_pointer_cast<ImageData>(data);
    if (imgData)
    {
        QPixmap pixmap = QPixmap::fromImage(imgData->image());
        _label->setPixmap(pixmap.scaled(_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else
    {
        _label->setText("No Image");
    }
}

QJsonObject ImageDisplayModel::save() const {
  QJsonObject modelJson;
  modelJson["fp_x"] = _pos.x();
  modelJson["fp_y"] = _pos.y();
  return modelJson;
}

void ImageDisplayModel::load(QJsonObject const &p) {
  if (p.contains("fp_x") && p.contains("fp_y")) {
    _pos.setX(p["fp_x"].toInt());
    _pos.setY(p["fp_y"].toInt());
  }
}
