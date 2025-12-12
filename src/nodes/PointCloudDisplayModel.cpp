#include "PointCloudDisplayModel.h"
#include <cmath>

PointCloudWidget::PointCloudWidget(QWidget *parent) : QOpenGLWidget(parent) {
  setMinimumSize(400, 300);
}

void PointCloudWidget::setPoints(QVector<Point3D> const &points) {
  _points = points;
  update();
}

void PointCloudWidget::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glEnable(GL_DEPTH_TEST);
}

void PointCloudWidget::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // Simple Perspective
  float aspect = float(w) / float(h ? h : 1);
  float zNear = 0.1f, zFar = 1000.0f;
  float fov = 45.0f;
  float f = 1.0f / std::tan(fov * 3.1415926f / 360.0f);

  // Frustum
  float xMin = -zNear / f * aspect; // error in calc, use gluPerspective logic
  // Manual gluPerspective
  float yMax = zNear * std::tan(fov * 3.1415926f / 360.0f);
  float yMin = -yMax;
  float xMax = yMax * aspect;
  float xMin2 = yMin * aspect; // renamed to avoid conflict
  glFrustum(xMin2, xMax, yMin, yMax, zNear, zFar);

  glMatrixMode(GL_MODELVIEW);
}

void PointCloudWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  glTranslatef(0.0f, 0.0f, _zoom);
  glRotatef(_xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(_yRot, 0.0f, 1.0f, 0.0f);

  // Draw Axis
  glBegin(GL_LINES);
  glColor3f(1, 0, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(1, 0, 0);
  glColor3f(0, 1, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 1, 0);
  glColor3f(0, 0, 1);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, 1);
  glEnd();

  // Draw Points
  glPointSize(2.0f);
  glBegin(GL_POINTS);
  for (const auto &p : _points) {
    glColor3f(p.color.redF(), p.color.greenF(), p.color.blueF());
    glVertex3f(p.position.x(), p.position.y(), p.position.z());
  }
  glEnd();
}

void PointCloudWidget::mousePressEvent(QMouseEvent *event) {
  _lastPos = event->pos();
}

void PointCloudWidget::mouseMoveEvent(QMouseEvent *event) {
  int dx = event->pos().x() - _lastPos.x();
  int dy = event->pos().y() - _lastPos.y();

  if (event->buttons() & Qt::LeftButton) {
    _xRot += 8 * dy;
    _yRot += 8 * dx;
  } else if (event->buttons() & Qt::RightButton) {
    _zoom += dy * 0.1f;
  }
  _lastPos = event->pos();
  update();
}

// ---------------------------

PointCloudDisplayModel::PointCloudDisplayModel()
    : _widget(new PointCloudWidget()) {}

void PointCloudDisplayModel::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                       QtNodes::PortIndex) {
  auto pcData = std::dynamic_pointer_cast<PointCloudData>(data);
  if (pcData) {
    _widget->setPoints(pcData->points());
  } else {
    _widget->setPoints({});
  }
}

QJsonObject PointCloudDisplayModel::save() const {
  QJsonObject modelJson;
  modelJson["fp_x"] = _pos.x();
  modelJson["fp_y"] = _pos.y();
  return modelJson;
}

void PointCloudDisplayModel::load(QJsonObject const &p) {
  if (p.contains("fp_x") && p.contains("fp_y")) {
    _pos.setX(p["fp_x"].toInt());
    _pos.setY(p["fp_y"].toInt());
  }
}
