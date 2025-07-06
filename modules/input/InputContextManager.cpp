#include "InputContextManager.h"

namespace Funhouse {

void InputContextManager::AddContext(std::shared_ptr<InputContext> context) {
    contexts_.push_back(context);
    needsSort_ = true;
}

void InputContextManager::RemoveContext(const std::string& name) {
    contexts_.erase(
        std::remove_if(contexts_.begin(), contexts_.end(),
            [&name](const auto& ctx) { return ctx->GetName() == name; }),
        contexts_.end()
    );
}

std::shared_ptr<InputContext> InputContextManager::GetContext(const std::string& name) {
    auto it = std::find_if(contexts_.begin(), contexts_.end(),
        [&name](const auto& ctx) { return ctx->GetName() == name; });
    
    return (it != contexts_.end()) ? *it : nullptr;
}

void InputContextManager::ActivateContext(const std::string& name) {
    if (auto ctx = GetContext(name)) {
        ctx->SetActive(true);
    }
}

void InputContextManager::DeactivateContext(const std::string& name) {
    if (auto ctx = GetContext(name)) {
        ctx->SetActive(false);
    }
}

void InputContextManager::ToggleContext(const std::string& name) {
    if (auto ctx = GetContext(name)) {
        ctx->SetActive(!ctx->IsActive());
    }
}

InputCommandPtr InputContextManager::ProcessKeyDown(SDL_Scancode key, SDL_Keymod modifiers) {
    SortContextsByPriority();
    
    for (const auto& context : contexts_) {
        if (!context->IsActive()) continue;
        
        if (auto command = context->ProcessKeyDown(key, modifiers)) {
            return command;
        }
    }
    
    return nullptr;
}

InputCommandPtr InputContextManager::ProcessKeyUp(SDL_Scancode key, SDL_Keymod modifiers) {
    SortContextsByPriority();
    
    for (const auto& context : contexts_) {
        if (!context->IsActive()) continue;
        
        if (auto command = context->ProcessKeyUp(key, modifiers)) {
            return command;
        }
    }
    
    return nullptr;
}

InputCommandPtr InputContextManager::ProcessMouseButtonDown(Uint8 button, SDL_Keymod modifiers) {
    SortContextsByPriority();
    
    for (const auto& context : contexts_) {
        if (!context->IsActive()) continue;
        
        if (auto command = context->ProcessMouseButtonDown(button, modifiers)) {
            return command;
        }
    }
    
    return nullptr;
}

InputCommandPtr InputContextManager::ProcessMouseButtonUp(Uint8 button, SDL_Keymod modifiers) {
    SortContextsByPriority();
    
    for (const auto& context : contexts_) {
        if (!context->IsActive()) continue;
        
        if (auto command = context->ProcessMouseButtonUp(button, modifiers)) {
            return command;
        }
    }
    
    return nullptr;
}

InputCommandPtr InputContextManager::ProcessMouseWheel(int direction) {
    SortContextsByPriority();
    
    for (const auto& context : contexts_) {
        if (!context->IsActive()) continue;
        
        if (auto command = context->ProcessMouseWheel(direction)) {
            return command;
        }
    }
    
    return nullptr;
}

InputCommandPtr InputContextManager::ProcessMouseMotion() {
    SortContextsByPriority();
    
    for (const auto& context : contexts_) {
        if (!context->IsActive()) continue;
        
        if (auto command = context->ProcessMouseMotion()) {
            return command;
        }
    }
    
    return nullptr;
}

void InputContextManager::ClearContexts() {
    contexts_.clear();
}

std::vector<std::shared_ptr<InputContext>> InputContextManager::GetActiveContexts() const {
    std::vector<std::shared_ptr<InputContext>> active;
    
    for (const auto& ctx : contexts_) {
        if (ctx->IsActive()) {
            active.push_back(ctx);
        }
    }
    
    // Sort by priority (higher priority first)
    std::sort(active.begin(), active.end(),
        [](const auto& a, const auto& b) {
            return a->GetPriority() > b->GetPriority();
        });
    
    return active;
}

void InputContextManager::SortContextsByPriority() {
    if (!needsSort_) return;
    
    std::sort(contexts_.begin(), contexts_.end(),
        [](const auto& a, const auto& b) {
            return a->GetPriority() > b->GetPriority();
        });
    
    needsSort_ = false;
}

} // namespace Funhouse