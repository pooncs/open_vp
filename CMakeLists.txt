cmake_minimum_required(VERSION 3.16)

project(QtLV LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/Zc:__cplusplus /permissive-)
endif()

# Disable AUTOMOC as it fails with 3rdParty Qt
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set Qt path
set(CMAKE_PREFIX_PATH 
    "${CMAKE_SOURCE_DIR}/3rdParty/qt6/msvc2019_64"
    "${CMAKE_SOURCE_DIR}/3rdParty/qtnodes/cmake"
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets Charts OpenGL PrintSupport Network Test)
find_package(Qt6SerialPort)

# Optional SerialPort
find_package(Qt6 COMPONENTS SerialPort QUIET)
if (Qt6SerialPort_FOUND)
    add_definitions(-DHAS_SERIALPORT)
endif()

# Configure NodeEditor
find_package(QtNodes REQUIRED)
add_compile_definitions(NODE_EDITOR_SHARED)

# Include Directories
include_directories(
    include
    include/core
    include/nodes
    include/ui
)

# Manual MOC Function
function(my_wrap_cpp outfiles)
    foreach(it ${ARGN})
        get_filename_component(it ${it} ABSOLUTE)
        get_filename_component(base ${it} NAME_WE)
        set(outfile "${CMAKE_CURRENT_BINARY_DIR}/moc_${base}.cpp")
        
        add_custom_command(OUTPUT ${outfile}
            COMMAND "${CMAKE_SOURCE_DIR}/3rdParty/qt6/msvc2019_64/bin/moc.exe"
            ARGS "-DNODE_EDITOR_SHARED" "-DQT_CORE_LIB" "-DQT_GUI_LIB" "-DQT_WIDGETS_LIB" 
                 "-I${CMAKE_SOURCE_DIR}/include" 
                 "-I${CMAKE_SOURCE_DIR}/include/core" 
                 "-I${CMAKE_SOURCE_DIR}/include/nodes" 
                 "-I${CMAKE_SOURCE_DIR}/include/ui" 
                 "-I${CMAKE_SOURCE_DIR}/3rdParty/qt6/msvc2019_64/include"
                 "-I${CMAKE_SOURCE_DIR}/3rdParty/qt6/msvc2019_64/include/QtCore"
                 "-I${CMAKE_SOURCE_DIR}/3rdParty/qt6/msvc2019_64/include/QtGui"
                 "-I${CMAKE_SOURCE_DIR}/3rdParty/qt6/msvc2019_64/include/QtWidgets"
                 "-I${CMAKE_SOURCE_DIR}/3rdParty/qtnodes/include"
                 ${it} -o ${outfile}
            DEPENDS ${it}
            VERBATIM)
        list(APPEND ${outfiles} ${outfile})
    endforeach()
    set(${outfiles} ${${outfiles}} PARENT_SCOPE)
endfunction()

# OpenFlowControls MOC
my_wrap_cpp(CONTROLS_MOC
    include/nodes/BooleanDisplayDataModel.h
    include/ui/controls/AbstractControl.h
    include/ui/controls/BooleanLED.h
    include/ui/controls/ToggleSwitch.h
    include/ui/controls/DigitalGauge.h
    include/ui/controls/WaveformGraph.h
)

add_library(OpenFlowControls STATIC
    src/nodes/BooleanDisplayDataModel.cpp
    include/nodes/BooleanDisplayDataModel.h
    "${CMAKE_SOURCE_DIR}/include/ui/controls/AbstractControl.h"
    "${CMAKE_SOURCE_DIR}/include/ui/controls/BooleanLED.h"
    "${CMAKE_SOURCE_DIR}/include/ui/controls/ToggleSwitch.h"
    "${CMAKE_SOURCE_DIR}/include/ui/controls/DigitalGauge.h"
    "${CMAKE_SOURCE_DIR}/include/ui/controls/WaveformGraph.h"
    "${CMAKE_SOURCE_DIR}/include/ui/controls/StyleManager.h"
    ${CONTROLS_MOC}
)
target_link_libraries(OpenFlowControls PUBLIC Qt6::Widgets Qt6::Core QtNodes::QtNodes)
target_include_directories(OpenFlowControls PUBLIC include)

# QtLV MOC
my_wrap_cpp(QTLV_MOC
    include/ui/MainWindow.h
    include/ui/FrontPanel.h
    include/ui/CanvasLayout.h
    include/ui/ControlsPalette.h
    include/ui/QuickDropDialog.h
    include/ui/ContextHelp.h
    include/ui/FlowGraphicsView.h
    include/ui/GridWidget.h
    include/ui/DraggableGroup.h
    include/nodes/BooleanSwitchModel.h
    include/nodes/CustomControls.h
    include/nodes/ImageNodes.h
    include/nodes/PlyLoaderModel.h
    include/nodes/PointCloudDisplayModel.h
    include/nodes/HardwareNodes.h
    include/nodes/NetworkNodes.h
    include/nodes/NumberSourceDataModel.h
    include/nodes/NumberDisplayDataModel.h
    include/nodes/StringDisplayDataModel.h
    include/nodes/MathOperationDataModel.h
    include/nodes/WaveformDisplayDataModel.h
    include/nodes/SignalGeneratorDataModel.h
    include/nodes/ContainerNodeModel.h
    include/nodes/SubVINodeModel.h
    include/nodes/WhileLoopNodeModel.h
    include/nodes/ForLoopNodeModel.h
    include/nodes/CaseStructureNodeModel.h
    include/nodes/SequenceStructureNodeModel.h
    include/nodes/PythonScriptNodeModel.h
    include/nodes/GraphTunnelNodes.h
    include/nodes/MathPrimitives.h
    include/nodes/LogicPrimitives.h
    include/nodes/AnalysisNodes.h
    include/ui/ProjectExplorer.h
    include/nodes/ErrorNodes.h
    include/ui/LayoutControls.h
)

add_executable(QtLV
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

    ${QTLV_MOC}
)

target_include_directories(QtLV PRIVATE include include/nodes include/core include/hardware)
target_link_libraries(QtLV PRIVATE Qt6::Widgets Qt6::OpenGLWidgets Qt6::Charts Qt6::PrintSupport QtNodes::QtNodes OpenFlowControls Qt6::Network opengl32 glu32)
if (Qt6SerialPort_FOUND)
    target_link_libraries(QtLV PRIVATE Qt6::SerialPort)
    target_compile_definitions(QtLV PRIVATE QT_SERIALPORT_LIB)
endif()

# Copy QtNodes.dll to build directory
add_custom_command(TARGET QtLV POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/3rdParty/qtnodes/bin/QtNodes.dll"
    $<TARGET_FILE_DIR:QtLV>
)

# Copy Qt6 DLLs
set(QT_DLLS 
    Qt6Cored Qt6Guid Qt6Widgetsd Qt6Networkd 
    Qt6OpenGLd Qt6OpenGLWidgetsd Qt6Svgd 
    Qt6Chartsd Qt6PrintSupportd
    Qt6Concurrentd Qt6Xmld
)
foreach(QT_DLL ${QT_DLLS})
    add_custom_command(TARGET QtLV POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/3rdParty/qt6/msvc2019_64/bin/${QT_DLL}.dll"
        $<TARGET_FILE_DIR:QtLV>
    )
endforeach()

# Copy platforms plugin
add_custom_command(TARGET QtLV POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/3rdParty/qt6/msvc2019_64/bin/platforms"
    "$<TARGET_FILE_DIR:QtLV>/platforms"
)

enable_testing()
# Tests are disabled for now to simplify build
