#include "DecimalData.h"
#include "MathNodes.h"
#include "NumberDisplayDataModel.h"
#include "NumberSourceDataModel.h"
#include "core/ExecutionEngine.h"
#include <QLabel>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtTest>

using namespace QtNodes;
using namespace OpenFlow::Engine;

class TestExecutionEngine : public QObject {
  Q_OBJECT
private slots:
  void testSimpleFlow();
};

void TestExecutionEngine::testSimpleFlow() {
  // 1. Setup Graph
  auto registry = std::make_shared<NodeDelegateModelRegistry>();
  registry->registerModel<NumberSourceDataModel>("Sources");
  registry->registerModel<NumberDisplayDataModel>("Displays");
  registry->registerModel<AdditionModel>("Operators");

  auto graphModel = std::make_shared<DataFlowGraphModel>(registry);

  // 2. Add Nodes
  auto sourceId = graphModel->addNode("NumberSource");
  auto mathId = graphModel->addNode("Addition");
  auto displayId = graphModel->addNode("NumberDisplay");

  QVERIFY(sourceId != InvalidNodeId);
  QVERIFY(mathId != InvalidNodeId);
  QVERIFY(displayId != InvalidNodeId);

  // 3. Set Data
  // Source -> 5.0
  auto sourceModel = graphModel->delegateModel<NumberSourceDataModel>(sourceId);
  sourceModel->setNumber(5.0);

  // Add second source for math
  auto sourceId2 = graphModel->addNode("NumberSource");
  auto sourceModel2 =
      graphModel->delegateModel<NumberSourceDataModel>(sourceId2);
  sourceModel2->setNumber(10.0);

  // 4. Connect
  // Source1 (0) -> Math (0)
  graphModel->addConnection(ConnectionId{sourceId, 0, mathId, 0});
  // Source2 (0) -> Math (1)
  graphModel->addConnection(ConnectionId{sourceId2, 0, mathId, 1});
  // Math (0) -> Display (0)
  graphModel->addConnection(ConnectionId{mathId, 0, displayId, 0});

  // 5. Run Engine
  ExecutionEngine engine(graphModel);
  engine.compile();
  engine.execute();

  // 6. Verify Result
  auto displayModel =
      graphModel->delegateModel<NumberDisplayDataModel>(displayId);
  QVERIFY(displayModel != nullptr);

  QLabel *label = qobject_cast<QLabel *>(displayModel->frontPanelWidget());
  QVERIFY(label != nullptr);

  // 5 + 10 = 15. The default formatting might vary.
  // Let's check if it contains "15"
  QVERIFY(label->text().contains("15"));
}

QTEST_MAIN(TestExecutionEngine)
#include "TestExecutionEngine.moc"
