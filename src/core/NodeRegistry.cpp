#include "core/NodeRegistry.h"

#include "ArrayNodes.h"
#include "ClusterNodes.h"
#include "FileNodes.h"
#include "SubVINodeModel.h"
#include "WaveformDisplayDataModel.h"

#include "GraphTunnelNodes.h"
#include "MathNodes.h"
#include "NumberDisplayDataModel.h"
#include "NumberSourceDataModel.h"
#include "StringDisplayDataModel.h"
#include "WhileLoopNodeModel.h"
#include "ContainerNodeModel.h"

// New Widgets
#include "AnalysisNodes.h"
#include "BooleanDisplayDataModel.h"
#include "BooleanSwitchModel.h"
#include "CaseStructureNodeModel.h"
#include "CustomControls.h"
#include "ErrorNodes.h"
#include "ForLoopNodeModel.h"
#include "HardwareNodes.h"
#include "ImageNodes.h"
#include "LayoutControls.h"
#include "LogicPrimitives.h"
#include "MathPrimitives.h"
#include "NetworkNodes.h"
#include "PlyLoaderModel.h"
#include "PointCloudDisplayModel.h"
#include "PythonScriptNodeModel.h"
#include "SequenceStructureNodeModel.h"
#include "SignalGeneratorDataModel.h"

using QtNodes::NodeDelegateModelRegistry;

namespace OpenFlow::Core {

std::shared_ptr<NodeDelegateModelRegistry> createNodeRegistry() {
  auto registry = std::make_shared<NodeDelegateModelRegistry>();

  registry->registerModel<NumberSourceDataModel>("Sources");
  registry->registerModel<NumberDisplayDataModel>("Displays");
  registry->registerModel<StringDisplayDataModel>("Displays");
  registry->registerModel<AdditionModel>("Math");
  registry->registerModel<SubtractionModel>("Math");
  registry->registerModel<MultiplicationModel>("Math");
  registry->registerModel<DivisionModel>("Math");

  registry->registerModel<GraphInputNodeModel>("Structures");
  registry->registerModel<GraphOutputNodeModel>("Structures");

  ContainerNodeModel::setRegistry(registry);

  registry->registerModel<WhileLoopNodeModel>("Structures");
  registry->registerModel<SubVINodeModel>("Structures");

  registry->registerModel<UnbundleNodeModel>("Cluster");
  registry->registerModel<BundleNodeModel>("Cluster");
  registry->registerModel<BuildArrayNodeModel>("Array");
  registry->registerModel<IndexArrayNodeModel>("Array");

  registry->registerModel<WriteFileNodeModel>("File I/O");
  registry->registerModel<ReadFileNodeModel>("File I/O");
  registry->registerModel<PythonScriptNodeModel>("Scripting");

  registry->registerModel<ForLoopNodeModel>("Structures");
  registry->registerModel<CaseStructureNodeModel>("Structures");
  registry->registerModel<SequenceStructureNodeModel>("Structures");

  // New Widgets
  registry->registerModel<WaveformDisplayDataModel>("Displays");
  registry->registerModel<BooleanDisplayDataModel>("Displays");
  registry->registerModel<SignalGeneratorDataModel>("Sources");
  registry->registerModel<BooleanSwitchModel>("Sources");
  registry->registerModel<KnobModel>("Sources");
  registry->registerModel<SlideModel>("Sources");

  // Layouts
  registry->registerModel<TabControlModel>("Layouts");
  registry->registerModel<SplitterModel>("Layouts");

  // Math Primitives
  registry->registerModel<SinModel>("Math");
  registry->registerModel<CosModel>("Math");
  registry->registerModel<TanModel>("Math");
  registry->registerModel<SqrtModel>("Math");
  registry->registerModel<AbsModel>("Math");

  // Logic Primitives
  registry->registerModel<AndModel>("Logic");
  registry->registerModel<OrModel>("Logic");
  registry->registerModel<NotModel>("Logic");

  // Image
  registry->registerModel<ImageLoaderModel>("Sources");
  registry->registerModel<ImageDisplayModel>("Displays");

  // Point Cloud
  registry->registerModel<PlyLoaderModel>("Sources");
  registry->registerModel<PointCloudDisplayModel>("Displays");

  // Hardware
  registry->registerModel<SerialReadNodeModel>("Hardware");
  registry->registerModel<SimulatedDAQNodeModel>("Hardware");
  registry->registerModel<TCPClientNodeModel>("Hardware");
  registry->registerModel<TCPServerNodeModel>("Hardware");

  // Error Handling
  registry->registerModel<ClearErrorNodeModel>("Dialog & User Interface");
  registry->registerModel<GeneralErrorHandlerNodeModel>(
      "Dialog & User Interface");

  // Analysis
  registry->registerModel<FFTNodeModel>("Analysis");

  return registry;
}

} // namespace OpenFlow::Core
