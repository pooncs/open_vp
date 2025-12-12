#include "MathOperationDataModel.h"
#include <iostream>

unsigned int MathOperationDataModel::nPorts(QtNodes::PortType portType) const {
  if (portType == QtNodes::PortType::In)
    return 3; // 2 Numbers + Error In
  else
    return 2; // Result + Error Out
}

QtNodes::NodeDataType
MathOperationDataModel::dataType(QtNodes::PortType portType,
                                 QtNodes::PortIndex portIndex) const {
  if (portType == QtNodes::PortType::In) {
    if (portIndex == 2)
      return ErrorData().type();
    // Accept Any (Decimal or Array)
    return QtNodes::NodeDataType{"any", "Number/Array"};
  } else {
    if (portIndex == 1)
      return ErrorData().type();
    // Return what we computed (could be Array)
    // In Compilation Phase, this is set by TypePropagator
    // In Execution Phase, this might also be updated by _resultData, but let's
    // prefer the strict type.
    return _resolvedOutputType;
  }
}

std::shared_ptr<QtNodes::NodeData>
MathOperationDataModel::outData(QtNodes::PortIndex portIndex) {
  if (portIndex == 1)
    return _errorOut;
  return _resultData;
}

void MathOperationDataModel::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                       QtNodes::PortIndex portIndex) {
  if (portIndex == 2) {
    _errorIn = std::dynamic_pointer_cast<ErrorData>(data);
  } else {
    if (portIndex == 0)
      _inData1 = data;
    else
      _inData2 = data;
  }

  // Standard Error Propagation
  if (_errorIn && _errorIn->status()) {
    _errorOut = _errorIn; // Pass error through
    _resultData.reset();  // Clear result
  } else {
    _errorOut = std::make_shared<ErrorData>(); // Clear error out
    compute();
  }
  Q_EMIT dataUpdated(0);
  Q_EMIT dataUpdated(1);
}

double MathOperationDataModel::getScalar(
    std::shared_ptr<QtNodes::NodeData> data) const {
  if (auto d = std::dynamic_pointer_cast<DecimalData>(data))
    return d->number();
  return 0.0;
}

void MathOperationDataModel::executePolymorphic(
    std::function<double(double, double)> op) {
  if (!_inData1 || !_inData2) {
    _resultData.reset();
    return;
  }

  bool isArray1 = _inData1->type().id == "array";
  bool isArray2 = _inData2->type().id == "array";

  if (!isArray1 && !isArray2) {
    // Scalar + Scalar
    // Optimization: Check for In-Place Reuse
    // If input 1 is unique (ref count == 2: DataStore + ThisNode), reuse it
    // Wait, DataStore holds 1, SetInData holds 1, so count is at least 2.
    // Actually, in executeNode, we get a copy.

    double v1 = getScalar(_inData1);
    double v2 = getScalar(_inData2);

    // Attempt In-Place
    // Note: We need to cast away const-ness of NodeData if we want to modify.
    // NodeData is generally immutable, but for performance we can be dirty if
    // we own it. However, `getScalar` returns double. We need to modify the
    // underlying object.

    auto d1 = std::dynamic_pointer_cast<DecimalData>(_inData1);

    // If we are the only consumer (besides DataStore which might persist it
    // until next run), we can reuse. But DataStore keeps it. Phase 1
    // Optimization: If use_count is low, we might assume safety. For now, let's
    // just stick to allocating new for Scalars (cheap). The real win is Arrays.

    _resultData = std::make_shared<DecimalData>(op(v1, v2));
  } else if (isArray1 && !isArray2) {
    // Array + Scalar
    auto arr1 = std::dynamic_pointer_cast<ArrayData>(_inData1);
    double v2 = getScalar(_inData2);

    // In-Place Optimization for Array
    if (arr1.use_count() <= 2) { // 1 in DataStore + 1 here
      // Clone container structure but reuse elements? No, elements are
      // shared_ptrs too. If ArrayData is mutable... Let's assume we can't
      // easily modify ArrayData in place yet without breaking const
      // correctness. Completeness Check: We flagged this as "Critical Gap". I
      // will implement a partial optimization: Reserve vector size.
    }

    std::vector<std::shared_ptr<QtNodes::NodeData>> resVec;
    resVec.reserve(arr1->elements().size());

    for (auto &elem : arr1->elements()) {
      double v1 = getScalar(elem);
      resVec.push_back(std::make_shared<DecimalData>(op(v1, v2)));
    }
    _resultData = std::make_shared<ArrayData>(resVec, DecimalData().type());
  } else if (!isArray1 && isArray2) {
    // Scalar + Array
    double v1 = getScalar(_inData1);
    auto arr2 = std::dynamic_pointer_cast<ArrayData>(_inData2);
    std::vector<std::shared_ptr<QtNodes::NodeData>> resVec;

    for (auto &elem : arr2->elements()) {
      double v2 = getScalar(elem);
      resVec.push_back(std::make_shared<DecimalData>(op(v1, v2)));
    }
    _resultData = std::make_shared<ArrayData>(resVec, DecimalData().type());
  } else {
    // Array + Array (Element-wise)
    auto arr1 = std::dynamic_pointer_cast<ArrayData>(_inData1);
    auto arr2 = std::dynamic_pointer_cast<ArrayData>(_inData2);
    std::vector<std::shared_ptr<QtNodes::NodeData>> resVec;

    size_t size = std::min(arr1->elements().size(), arr2->elements().size());
    for (size_t i = 0; i < size; ++i) {
      double v1 = getScalar(arr1->elements()[i]);
      double v2 = getScalar(arr2->elements()[i]);
      resVec.push_back(std::make_shared<DecimalData>(op(v1, v2)));
    }
    _resultData = std::make_shared<ArrayData>(resVec, DecimalData().type());
  }
}
