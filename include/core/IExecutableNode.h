#pragma once

namespace OpenFlow::Engine {

class ExecutionContext {
public:
    virtual bool isStopRequested() const = 0;
};

class IExecutableNode {
public:
    virtual ~IExecutableNode() = default;
    virtual void execute(ExecutionContext& context) = 0;
};

} // namespace OpenFlow::Engine
