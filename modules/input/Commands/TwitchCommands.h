#pragma once

#include "../InputCommand.h"
#include "../../twitch/TwitchIrcClient.h"
#include <string>
#include <memory>
#include <functional>

namespace Funhouse {

/**
 * @brief Base class for all Twitch chat commands
 * 
 * This class serves as a bridge between TwitchCommand data and the InputCommand system.
 * It preserves the original Twitch metadata (username, timestamp) while integrating
 * with the existing input command infrastructure.
 */
class TwitchInputCommand : public InputCommand {
public:
    TwitchInputCommand(const TwitchCommand& twitchCmd)
        : username_(twitchCmd.username)
        , command_(twitchCmd.command)
        , parameters_(twitchCmd.parameters) {
        // Preserve the original Twitch timestamp
        timestamp_ = twitchCmd.timestamp;
    }
    
    virtual ~TwitchInputCommand() = default;
    
    // Accessors for Twitch-specific data
    const std::string& GetUsername() const { return username_; }
    const std::string& GetCommand() const { return command_; }
    const std::string& GetParameters() const { return parameters_; }
    
    // InputCommand interface
    std::string GetName() const override {
        return "TwitchCommand:" + command_;
    }
    
protected:
    std::string username_;
    std::string command_;
    std::string parameters_;
};

/**
 * @brief Generic Twitch chat command that executes a callback
 * 
 * This is the most flexible Twitch command type, allowing game code to
 * register callbacks for specific chat commands without creating new classes.
 */
class TwitchChatCommand : public TwitchInputCommand {
public:
    using Callback = std::function<void(const std::string& username, 
                                       const std::string& command, 
                                       const std::string& parameters)>;
    
    TwitchChatCommand(const TwitchCommand& twitchCmd, Callback callback = nullptr)
        : TwitchInputCommand(twitchCmd)
        , callback_(callback) {}
    
    void Execute() override {
        if (callback_) {
            callback_(username_, command_, parameters_);
        }
    }
    
    std::unique_ptr<InputCommand> Clone() const override {
        return std::make_unique<TwitchChatCommand>(
            TwitchCommand(username_, command_, parameters_), 
            callback_
        );
    }
    
    void SetCallback(Callback callback) { callback_ = callback; }
    
private:
    Callback callback_;
};

/**
 * @brief Example specialized Twitch command for spawning materials
 * 
 * This demonstrates how game-specific Twitch commands can be implemented.
 * The actual implementation would depend on having access to World instance.
 */
class TwitchSpawnCommand : public TwitchInputCommand {
public:
    TwitchSpawnCommand(const TwitchCommand& twitchCmd, void* world = nullptr)
        : TwitchInputCommand(twitchCmd)
        , world_(world) {}
    
    void Execute() override {
        // Parse parameters to extract material type and position
        // Example: "!spawn sand 100 200"
        // This would require access to World and material parsing logic
        
        // For now, this is a placeholder that demonstrates the pattern
        // Actual implementation would parse parameters_ and call world methods
    }
    
    std::unique_ptr<InputCommand> Clone() const override {
        return std::make_unique<TwitchSpawnCommand>(
            TwitchCommand(username_, command_, parameters_), 
            world_
        );
    }
    
private:
    void* world_; // Would be ::World* in actual implementation
};

} // namespace Funhouse