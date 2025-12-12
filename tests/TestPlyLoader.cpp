#include "ErrorData.h"
#include "PlyLoaderModel.h"
#include "PointCloudData.h"
#include <QDir>
#include <QVector>
#include <QtTest>


using namespace QtNodes;

class TestPlyLoader : public QObject {
  Q_OBJECT
private slots:
  void testLoadValidPly();
  void testLoadInvalidPly();
  void testMissingFile();
};

// Access private method for testing
class PlyLoaderTestWrapper : public PlyLoaderModel {
public:
  using PlyLoaderModel::parsePly;
};

void TestPlyLoader::testLoadValidPly() {
  PlyLoaderTestWrapper loader;
  QVector<Point3D> points;
  QString errorMsg;

  // Path to test file
  QString filePath = "tests/data/cube.ply";
  // Check relative to build dir or source
  if (!QFile::exists(filePath)) {
    filePath = "../tests/data/cube.ply";
  }
  if (!QFile::exists(filePath)) {
    // Try absolute path if we are in deep build structure
    filePath = "c:/Users/hmgics/projects/qtlv/tests/data/cube.ply";
  }

  QVERIFY2(QFile::exists(filePath), "Test PLY file not found");

  bool result = loader.parsePly(filePath, points, errorMsg);

  QVERIFY2(result, qPrintable(errorMsg));
  QCOMPARE(points.size(), 8);

  // Check first point color
  QCOMPARE(points[0].color, QColor(255, 0, 0));
}

void TestPlyLoader::testLoadInvalidPly() {
  PlyLoaderTestWrapper loader;
  QVector<Point3D> points;
  QString errorMsg;

  // Create invalid file
  QFile file("invalid.ply");
  if (file.open(QIODevice::WriteOnly)) {
    file.write("ply\nformat ascii 1.0\nend_header\nthis is not a number");
    file.close();
  }

  bool result = loader.parsePly("invalid.ply", points, errorMsg);
  QVERIFY(result == false);
  QVERIFY(!errorMsg.isEmpty());
}

void TestPlyLoader::testMissingFile() {
  PlyLoaderTestWrapper loader;
  QVector<Point3D> points;
  QString errorMsg;

  bool result = loader.parsePly("non_existent.ply", points, errorMsg);
  QVERIFY(result == false);
}

QTEST_MAIN(TestPlyLoader)
#include "TestPlyLoader.moc"
