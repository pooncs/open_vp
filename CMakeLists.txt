cmake_minimum_required(VERSION 3.16)

project(QtLV LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set Qt path if not in environment
set(CMAKE_PREFIX_PATH "C:/Qt/6.7.3/msvc2019_64")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets Charts OpenGL PrintSupport Network)
find_package(Qt6SerialPort)

# Optional SerialPort
find_package(Qt6 COMPONENTS SerialPort QUIET)
if (Qt6SerialPort_FOUND)
    add_definitions(-DHAS_SERIALPORT)
endif()

# Configure NodeEditor
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(USE_QT6 ON CACHE BOOL "" FORCE)

# Include Directories (Modularized)
include_directories(
    include
    include/core
    include/nodes
    include/ui
)

add_subdirectory(external/nodeeditor)

add_library(OpenFlowControls STATIC
    src/nodes/BooleanDisplayDataModel.cpp
    include/ui/controls/AbstractControl.h
    include/ui/controls/BooleanLED.h
    include/ui/controls/DigitalGauge.h
    include/ui/controls/ToggleSwitch.h
    include/ui/controls/WaveformGraph.h
    include/ui/controls/Skin.h
    include/ui/controls/StyleManager.h
    include/nodes/BooleanDisplayDataModel.h
)
target_link_libraries(OpenFlowControls PUBLIC Qt6::Widgets Qt6::Core QtNodes)
target_include_directories(OpenFlowControls PUBLIC include)

