#include "SubVINodeModel.h"
#include <QFile>
#include <QJsonArray>

// Implementation of setPath to actually load the graph
void SubVINodeModel::setPath(const QString &path) {
  _path = path;
  if (_path.isEmpty())
    return;

  QFile file(_path);
  if (!file.open(QIODevice::ReadOnly)) {
    return;
  }

  QByteArray data = file.readAll();
  QJsonDocument doc = QJsonDocument::fromJson(data);
  QJsonObject json = doc.object();

  // We need to load this into _internalGraph
  // But _internalGraph is a shared_ptr, and DataFlowGraphModel might not
  // support direct loading from JSON if it's not exposed. Usually, we use the
  // Scene to load, but here we only have the Model. We might need to manually
  // parse nodes and connections.

  // Assuming DataFlowGraphModel has a registry set
  // We need to use the registry to recreate nodes.

  // For now, let's assume we can reuse the "load" method if we had a scene.
  // Since we don't have a scene, we must rely on the GraphModel's ability to
  // deserialize. QtNodes::DataFlowGraphModel doesn't have a load() method by
  // default (it's in the Scene).

  // WORKAROUND: We will parse the JSON manually and create nodes in the
  // internal graph. This is complex because we need the registry.

  if (auto registry = ContainerNodeModel::getRegistry()) {
    _internalGraph = std::make_shared<QtNodes::DataFlowGraphModel>(registry);

    // Map from File Node ID to New Graph Node ID
    std::map<QtNodes::NodeId, QtNodes::NodeId> idMapping;

    QJsonObject nodesJson = json["nodes"].toObject();
    for (auto it = nodesJson.begin(); it != nodesJson.end(); ++it) {
      QtNodes::NodeId oldId = it.key().toUInt();
      QJsonObject nodeJson = it.value().toObject();
      QString modelName = nodeJson["model"].toObject()["name"].toString();

      auto nodeId = _internalGraph->addNode(modelName);
      if (nodeId != QtNodes::InvalidNodeId) {
        idMapping[oldId] = nodeId;
        // Restore internal data/properties
        auto model =
            _internalGraph->delegateModel<QtNodes::NodeDelegateModel>(nodeId);
        if (model) {
          model->load(nodeJson["model"].toObject());
        }
      }
    }

    // Restore Connections
    QJsonArray connectionsJson = json["connections"].toArray();
    for (const auto val : connectionsJson) {
      QJsonObject connJson = val.toObject();
      QtNodes::NodeId outNodeId = connJson["outNodeId"].toInt();
      QtNodes::NodeId inNodeId = connJson["inNodeId"].toInt();
      int outPortIndex = connJson["outPortIndex"].toInt();
      int inPortIndex = connJson["inPortIndex"].toInt();

      if (idMapping.count(outNodeId) && idMapping.count(inNodeId)) {
        _internalGraph->addConnection(
            QtNodes::ConnectionId{idMapping[outNodeId], (size_t)outPortIndex,
                                  idMapping[inNodeId], (size_t)inPortIndex});
      }
    }
  }
}
