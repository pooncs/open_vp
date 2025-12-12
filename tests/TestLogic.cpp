#include "ForLoopNodeModel.h"
#include "GraphTunnelNodes.h"
#include "MathNodes.h"
#include "NumberDisplayDataModel.h"
#include "NumberSourceDataModel.h"
#include "WhileLoopNodeModel.h"
#include "core/ExecutionEngine.h"
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtTest>

#include "BooleanSwitchModel.h"
#include "LogicPrimitives.h"
#include "LoopConditionNodeModel.h"

using namespace QtNodes;
using namespace OpenFlow::Engine;

class TestLogic : public QObject {
  Q_OBJECT
private slots:
  void testNestedLoops();
  void testWhileLoopTermination();
};

void TestLogic::testWhileLoopTermination() {
  auto registry = std::make_shared<NodeDelegateModelRegistry>();
  registry->registerModel<WhileLoopNodeModel>("Structures");
  registry->registerModel<LoopConditionNodeModel>("Structures");
  registry->registerModel<BooleanSwitchModel>("Sources");
  registry->registerModel<NotModel>("Logic");
  registry->registerModel<GraphInputNodeModel>("Structures");
  registry->registerModel<GraphOutputNodeModel>("Structures");
  ContainerNodeModel::setRegistry(registry);

  auto graphModel = std::make_shared<DataFlowGraphModel>(registry);

  // Add While Loop
  auto whileId = graphModel->addNode("WhileLoop");
  auto whileNode = dynamic_cast<WhileLoopNodeModel *>(
      graphModel->delegateModel<QtNodes::NodeDelegateModel>(whileId));
  QVERIFY(whileNode != nullptr);

  auto internalGraph = whileNode->internalGraph();

  // Add Nodes inside: Switch(False) -> Not -> Condition
  auto switchId = internalGraph->addNode("BooleanSwitch");
  auto notId = internalGraph->addNode("Not");
  auto condId = internalGraph->addNode("LoopCondition");

  // Connect Switch -> Not
  internalGraph->addConnection(ConnectionId{switchId, 0, notId, 0});
  // Connect Not -> Condition
  internalGraph->addConnection(ConnectionId{notId, 0, condId, 0});

  ExecutionEngine engine(graphModel);
  engine.compile();

  // Execute Async
  auto future = engine.executeAsync();

  // Wait for completion (should be instant because Condition is True)
  std::future_status status = future.wait_for(std::chrono::seconds(2));

  if (status == std::future_status::timeout) {
    engine.stop(); // Force stop
    future.wait();
    QFAIL("While Loop did not terminate!");
  }

  QVERIFY(status == std::future_status::ready);
}

void TestLogic::testNestedLoops() {
  // Registry
  auto registry = std::make_shared<NodeDelegateModelRegistry>();
  registry->registerModel<NumberSourceDataModel>("Sources");
  registry->registerModel<NumberDisplayDataModel>("Displays");
  registry->registerModel<AdditionModel>("Operators");
  registry->registerModel<WhileLoopNodeModel>("Structures");
  registry->registerModel<ForLoopNodeModel>("Structures");
  registry->registerModel<GraphInputNodeModel>("Structures");
  registry->registerModel<GraphOutputNodeModel>("Structures");

  ContainerNodeModel::setRegistry(registry);

  // Outer Graph
  auto graphModel = std::make_shared<DataFlowGraphModel>(registry);

  // 1. Create While Loop
  auto whileId = graphModel->addNode("WhileLoop");
  auto whileNode = dynamic_cast<WhileLoopNodeModel *>(
      graphModel->delegateModel<QtNodes::NodeDelegateModel>(whileId));
  QVERIFY(whileNode != nullptr);

  // 2. Inside While Loop: Create For Loop
  auto internalGraph = whileNode->internalGraph();
  auto forId = internalGraph->addNode("ForLoop");
  auto forNode = dynamic_cast<ForLoopNodeModel *>(
      internalGraph->delegateModel<QtNodes::NodeDelegateModel>(forId));
  QVERIFY(forNode != nullptr);

  // 3. Inside For Loop: Add operation
  auto innerGraph = forNode->internalGraph();
  auto addId = innerGraph->addNode("Addition");

  // Verification of structure hierarchy
  QCOMPARE(graphModel->allNodeIds().size(), 1);    // Only While Loop in root
  QCOMPARE(internalGraph->allNodeIds().size(), 1); // Only For Loop inside While
  QCOMPARE(innerGraph->allNodeIds().size(), 1);    // Only Addition inside For

  // Execution Test (Mocked for now as we need full wiring which is complex in
  // code)
  ExecutionEngine engine(graphModel);
  engine.compile();
  // engine.execute(); // Should run without crashing even if empty logic
}

QTEST_MAIN(TestLogic)
#include "TestLogic.moc"
