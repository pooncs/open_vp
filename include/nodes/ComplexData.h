#pragma once
#include <QString>
#include <QtNodes/NodeData>
#include <memory>
#include <variant>
#include <vector>


using QtNodes::NodeData;
using QtNodes::NodeDataType;

// Forward declarations
class DecimalData;
class BoolData;
class StringData;
class ErrorData;

/**
 * @brief Generic wrapper for any data type in OpenFlow.
 * This is the base for Arrays and Clusters to hold mixed types.
 * For now, we rely on std::shared_ptr<NodeData> as the polymorphic holder.
 */

// ----------------------------------------------------------------------------
// Array Data (Homogeneous List)
// ----------------------------------------------------------------------------
class ArrayData : public NodeData {
public:
  ArrayData() = default;
  ArrayData(const std::vector<std::shared_ptr<NodeData>> &elements,
            const NodeDataType &elementType)
      : _elements(elements), _elementType(elementType) {}

  NodeDataType type() const override {
    return NodeDataType{"array", "Array of " + _elementType.name};
  }

  const std::vector<std::shared_ptr<NodeData>> &elements() const {
    return _elements;
  }
  NodeDataType elementType() const { return _elementType; }

private:
  std::vector<std::shared_ptr<NodeData>> _elements;
  NodeDataType _elementType;
};

// ----------------------------------------------------------------------------
// Cluster Data (Heterogeneous Ordered Set / Struct)
// ----------------------------------------------------------------------------
class ClusterData : public NodeData {
public:
  ClusterData() = default;
  ClusterData(const std::vector<std::shared_ptr<NodeData>> &elements)
      : _elements(elements) {}

  NodeDataType type() const override {
    return NodeDataType{"cluster", "Cluster"};
  }

  const std::vector<std::shared_ptr<NodeData>> &elements() const {
    return _elements;
  }

private:
  std::vector<std::shared_ptr<NodeData>> _elements;
};
