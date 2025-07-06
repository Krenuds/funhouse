#include "InputManager.h"
#include <iostream>

namespace Funhouse {

InputManager::InputManager(InputSystem* inputSystem, ::World* world)
    : inputSystem_(inputSystem), world_(world) {
}

void InputManager::Initialize() {
    SetupDefaultBindings();
}

void InputManager::SetupDefaultBindings() {
    // Material selection keys (1-4)
    inputSystem_->RegisterKeyCommandFactory(SDL_SCANCODE_1, 
        [this](const SDL_Event&) -> InputCommandPtr {
            return std::make_unique<SelectMaterialCommand>(
                MaterialType::Air,
                [this](MaterialType mat) { 
                    selectedMaterial_ = mat;
                    std::cout << "Selected: Air" << std::endl;
                }
            );
        });
    
    inputSystem_->RegisterKeyCommandFactory(SDL_SCANCODE_2,
        [this](const SDL_Event&) -> InputCommandPtr {
            return std::make_unique<SelectMaterialCommand>(
                MaterialType::Sand,
                [this](MaterialType mat) { 
                    selectedMaterial_ = mat;
                    std::cout << "Selected: Sand" << std::endl;
                }
            );
        });
    
    inputSystem_->RegisterKeyCommandFactory(SDL_SCANCODE_3,
        [this](const SDL_Event&) -> InputCommandPtr {
            return std::make_unique<SelectMaterialCommand>(
                MaterialType::Water,
                [this](MaterialType mat) { 
                    selectedMaterial_ = mat;
                    std::cout << "Selected: Water" << std::endl;
                }
            );
        });
    
    inputSystem_->RegisterKeyCommandFactory(SDL_SCANCODE_4,
        [this](const SDL_Event&) -> InputCommandPtr {
            return std::make_unique<SelectMaterialCommand>(
                MaterialType::Stone,
                [this](MaterialType mat) { 
                    selectedMaterial_ = mat;
                    std::cout << "Selected: Stone" << std::endl;
                }
            );
        });
    
    // Clear world
    inputSystem_->RegisterKeyCommandFactory(SDL_SCANCODE_C,
        [this](const SDL_Event&) -> InputCommandPtr {
            return std::make_unique<ClearWorldCommand>(world_);
        });
    
    // Recording controls
    inputSystem_->RegisterKeyCommandFactory(SDL_SCANCODE_R,
        [this](const SDL_Event&) -> InputCommandPtr {
            return std::make_unique<ToggleRecordingCommand>(
                [this]() {
                    if (inputSystem_->IsRecording()) {
                        inputSystem_->StopRecording();
                        std::cout << "Recording stopped. " 
                                  << inputSystem_->GetRecordedCommands().size() 
                                  << " commands recorded." << std::endl;
                    } else {
                        inputSystem_->StartRecording();
                        std::cout << "Recording started..." << std::endl;
                    }
                }
            );
        });
    
    inputSystem_->RegisterKeyCommandFactory(SDL_SCANCODE_P,
        [this](const SDL_Event&) -> InputCommandPtr {
            return std::make_unique<TogglePlaybackCommand>(
                [this]() {
                    if (inputSystem_->IsPlayingBack()) {
                        inputSystem_->StopPlayback();
                        std::cout << "Playback stopped." << std::endl;
                    } else if (!inputSystem_->GetRecordedCommands().empty()) {
                        inputSystem_->StartPlayback(inputSystem_->GetRecordedCommands());
                        std::cout << "Playback started..." << std::endl;
                    } else {
                        std::cout << "No recorded commands to playback." << std::endl;
                    }
                }
            );
        });
    
    // Brush size controls
    inputSystem_->RegisterKeyCommandFactory(SDL_SCANCODE_MINUS,
        [this](const SDL_Event&) -> InputCommandPtr {
            brushSize_ = std::max(1, brushSize_ - 2);
            std::cout << "Brush size: " << brushSize_ << std::endl;
            return nullptr;
        });
    
    inputSystem_->RegisterKeyCommandFactory(SDL_SCANCODE_EQUALS,
        [this](const SDL_Event&) -> InputCommandPtr {
            brushSize_ = std::min(50, brushSize_ + 2);
            std::cout << "Brush size: " << brushSize_ << std::endl;
            return nullptr;
        });
}

void InputManager::HandleMouseDraw(int x, int y, bool leftButton, bool rightButton) {
    if (!leftButton && !rightButton) {
        lastMouseX_ = -1;
        lastMouseY_ = -1;
        return;
    }
    
    // Convert screen coordinates to world coordinates
    // Assuming the pixel buffer is 1/4 the window size
    int worldX = x / 4;
    int worldY = y / 4;
    
    if (worldX < 0 || worldX >= world_->GetWidth() || 
        worldY < 0 || worldY >= world_->GetHeight()) {
        return;
    }
    
    // Draw a line from last position to current position for smooth drawing
    if (lastMouseX_ != -1 && lastMouseY_ != -1) {
        int dx = std::abs(worldX - lastMouseX_);
        int dy = std::abs(worldY - lastMouseY_);
        int sx = lastMouseX_ < worldX ? 1 : -1;
        int sy = lastMouseY_ < worldY ? 1 : -1;
        int err = dx - dy;
        
        int x0 = lastMouseX_;
        int y0 = lastMouseY_;
        
        while (true) {
            auto command = std::make_unique<MouseDrawCommand>(
                world_, x0, y0, brushSize_, selectedMaterial_, rightButton
            );
            inputSystem_->QueueCommand(std::move(command));
            
            if (x0 == worldX && y0 == worldY) break;
            
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }
        }
    } else {
        // Just draw at current position
        auto command = std::make_unique<MouseDrawCommand>(
            world_, worldX, worldY, brushSize_, selectedMaterial_, rightButton
        );
        inputSystem_->QueueCommand(std::move(command));
    }
    
    lastMouseX_ = worldX;
    lastMouseY_ = worldY;
}

} // namespace Funhouse