add_executable(open_vp
    src/main.cpp
    
    # UI
    src/ui/MainWindow.cpp
    src/ui/FrontPanel.cpp
    src/ui/CanvasLayout.cpp
    src/ui/GraphLayout.cpp
    src/ui/ControlsPalette.cpp
    src/ui/QuickDropDialog.cpp
    src/ui/ContextHelp.cpp
    
    # Core
    src/core/ExecutionEngine.cpp
    src/core/DataStore.cpp
    src/core/TypePropagator.cpp
    src/core/NodeRegistry.cpp
    
    # Hardware
    src/hardware/SerialPortImpl.cpp
    
    # Nodes
    src/nodes/ContainerNodeModel.cpp
    src/nodes/SubVINodeModel.cpp
    src/nodes/CustomControls.cpp
    src/nodes/NumberSourceDataModel.cpp
    src/nodes/NumberDisplayDataModel.cpp
    src/nodes/StringDisplayDataModel.cpp
    src/nodes/MathOperationDataModel.cpp
    src/nodes/WaveformDisplayDataModel.cpp
    src/nodes/SignalGeneratorDataModel.cpp
    src/nodes/BooleanSwitchModel.cpp
    src/nodes/ImageNodes.cpp
    src/nodes/PlyLoaderModel.cpp
    src/nodes/PointCloudDisplayModel.cpp
    src/nodes/HardwareNodes.cpp
    src/nodes/AnalysisNodes.cpp
    src/nodes/WhileLoopNodeModel.cpp
    src/nodes/NetworkNodes.cpp
    src/nodes/PythonScriptNodeModel.cpp

    # Headers with Q_OBJECT (UI)
    include/ui/CustomWidgets.h
    include/ui/ControlsPalette.h
    include/ui/QuickDropDialog.h
    include/ui/ContextHelp.h
    include/ui/FlowGraphicsView.h
    include/ui/GridWidget.h
    include/ui/DraggableGroup.h
    include/ui/CanvasLayout.h
    include/ui/LayoutControls.h
    include/ui/MainWindow.h
    include/ui/FrontPanel.h
    include/ui/ProjectExplorer.h
    
    # Hardware (No Q_OBJECT yet, but good to include)
    include/hardware/IHardwareInterface.h
    include/hardware/SerialPortImpl.h
    include/hardware/SimulatedDAQ.h
    
    # Headers with Q_OBJECT (Nodes)
    include/nodes/CustomControls.h
    include/nodes/NetworkNodes.h
    include/nodes/PythonScriptNodeModel.h
    include/nodes/ErrorNodes.h
    include/nodes/ContainerNodeModel.h
    include/nodes/SubVINodeModel.h
    include/nodes/WhileLoopNodeModel.h
    include/nodes/ForLoopNodeModel.h
    include/nodes/CaseStructureNodeModel.h
    include/nodes/SequenceStructureNodeModel.h
    include/nodes/NumberSourceDataModel.h
    include/nodes/NumberDisplayDataModel.h
    include/nodes/StringDisplayDataModel.h
    include/nodes/MathOperationDataModel.h
    include/nodes/WaveformDisplayDataModel.h
    include/nodes/SignalGeneratorDataModel.h
    include/nodes/BooleanSwitchModel.h
    include/nodes/ImageNodes.h
    include/nodes/PlyLoaderModel.h
    include/nodes/PointCloudDisplayModel.h
    include/nodes/GraphTunnelNodes.h
    include/nodes/MathPrimitives.h
    include/nodes/LogicPrimitives.h
    include/nodes/HardwareNodes.h
    include/nodes/AnalysisNodes.h
)
set_target_properties(open_vp PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

enable_testing()
find_package(Qt6 REQUIRED COMPONENTS Test)

add_executable(TestPrimitives tests/TestPrimitives.cpp)
target_include_directories(TestPrimitives PRIVATE include include/nodes include/core)
target_link_libraries(TestPrimitives PRIVATE Qt6::Core Qt6::Test QtNodes)
add_test(NAME TestPrimitives COMMAND TestPrimitives)
set_target_properties(TestPrimitives PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



add_executable(TestExecutionEngine tests/TestExecutionEngine.cpp
    src/core/ExecutionEngine.cpp
    src/core/DataStore.cpp
    src/core/TypePropagator.cpp
    src/nodes/NumberSourceDataModel.cpp
    src/nodes/NumberDisplayDataModel.cpp
    src/nodes/MathOperationDataModel.cpp
    src/nodes/BooleanSwitchModel.cpp
    
    # Headers with Q_OBJECT
    include/core/TypePropagator.h
    include/nodes/NumberSourceDataModel.h
    include/nodes/NumberDisplayDataModel.h
    include/nodes/MathOperationDataModel.h
    include/nodes/MathNodes.h
    include/nodes/BooleanSwitchModel.h
)
target_include_directories(TestExecutionEngine PRIVATE include include/nodes include/core)
target_link_libraries(TestExecutionEngine PRIVATE Qt6::Core Qt6::Test Qt6::Widgets QtNodes OpenFlowControls)
add_test(NAME TestExecutionEngine COMMAND TestExecutionEngine)
set_target_properties(TestExecutionEngine PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(TestParallelExecution tests/TestParallelExecution.cpp
    src/core/ExecutionEngine.cpp
    src/core/DataStore.cpp
    src/core/TypePropagator.cpp
    src/nodes/MathOperationDataModel.cpp
    
    # Headers with Q_OBJECT
    include/core/ExecutionEngine.h
    include/core/TypePropagator.h
    include/nodes/MathOperationDataModel.h
)
target_include_directories(TestParallelExecution PRIVATE include include/nodes include/core)
target_link_libraries(TestParallelExecution PRIVATE Qt6::Core Qt6::Test Qt6::Widgets QtNodes)
add_test(NAME TestParallelExecution COMMAND TestParallelExecution)
set_target_properties(TestParallelExecution PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(TestTypeSystem tests/TestTypeSystem.cpp
    src/core/TypePropagator.cpp
    src/nodes/MathOperationDataModel.cpp
    src/nodes/NumberSourceDataModel.cpp
    src/nodes/NumberDisplayDataModel.cpp
    src/nodes/StringDisplayDataModel.cpp
    
    # Headers with Q_OBJECT
    include/core/TypePropagator.h
    include/nodes/MathOperationDataModel.h
    include/nodes/NumberSourceDataModel.h
    include/nodes/NumberDisplayDataModel.h
    include/nodes/StringDisplayDataModel.h
    include/nodes/ArrayNodes.h
    include/nodes/MathNodes.h
)
target_include_directories(TestTypeSystem PRIVATE include include/nodes include/core)
target_link_libraries(TestTypeSystem PRIVATE Qt6::Core Qt6::Test Qt6::Widgets QtNodes)
add_test(NAME TestTypeSystem COMMAND TestTypeSystem)
set_target_properties(TestTypeSystem PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(TestLogic tests/TestLogic.cpp
    src/core/ExecutionEngine.cpp
    src/core/DataStore.cpp
    src/core/TypePropagator.cpp
    src/nodes/ContainerNodeModel.cpp
    src/nodes/WhileLoopNodeModel.cpp
    src/nodes/BooleanSwitchModel.cpp
    src/nodes/NumberSourceDataModel.cpp
    src/nodes/NumberDisplayDataModel.cpp
    src/nodes/MathOperationDataModel.cpp
    
    # Headers with Q_OBJECT for AUTOMOC
    include/core/TypePropagator.h
    include/nodes/ContainerNodeModel.h
    include/nodes/GraphTunnelNodes.h
    include/nodes/WhileLoopNodeModel.h
    include/nodes/ForLoopNodeModel.h
    include/nodes/NumberSourceDataModel.h
    include/nodes/NumberDisplayDataModel.h
    include/nodes/MathOperationDataModel.h
    include/nodes/MathNodes.h
    include/nodes/BooleanSwitchModel.h
)
target_include_directories(TestLogic PRIVATE include include/nodes include/core)
target_link_libraries(TestLogic PRIVATE Qt6::Core Qt6::Test Qt6::Widgets QtNodes OpenFlowControls)
add_test(NAME TestLogic COMMAND TestLogic)
set_target_properties(TestLogic PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(TestComplexData tests/TestComplexData.cpp
    src/nodes/MathOperationDataModel.cpp
    
    # Headers with Q_OBJECT
    include/nodes/ComplexData.h
    include/nodes/ArrayNodes.h
    include/nodes/ClusterNodes.h
    include/nodes/MathOperationDataModel.h
    include/nodes/MathNodes.h
)
target_include_directories(TestComplexData PRIVATE include include/nodes include/core)
target_link_libraries(TestComplexData PRIVATE Qt6::Core Qt6::Test Qt6::Widgets QtNodes)
add_test(NAME TestComplexData COMMAND TestComplexData)
set_target_properties(TestComplexData PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



add_executable(TestUI tests/TestUI.cpp
    src/ui/FrontPanel.cpp
    src/ui/CanvasLayout.cpp
    src/nodes/CustomControls.cpp
    
    # Headers with Q_OBJECT
    include/ui/FrontPanel.h
    include/ui/CanvasLayout.h
    include/ui/GridWidget.h
    include/ui/DraggableGroup.h
    include/nodes/CustomControls.h
    include/ui/CustomWidgets.h
)
target_include_directories(TestUI PRIVATE include include/nodes include/core include/ui)
target_link_libraries(TestUI PRIVATE Qt6::Core Qt6::Test Qt6::Widgets QtNodes OpenFlowControls)
add_test(NAME TestUI COMMAND TestUI)
set_target_properties(TestUI PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(TestPythonScript tests/TestPythonScript.cpp
    src/nodes/PythonScriptNodeModel.cpp
    
    # Headers with Q_OBJECT
    include/nodes/PythonScriptNodeModel.h
    include/nodes/StringData.h
    include/nodes/ErrorData.h
)
target_include_directories(TestPythonScript PRIVATE include include/nodes)
target_link_libraries(TestPythonScript PRIVATE Qt6::Core Qt6::Test Qt6::Widgets QtNodes)
add_test(NAME TestPythonScript COMMAND TestPythonScript)
set_target_properties(TestPythonScript PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(GenerateSample examples/GenerateSample.cpp
    src/nodes/NumberSourceDataModel.cpp
    src/nodes/NumberDisplayDataModel.cpp
    src/nodes/StringDisplayDataModel.cpp
    src/nodes/PythonScriptNodeModel.cpp
    
    # Headers with Q_OBJECT
    include/nodes/NumberSourceDataModel.h
    include/nodes/NumberDisplayDataModel.h
    include/nodes/StringDisplayDataModel.h
    include/nodes/PythonScriptNodeModel.h
    include/nodes/StringData.h
    include/nodes/ErrorData.h
)
target_include_directories(GenerateSample PRIVATE include include/nodes)
target_link_libraries(GenerateSample PRIVATE Qt6::Core Qt6::Widgets QtNodes)

add_executable(TestPlyLoader tests/TestPlyLoader.cpp
    src/nodes/PlyLoaderModel.cpp
    
    # Headers with Q_OBJECT
    include/nodes/PlyLoaderModel.h
    include/nodes/PointCloudData.h
    include/nodes/ErrorData.h
)
target_include_directories(TestPlyLoader PRIVATE include include/nodes)
target_link_libraries(TestPlyLoader PRIVATE Qt6::Core Qt6::Test Qt6::Widgets QtNodes)
add_test(NAME TestPlyLoader COMMAND TestPlyLoader)
set_target_properties(TestPlyLoader PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_include_directories(open_vp PRIVATE include include/nodes include/core include/hardware)
target_link_libraries(open_vp PRIVATE Qt6::Widgets Qt6::OpenGLWidgets Qt6::Charts Qt6::PrintSupport QtNodes OpenFlowControls Qt6::Network opengl32 glu32)
if (Qt6SerialPort_FOUND)
    target_link_libraries(open_vp PRIVATE Qt6::SerialPort)
    target_compile_definitions(open_vp PRIVATE QT_SERIALPORT_LIB)
endif()
