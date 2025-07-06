#pragma once

#include <string>
#include <memory>
#include <chrono>

namespace Funhouse {

class InputCommand {
public:
    using Timestamp = std::chrono::steady_clock::time_point;
    
    InputCommand() : timestamp_(std::chrono::steady_clock::now()) {}
    virtual ~InputCommand() = default;
    
    virtual void Execute() = 0;
    
    virtual void Undo() {}
    
    virtual std::string GetName() const = 0;
    
    virtual std::unique_ptr<InputCommand> Clone() const = 0;
    
    Timestamp GetTimestamp() const { return timestamp_; }
    
    virtual bool IsReplayable() const { return true; }
    
protected:
    Timestamp timestamp_;
};

using InputCommandPtr = std::unique_ptr<InputCommand>;

} // namespace Funhouse