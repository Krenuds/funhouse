#pragma once

#include "InputCommand.h"
#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

namespace Funhouse {

class InputContext {
public:
    using CommandFactory = std::function<InputCommandPtr()>;
    using KeyBinding = std::pair<SDL_Scancode, SDL_Keymod>;
    using MouseBinding = std::pair<Uint8, SDL_Keymod>;
    
    InputContext(const std::string& name);
    ~InputContext() = default;
    
    // Binding management
    void BindKey(SDL_Scancode key, CommandFactory factory, SDL_Keymod modifiers = KMOD_NONE);
    void BindMouseButton(Uint8 button, CommandFactory factory, SDL_Keymod modifiers = KMOD_NONE);
    void BindMouseWheel(CommandFactory scrollUp, CommandFactory scrollDown);
    void BindMouseMotion(CommandFactory factory);
    
    // Unbinding
    void UnbindKey(SDL_Scancode key, SDL_Keymod modifiers = KMOD_NONE);
    void UnbindMouseButton(Uint8 button, SDL_Keymod modifiers = KMOD_NONE);
    void UnbindMouseWheel();
    void UnbindMouseMotion();
    
    // Clear all bindings
    void ClearBindings();
    
    // Process events
    InputCommandPtr ProcessKeyDown(SDL_Scancode key, SDL_Keymod modifiers) const;
    InputCommandPtr ProcessKeyUp(SDL_Scancode key, SDL_Keymod modifiers) const;
    InputCommandPtr ProcessMouseButtonDown(Uint8 button, SDL_Keymod modifiers) const;
    InputCommandPtr ProcessMouseButtonUp(Uint8 button, SDL_Keymod modifiers) const;
    InputCommandPtr ProcessMouseWheel(int direction) const;
    InputCommandPtr ProcessMouseMotion() const;
    
    // Context info
    const std::string& GetName() const { return name_; }
    bool IsActive() const { return active_; }
    void SetActive(bool active) { active_ = active; }
    
    // Priority for context stacking
    int GetPriority() const { return priority_; }
    void SetPriority(int priority) { priority_ = priority; }
    
private:
    struct KeyHash {
        std::size_t operator()(const KeyBinding& k) const {
            return std::hash<int>()(k.first) ^ (std::hash<int>()(k.second) << 1);
        }
    };
    
    struct MouseHash {
        std::size_t operator()(const MouseBinding& m) const {
            return std::hash<int>()(m.first) ^ (std::hash<int>()(m.second) << 1);
        }
    };
    
    std::string name_;
    bool active_ = true;
    int priority_ = 0;
    
    std::unordered_map<KeyBinding, CommandFactory, KeyHash> keyDownBindings_;
    std::unordered_map<KeyBinding, CommandFactory, KeyHash> keyUpBindings_;
    std::unordered_map<MouseBinding, CommandFactory, MouseHash> mouseDownBindings_;
    std::unordered_map<MouseBinding, CommandFactory, MouseHash> mouseUpBindings_;
    CommandFactory mouseWheelUpBinding_;
    CommandFactory mouseWheelDownBinding_;
    CommandFactory mouseMotionBinding_;
};

} // namespace Funhouse