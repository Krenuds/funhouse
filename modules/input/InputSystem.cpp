#include "InputSystem.h"
#include <algorithm>

namespace Funhouse {

InputSystem::InputSystem() {
    const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        keyboardState_.keysPressed[static_cast<SDL_Scancode>(i)] = keyboardState[i] != 0;
        keyboardState_.keysJustPressed[static_cast<SDL_Scancode>(i)] = false;
        keyboardState_.keysJustReleased[static_cast<SDL_Scancode>(i)] = false;
    }
}

InputSystem::~InputSystem() = default;

void InputSystem::ProcessEvent(const SDL_Event& event, ::World* world) {
    world_ = world;
    
    UpdateMouseState(event);
    UpdateKeyboardState(event);
    
    // First try context-based input handling
    InputCommandPtr contextCommand = nullptr;
    SDL_Keymod modifiers = static_cast<SDL_Keymod>(SDL_GetModState());
    
    switch (event.type) {
        case SDL_KEYDOWN:
            contextCommand = contextManager_.ProcessKeyDown(event.key.keysym.scancode, modifiers);
            break;
        case SDL_KEYUP:
            contextCommand = contextManager_.ProcessKeyUp(event.key.keysym.scancode, modifiers);
            break;
        case SDL_MOUSEBUTTONDOWN:
            contextCommand = contextManager_.ProcessMouseButtonDown(event.button.button, modifiers);
            break;
        case SDL_MOUSEBUTTONUP:
            contextCommand = contextManager_.ProcessMouseButtonUp(event.button.button, modifiers);
            break;
        case SDL_MOUSEWHEEL:
            contextCommand = contextManager_.ProcessMouseWheel(event.wheel.y);
            break;
        case SDL_MOUSEMOTION:
            contextCommand = contextManager_.ProcessMouseMotion();
            break;
    }
    
    if (contextCommand) {
        QueueCommand(std::move(contextCommand));
        return; // Context handled the input, don't process legacy bindings
    }
    
    // Fall back to legacy factory-based handling
    auto eventIt = eventFactories_.find(event.type);
    if (eventIt != eventFactories_.end()) {
        for (const auto& factory : eventIt->second) {
            if (auto command = factory(event)) {
                QueueCommand(std::move(command));
            }
        }
    }
    
    if (event.type == SDL_KEYDOWN) {
        auto keyIt = keyFactories_.find(event.key.keysym.scancode);
        if (keyIt != keyFactories_.end()) {
            for (const auto& factory : keyIt->second) {
                if (auto command = factory(event)) {
                    QueueCommand(std::move(command));
                }
            }
        }
    }
    
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        auto mouseIt = mouseButtonFactories_.find(event.button.button);
        if (mouseIt != mouseButtonFactories_.end()) {
            for (const auto& factory : mouseIt->second) {
                if (auto command = factory(event)) {
                    QueueCommand(std::move(command));
                }
            }
        }
    }
}

void InputSystem::Update() {
    ClearKeyboardTransitions();
    
    if (isPlayingBack_ && playbackIndex_ < playbackCommands_.size()) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - playbackStartTime_).count();
        
        while (playbackIndex_ < playbackCommands_.size()) {
            const auto& command = playbackCommands_[playbackIndex_];
            auto commandTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                command->GetTimestamp() - playbackCommands_[0]->GetTimestamp()).count();
            
            if (commandTime <= elapsed) {
                QueueCommand(command->Clone());
                playbackIndex_++;
            } else {
                break;
            }
        }
        
        if (playbackIndex_ >= playbackCommands_.size()) {
            StopPlayback();
        }
    }
}

void InputSystem::ExecuteCommands() {
    while (!commandQueue_.empty()) {
        auto& command = commandQueue_.front();
        command->Execute();
        
        if (isRecording_ && command->IsReplayable()) {
            recordedCommands_.push_back(command->Clone());
        }
        
        commandQueue_.pop();
    }
}

void InputSystem::RegisterCommandFactory(Uint32 eventType, CommandFactory factory) {
    eventFactories_[eventType].push_back(factory);
}

void InputSystem::RegisterKeyCommandFactory(SDL_Scancode key, CommandFactory factory) {
    keyFactories_[key].push_back(factory);
}

void InputSystem::RegisterMouseCommandFactory(Uint8 button, CommandFactory factory) {
    mouseButtonFactories_[button].push_back(factory);
}

void InputSystem::QueueCommand(InputCommandPtr command) {
    commandQueue_.push(std::move(command));
}

void InputSystem::StartRecording() {
    isRecording_ = true;
    recordedCommands_.clear();
}

void InputSystem::StopRecording() {
    isRecording_ = false;
}

void InputSystem::StartPlayback(const std::vector<InputCommandPtr>& commands) {
    if (commands.empty()) return;
    
    playbackCommands_.clear();
    playbackCommands_.reserve(commands.size());
    for (const auto& cmd : commands) {
        playbackCommands_.push_back(cmd->Clone());
    }
    
    isPlayingBack_ = true;
    playbackIndex_ = 0;
    playbackStartTime_ = std::chrono::steady_clock::now();
}

void InputSystem::StopPlayback() {
    isPlayingBack_ = false;
    playbackCommands_.clear();
    playbackIndex_ = 0;
}

void InputSystem::UpdateMouseState(const SDL_Event& event) {
    switch (event.type) {
        case SDL_MOUSEMOTION:
            mouseState_.x = event.motion.x;
            mouseState_.y = event.motion.y;
            break;
            
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                mouseState_.leftPressed = true;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                mouseState_.rightPressed = true;
            } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                mouseState_.middlePressed = true;
            }
            break;
            
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                mouseState_.leftPressed = false;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                mouseState_.rightPressed = false;
            } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                mouseState_.middlePressed = false;
            }
            break;
            
        case SDL_MOUSEWHEEL:
            mouseState_.wheelDelta = event.wheel.y;
            break;
    }
}

void InputSystem::UpdateKeyboardState(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Scancode scancode = event.key.keysym.scancode;
        if (!keyboardState_.keysPressed[scancode]) {
            keyboardState_.keysJustPressed[scancode] = true;
        }
        keyboardState_.keysPressed[scancode] = true;
    } else if (event.type == SDL_KEYUP) {
        SDL_Scancode scancode = event.key.keysym.scancode;
        keyboardState_.keysPressed[scancode] = false;
        keyboardState_.keysJustReleased[scancode] = true;
    }
}

void InputSystem::ClearKeyboardTransitions() {
    for (auto& pair : keyboardState_.keysJustPressed) {
        pair.second = false;
    }
    for (auto& pair : keyboardState_.keysJustReleased) {
        pair.second = false;
    }
    mouseState_.wheelDelta = 0;
}

} // namespace Funhouse