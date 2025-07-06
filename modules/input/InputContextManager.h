#pragma once

#include "InputContext.h"
#include <vector>
#include <memory>
#include <algorithm>

namespace Funhouse {

class InputContextManager {
public:
    InputContextManager() = default;
    ~InputContextManager() = default;
    
    // Context management
    void AddContext(std::shared_ptr<InputContext> context);
    void RemoveContext(const std::string& name);
    std::shared_ptr<InputContext> GetContext(const std::string& name);
    
    // Activate/deactivate contexts
    void ActivateContext(const std::string& name);
    void DeactivateContext(const std::string& name);
    void ToggleContext(const std::string& name);
    
    // Process events through active contexts
    InputCommandPtr ProcessKeyDown(SDL_Scancode key, SDL_Keymod modifiers);
    InputCommandPtr ProcessKeyUp(SDL_Scancode key, SDL_Keymod modifiers);
    InputCommandPtr ProcessMouseButtonDown(Uint8 button, SDL_Keymod modifiers);
    InputCommandPtr ProcessMouseButtonUp(Uint8 button, SDL_Keymod modifiers);
    InputCommandPtr ProcessMouseWheel(int direction);
    InputCommandPtr ProcessMouseMotion();
    
    // Clear all contexts
    void ClearContexts();
    
    // Get all active contexts (sorted by priority)
    std::vector<std::shared_ptr<InputContext>> GetActiveContexts() const;
    
private:
    void SortContextsByPriority();
    
    std::vector<std::shared_ptr<InputContext>> contexts_;
    bool needsSort_ = false;
};

} // namespace Funhouse