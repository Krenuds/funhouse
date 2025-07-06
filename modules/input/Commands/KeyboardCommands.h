#pragma once

#include "../InputCommand.h"
#include "../../materials/Materials.h"
#include "../../world/World.h"
#include <functional>

namespace Funhouse {

class SelectMaterialCommand : public InputCommand {
public:
    using MaterialCallback = std::function<void(MaterialType)>;
    
    SelectMaterialCommand(MaterialType material, MaterialCallback callback)
        : material_(material), callback_(callback) {}
    
    void Execute() override {
        if (callback_) {
            callback_(material_);
        }
    }
    
    std::string GetName() const override {
        return "SelectMaterial";
    }
    
    std::unique_ptr<InputCommand> Clone() const override {
        return std::make_unique<SelectMaterialCommand>(material_, callback_);
    }
    
private:
    MaterialType material_;
    MaterialCallback callback_;
};

class ToggleRecordingCommand : public InputCommand {
public:
    using ToggleCallback = std::function<void()>;
    
    ToggleRecordingCommand(ToggleCallback callback)
        : callback_(callback) {}
    
    void Execute() override {
        if (callback_) {
            callback_();
        }
    }
    
    std::string GetName() const override {
        return "ToggleRecording";
    }
    
    std::unique_ptr<InputCommand> Clone() const override {
        return std::make_unique<ToggleRecordingCommand>(callback_);
    }
    
    bool IsReplayable() const override {
        return false;
    }
    
private:
    ToggleCallback callback_;
};

class TogglePlaybackCommand : public InputCommand {
public:
    using PlaybackCallback = std::function<void()>;
    
    TogglePlaybackCommand(PlaybackCallback callback)
        : callback_(callback) {}
    
    void Execute() override {
        if (callback_) {
            callback_();
        }
    }
    
    std::string GetName() const override {
        return "TogglePlayback";
    }
    
    std::unique_ptr<InputCommand> Clone() const override {
        return std::make_unique<TogglePlaybackCommand>(callback_);
    }
    
    bool IsReplayable() const override {
        return false;
    }
    
private:
    PlaybackCallback callback_;
};

class ClearWorldCommand : public InputCommand {
public:
    ClearWorldCommand(::World* world)
        : world_(world) {}
    
    void Execute() override {
        if (world_) {
            world_->Clear();
        }
    }
    
    std::string GetName() const override {
        return "ClearWorld";
    }
    
    std::unique_ptr<InputCommand> Clone() const override {
        return std::make_unique<ClearWorldCommand>(world_);
    }
    
private:
    ::World* world_;
};

} // namespace Funhouse