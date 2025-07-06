#pragma once

#include "../../twitch/TwitchIrcClient.h"
#include "../InputSystem.h"
#include "TwitchCommands.h"
#include <memory>
#include <unordered_map>
#include <functional>

namespace Funhouse {

/**
 * @brief Adapter that bridges TwitchIrcClient with the InputSystem
 * 
 * This class polls the TwitchIrcClient for new commands and converts them
 * into InputCommand objects that can be processed by the InputSystem.
 * It supports both generic command handling and specific command registration.
 */
class TwitchCommandAdapter {
public:
    using CommandHandler = std::function<InputCommandPtr(const TwitchCommand&)>;
    
    TwitchCommandAdapter(TwitchIrcClient* twitchClient, InputSystem* inputSystem)
        : twitchClient_(twitchClient)
        , inputSystem_(inputSystem) {
        
        // Register default handler for unregistered commands
        defaultHandler_ = [](const TwitchCommand& cmd) -> InputCommandPtr {
            return std::make_unique<TwitchChatCommand>(cmd);
        };
    }
    
    /**
     * @brief Poll for new Twitch commands and queue them into InputSystem
     * 
     * This should be called once per frame in the main update loop.
     * It retrieves all new commands from the Twitch client and converts
     * them to InputCommand objects.
     */
    void Update() {
        if (!twitchClient_ || !inputSystem_) {
            return;
        }
        
        // Get all new commands since last update
        auto newCommands = twitchClient_->getCommandBuffer().getNewCommands();
        
        // Process each command
        for (const auto& twitchCmd : newCommands) {
            InputCommandPtr inputCmd = ConvertCommand(twitchCmd);
            if (inputCmd) {
                inputSystem_->QueueCommand(std::move(inputCmd));
            }
        }
    }
    
    /**
     * @brief Register a handler for a specific Twitch command
     * 
     * @param command The command name (without prefix, e.g., "spawn" for "!spawn")
     * @param handler Function that creates an InputCommand from TwitchCommand
     */
    void RegisterCommandHandler(const std::string& command, CommandHandler handler) {
        commandHandlers_[command] = handler;
    }
    
    /**
     * @brief Register a generic callback for a specific command
     * 
     * This is a convenience method that creates a TwitchChatCommand with
     * the provided callback.
     */
    void RegisterCommandCallback(const std::string& command, 
                                TwitchChatCommand::Callback callback) {
        RegisterCommandHandler(command, 
            [callback](const TwitchCommand& cmd) -> InputCommandPtr {
                auto chatCmd = std::make_unique<TwitchChatCommand>(cmd);
                chatCmd->SetCallback(callback);
                return chatCmd;
            });
    }
    
    /**
     * @brief Set the default handler for unregistered commands
     */
    void SetDefaultHandler(CommandHandler handler) {
        defaultHandler_ = handler;
    }
    
    /**
     * @brief Enable or disable processing of unregistered commands
     */
    void SetProcessUnregisteredCommands(bool process) {
        processUnregistered_ = process;
    }
    
    /**
     * @brief Check if the Twitch client is connected
     */
    bool IsConnected() const {
        return twitchClient_ && twitchClient_->isConnected();
    }
    
private:
    /**
     * @brief Convert a TwitchCommand to an InputCommand
     * 
     * Uses registered handlers or the default handler to create
     * appropriate InputCommand instances.
     */
    InputCommandPtr ConvertCommand(const TwitchCommand& twitchCmd) {
        // Look for a registered handler for this command
        auto it = commandHandlers_.find(twitchCmd.command);
        if (it != commandHandlers_.end()) {
            return it->second(twitchCmd);
        }
        
        // Use default handler if processing unregistered commands
        if (processUnregistered_ && defaultHandler_) {
            return defaultHandler_(twitchCmd);
        }
        
        // No handler found and not processing unregistered
        return nullptr;
    }
    
    TwitchIrcClient* twitchClient_;
    InputSystem* inputSystem_;
    
    // Map of command names to their handlers
    std::unordered_map<std::string, CommandHandler> commandHandlers_;
    
    // Default handler for unregistered commands
    CommandHandler defaultHandler_;
    
    // Whether to process commands that don't have specific handlers
    bool processUnregistered_ = true;
};

} // namespace Funhouse