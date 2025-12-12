# OpenFlow Custom Node API

This guide explains how to create custom nodes for OpenFlow.

## Directory Structure

The project is organized into modular components:
*   `include/nodes/`: Header files for node models.
*   `src/nodes/`: Implementation files.
*   `include/core/`: Engine and Data types.

## Steps to Create a New Node

### 1. Create the Header File
Create a file in `include/nodes/MyNodeModel.h`.

```cpp
#pragma once
#include <QtNodes/NodeDelegateModel>
#include "DecimalData.h" // Or other data types

class MyNodeModel : public QtNodes::NodeDelegateModel {
public:
    QString caption() const override { return "My Custom Node"; }
    QString name() const override { return "MyNode"; }

    unsigned int nPorts(QtNodes::PortType portType) const override {
        // Example: 1 Input, 1 Output
        return 1;
    }

    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override {
        return DecimalData().type();
    }

    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override {
        return _result;
    }

    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex portIndex) override {
        auto numberData = std::dynamic_pointer_cast<DecimalData>(data);
        if (numberData) {
            // Process data
            double val = numberData->number();
            _result = std::make_shared<DecimalData>(val * 2.0); // Example logic
            Q_EMIT dataUpdated(0);
        }
    }

    QWidget* embeddedWidget() override { return nullptr; }

private:
    std::shared_ptr<DecimalData> _result;
};
```

### 2. Implement Logic (Optional .cpp)
If your logic is complex, create `src/nodes/MyNodeModel.cpp`.

### 3. Register the Node
In `src/ui/MainWindow.cpp`, add your node to the registry:

```cpp
#include "MyNodeModel.h"

// In constructor or initialization:
registry->registerModel<MyNodeModel>("Custom Category");
```

### 4. Update Build System
Add your new files to `CMakeLists.txt`:

```cmake
add_executable(QtLV
    ...
    src/nodes/MyNodeModel.cpp
    include/nodes/MyNodeModel.h
    ...
)
```

## Advanced Nodes

### Executable Nodes
To control execution flow (like Loops), inherit from `OpenFlow::Engine::IExecutableNode`:

```cpp
#include "core/IExecutableNode.h"

class MyLoopNode : public QtNodes::NodeDelegateModel, public OpenFlow::Engine::IExecutableNode {
    void execute(OpenFlow::Engine::ExecutionContext& context) override {
        // Custom execution logic
    }
};
```

### UI Nodes
To display widgets on the Front Panel, inherit from `IFrontPanelNode`:

```cpp
#include "ui/IFrontPanelNode.h"

class MyUINode : public QtNodes::NodeDelegateModel, public IFrontPanelNode {
    QWidget* frontPanelWidget() override { return _myWidget; }
};
```
