#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModelRegistry>

#include "NumberDisplayDataModel.h"
#include "NumberSourceDataModel.h"
#include "PythonScriptNodeModel.h"
#include "StringData.h"
#include "StringDisplayDataModel.h"

using namespace QtNodes;

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  auto registry = std::make_shared<NodeDelegateModelRegistry>();
  registry->registerModel<NumberSourceDataModel>("Sources");
  registry->registerModel<NumberDisplayDataModel>("Displays");
  registry->registerModel<StringDisplayDataModel>("Displays");
  registry->registerModel<PythonScriptNodeModel>("Scripting");

  DataFlowGraphModel graphModel(registry);

  // 1. Create Nodes
  qDebug() << "Creating Nodes...";
  auto sourceId = graphModel.addNode("NumberSource");
  auto scriptId = graphModel.addNode("PythonScript");
  auto displayId = graphModel.addNode("StringDisplay");

  if (sourceId == InvalidNodeId || scriptId == InvalidNodeId ||
      displayId == InvalidNodeId) {
    qCritical() << "Failed to create nodes";
    return 1;
  }

  // 2. Position Nodes
  graphModel.setNodeData(sourceId, NodeRole::Position, QPointF(100, 100));
  graphModel.setNodeData(scriptId, NodeRole::Position, QPointF(400, 100));
  graphModel.setNodeData(displayId, NodeRole::Position, QPointF(700, 100));

  // 3. Configure Nodes
  // Set Source to 5.0
  auto sourceModel = graphModel.delegateModel<NumberSourceDataModel>(sourceId);
  if (sourceModel) {
    sourceModel->setNumber(5.0);
  }

  // Set Script Path
  auto scriptModel = graphModel.delegateModel<PythonScriptNodeModel>(scriptId);
  if (scriptModel) {
    // Use absolute path for robustness in this demo generation
    // Or relative to project root
    QString scriptPath = QDir::currentPath() + "/examples/scripts/square.py";
    // Check if we are in build dir, adjust if needed
    if (!QFile::exists(scriptPath)) {
      // Assume we might be in build/bin or similar, try to find it
      scriptPath = "C:/Users/hmgics/projects/qtlv/examples/scripts/square.py";
    }

    auto pathData = std::make_shared<StringData>(scriptPath);
    scriptModel->setInData(pathData, 0);
  }

  // 4. Connect Nodes
  // Source (0) -> Script (1) [Args]
  graphModel.addConnection(ConnectionId{sourceId, 0, scriptId, 1});

  // Script (0) -> Display (0)
  // Display expects DecimalData. Script outputs StringData.
  // NumberDisplayDataModel expects DecimalData.
  // If I connect String to Decimal, it won't work unless NumberDisplay accepts
  // String. Let's check NumberDisplayDataModel.

  // Assuming NumberDisplay only accepts DecimalData for now.
  // We can't connect String Output of Python to Number Display directly without
  // a converter node. Or we update NumberDisplay to accept String.

  // For this demo, let's create a "String to Number" node? No time.
  // Let's just update NumberDisplayDataModel to accept StringData and try to
  // parse it.

  // Let's skip the output connection for now or assume user will add a
  // converter. Actually, let's just save the file. The user will see the output
  // in the node's tooltip or internal state if we had one. But NumberDisplay is
  // the best way to visualize.

  // I will update NumberDisplayDataModel quickly as well.
  graphModel.addConnection(ConnectionId{scriptId, 0, displayId, 0});

  // 5. Save
  QJsonObject json = graphModel.save();
  QJsonDocument doc(json);

  QFile file("examples/demo_python_math.ofproj");
  if (file.open(QIODevice::WriteOnly)) {
    file.write(doc.toJson());
    qDebug() << "Saved example to" << file.fileName();
  } else {
    qCritical() << "Failed to save file:" << file.errorString();
    return 1;
  }

  return 0;
}
