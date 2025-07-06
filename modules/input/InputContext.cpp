#include "InputContext.h"

namespace Funhouse {

InputContext::InputContext(const std::string& name)
    : name_(name) {
}

void InputContext::BindKey(SDL_Scancode key, CommandFactory factory, SDL_Keymod modifiers) {
    keyDownBindings_[{key, modifiers}] = factory;
}

void InputContext::BindMouseButton(Uint8 button, CommandFactory factory, SDL_Keymod modifiers) {
    mouseDownBindings_[{button, modifiers}] = factory;
}

void InputContext::BindMouseWheel(CommandFactory scrollUp, CommandFactory scrollDown) {
    mouseWheelUpBinding_ = scrollUp;
    mouseWheelDownBinding_ = scrollDown;
}

void InputContext::BindMouseMotion(CommandFactory factory) {
    mouseMotionBinding_ = factory;
}

void InputContext::UnbindKey(SDL_Scancode key, SDL_Keymod modifiers) {
    keyDownBindings_.erase({key, modifiers});
    keyUpBindings_.erase({key, modifiers});
}

void InputContext::UnbindMouseButton(Uint8 button, SDL_Keymod modifiers) {
    mouseDownBindings_.erase({button, modifiers});
    mouseUpBindings_.erase({button, modifiers});
}

void InputContext::UnbindMouseWheel() {
    mouseWheelUpBinding_ = nullptr;
    mouseWheelDownBinding_ = nullptr;
}

void InputContext::UnbindMouseMotion() {
    mouseMotionBinding_ = nullptr;
}

void InputContext::ClearBindings() {
    keyDownBindings_.clear();
    keyUpBindings_.clear();
    mouseDownBindings_.clear();
    mouseUpBindings_.clear();
    mouseWheelUpBinding_ = nullptr;
    mouseWheelDownBinding_ = nullptr;
    mouseMotionBinding_ = nullptr;
}

InputCommandPtr InputContext::ProcessKeyDown(SDL_Scancode key, SDL_Keymod modifiers) const {
    if (!active_) return nullptr;
    
    // First try exact modifier match
    auto it = keyDownBindings_.find({key, modifiers});
    if (it != keyDownBindings_.end() && it->second) {
        return it->second();
    }
    
    // Then try without modifiers
    it = keyDownBindings_.find({key, KMOD_NONE});
    if (it != keyDownBindings_.end() && it->second) {
        return it->second();
    }
    
    return nullptr;
}

InputCommandPtr InputContext::ProcessKeyUp(SDL_Scancode key, SDL_Keymod modifiers) const {
    if (!active_) return nullptr;
    
    auto it = keyUpBindings_.find({key, modifiers});
    if (it != keyUpBindings_.end() && it->second) {
        return it->second();
    }
    
    it = keyUpBindings_.find({key, KMOD_NONE});
    if (it != keyUpBindings_.end() && it->second) {
        return it->second();
    }
    
    return nullptr;
}

InputCommandPtr InputContext::ProcessMouseButtonDown(Uint8 button, SDL_Keymod modifiers) const {
    if (!active_) return nullptr;
    
    auto it = mouseDownBindings_.find({button, modifiers});
    if (it != mouseDownBindings_.end() && it->second) {
        return it->second();
    }
    
    it = mouseDownBindings_.find({button, KMOD_NONE});
    if (it != mouseDownBindings_.end() && it->second) {
        return it->second();
    }
    
    return nullptr;
}

InputCommandPtr InputContext::ProcessMouseButtonUp(Uint8 button, SDL_Keymod modifiers) const {
    if (!active_) return nullptr;
    
    auto it = mouseUpBindings_.find({button, modifiers});
    if (it != mouseUpBindings_.end() && it->second) {
        return it->second();
    }
    
    it = mouseUpBindings_.find({button, KMOD_NONE});
    if (it != mouseUpBindings_.end() && it->second) {
        return it->second();
    }
    
    return nullptr;
}

InputCommandPtr InputContext::ProcessMouseWheel(int direction) const {
    if (!active_) return nullptr;
    
    if (direction > 0 && mouseWheelUpBinding_) {
        return mouseWheelUpBinding_();
    } else if (direction < 0 && mouseWheelDownBinding_) {
        return mouseWheelDownBinding_();
    }
    
    return nullptr;
}

InputCommandPtr InputContext::ProcessMouseMotion() const {
    if (!active_ || !mouseMotionBinding_) return nullptr;
    
    return mouseMotionBinding_();
}

} // namespace Funhouse