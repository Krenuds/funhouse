#pragma once

#include "InputCommand.h"
#include <SDL2/SDL.h>
#include <queue>
#include <vector>
#include <functional>
#include <unordered_map>

class World;

namespace Funhouse {

struct MouseState {
    int x = 0;
    int y = 0;
    bool leftPressed = false;
    bool rightPressed = false;
    bool middlePressed = false;
    int wheelDelta = 0;
};

struct KeyboardState {
    std::unordered_map<SDL_Scancode, bool> keysPressed;
    std::unordered_map<SDL_Scancode, bool> keysJustPressed;
    std::unordered_map<SDL_Scancode, bool> keysJustReleased;
};

class InputSystem {
public:
    using CommandFactory = std::function<InputCommandPtr(const SDL_Event&)>;
    
    InputSystem();
    ~InputSystem();
    
    void ProcessEvent(const SDL_Event& event, ::World* world);
    
    void Update();
    
    void ExecuteCommands();
    
    void RegisterCommandFactory(Uint32 eventType, CommandFactory factory);
    
    void RegisterKeyCommandFactory(SDL_Scancode key, CommandFactory factory);
    
    void RegisterMouseCommandFactory(Uint8 button, CommandFactory factory);
    
    const MouseState& GetMouseState() const { return mouseState_; }
    const KeyboardState& GetKeyboardState() const { return keyboardState_; }
    
    void QueueCommand(InputCommandPtr command);
    
    void StartRecording();
    void StopRecording();
    bool IsRecording() const { return isRecording_; }
    const std::vector<InputCommandPtr>& GetRecordedCommands() const { return recordedCommands_; }
    
    void StartPlayback(const std::vector<InputCommandPtr>& commands);
    void StopPlayback();
    bool IsPlayingBack() const { return isPlayingBack_; }
    
    void SetWorld(::World* world) { world_ = world; }
    ::World* GetWorld() { return world_; }
    
private:
    void UpdateMouseState(const SDL_Event& event);
    void UpdateKeyboardState(const SDL_Event& event);
    void ClearKeyboardTransitions();
    
    MouseState mouseState_;
    KeyboardState keyboardState_;
    
    std::queue<InputCommandPtr> commandQueue_;
    std::unordered_map<Uint32, std::vector<CommandFactory>> eventFactories_;
    std::unordered_map<SDL_Scancode, std::vector<CommandFactory>> keyFactories_;
    std::unordered_map<Uint8, std::vector<CommandFactory>> mouseButtonFactories_;
    
    bool isRecording_ = false;
    std::vector<InputCommandPtr> recordedCommands_;
    
    bool isPlayingBack_ = false;
    std::vector<InputCommandPtr> playbackCommands_;
    size_t playbackIndex_ = 0;
    std::chrono::steady_clock::time_point playbackStartTime_;
    
    ::World* world_ = nullptr;
};

} // namespace Funhouse