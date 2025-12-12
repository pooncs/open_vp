#include "IExecutableNode.h"
#include "core/ExecutionEngine.h"
#include <QElapsedTimer>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtTest>
#include <iostream>
#include <thread>

using namespace QtNodes;
using namespace OpenFlow::Engine;

class DelayNodeModel : public NodeDelegateModel, public IExecutableNode {
  Q_OBJECT
public:
  QString name() const override { return "DelayNode"; }
  QString caption() const override { return "Delay"; }
  unsigned int nPorts(PortType) const override { return 0; }
  NodeDataType dataType(PortType, PortIndex) const override {
    return NodeDataType{"id", "name"};
  }
  std::shared_ptr<NodeData> outData(PortIndex) override { return nullptr; }
  void setInData(std::shared_ptr<NodeData>, PortIndex) override {}
  QWidget *embeddedWidget() override { return nullptr; }

  void execute(ExecutionContext &context) override { QThread::msleep(200); }
};

class TestParallelExecution : public QObject {
  Q_OBJECT
private slots:
  void testParallelExecution();
};

void TestParallelExecution::testParallelExecution() {
  auto registry = std::make_shared<NodeDelegateModelRegistry>();
  registry->registerModel<DelayNodeModel>("Test");

  auto graphModel = std::make_shared<DataFlowGraphModel>(registry);

  // Add 4 independent delay nodes
  // If sequential: 4 * 200ms = 800ms
  // If parallel (assuming >1 core): < 800ms (likely ~200-300ms)

  graphModel->addNode("DelayNode");
  graphModel->addNode("DelayNode");
  graphModel->addNode("DelayNode");
  graphModel->addNode("DelayNode");

  ExecutionEngine engine(graphModel);
  engine.compile();

  QElapsedTimer timer;
  timer.start();

  engine.execute(); // Calls executeAsync().wait()

  qint64 elapsed = timer.elapsed();
  qDebug() << "Elapsed:" << elapsed << "ms";

  unsigned int threads = std::thread::hardware_concurrency();
  if (threads > 1) {
    // 4 nodes * 200ms = 800ms sequential.
    // Parallel should be faster. Let's say < 600ms to be safe.
    QVERIFY2(elapsed < 600, "Execution took too long, likely sequential");
  } else {
    qWarning() << "Single core detected, skipping parallelism check";
  }
}

QTEST_MAIN(TestParallelExecution)
#include "TestParallelExecution.moc"
