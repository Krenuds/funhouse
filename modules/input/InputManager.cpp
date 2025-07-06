#include "InputManager.h"
#include <iostream>
#include <sstream>

namespace Funhouse {

InputManager::InputManager(InputSystem* inputSystem, ::World* world)
    : inputSystem_(inputSystem), world_(world) {
}

void InputManager::Initialize() {
    SetupDefaultBindings();
    SetupContextBindings();
}

void InputManager::SetupContextBindings() {
    // Create gameplay context
    auto gameplayContext = std::make_shared<InputContext>("gameplay");
    gameplayContext->SetPriority(10);
    
    // Material selection
    gameplayContext->BindKey(SDL_SCANCODE_1, [this]() {
        return std::make_unique<SelectMaterialCommand>(
            MaterialType::Air,
            [this](MaterialType mat) { 
                selectedMaterial_ = mat;
                std::cout << "Selected: Air" << std::endl;
            }
        );
    });
    
    gameplayContext->BindKey(SDL_SCANCODE_2, [this]() {
        return std::make_unique<SelectMaterialCommand>(
            MaterialType::Sand,
            [this](MaterialType mat) { 
                selectedMaterial_ = mat;
                std::cout << "Selected: Sand" << std::endl;
            }
        );
    });
    
    gameplayContext->BindKey(SDL_SCANCODE_3, [this]() {
        return std::make_unique<SelectMaterialCommand>(
            MaterialType::Water,
            [this](MaterialType mat) { 
                selectedMaterial_ = mat;
                std::cout << "Selected: Water" << std::endl;
            }
        );
    });
    
    gameplayContext->BindKey(SDL_SCANCODE_4, [this]() {
        return std::make_unique<SelectMaterialCommand>(
            MaterialType::Stone,
            [this](MaterialType mat) { 
                selectedMaterial_ = mat;
                std::cout << "Selected: Stone" << std::endl;
            }
        );
    });
    
    // Clear world
    gameplayContext->BindKey(SDL_SCANCODE_C, [this]() {
        return std::make_unique<ClearWorldCommand>(world_);
    });
    
    // Brush size controls
    gameplayContext->BindKey(SDL_SCANCODE_MINUS, [this]() {
        brushSize_ = std::max(1, brushSize_ - 2);
        std::cout << "Brush size: " << brushSize_ << std::endl;
        return nullptr;
    });
    
    gameplayContext->BindKey(SDL_SCANCODE_EQUALS, [this]() {
        brushSize_ = std::min(50, brushSize_ + 2);
        std::cout << "Brush size: " << brushSize_ << std::endl;
        return nullptr;
    });
    
    // Create recording context with higher priority
    auto recordingContext = std::make_shared<InputContext>("recording");
    recordingContext->SetPriority(20);
    
    // Recording controls
    recordingContext->BindKey(SDL_SCANCODE_R, [this]() {
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
    
    recordingContext->BindKey(SDL_SCANCODE_P, [this]() {
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
    
    // Add contexts to the system
    inputSystem_->AddContext(gameplayContext);
    inputSystem_->AddContext(recordingContext);
}

void InputManager::SetupDefaultBindings() {
    // Keep legacy bindings for backward compatibility
    // These will be overridden by context bindings
    
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

void InputManager::Update() {
    // Poll Twitch commands if enabled
    if (twitchAdapter_) {
        twitchAdapter_->Update();
    }
}

void InputManager::EnableTwitchIntegration(const TwitchIrcClient::Config& config) {
    // Disable existing integration if any
    DisableTwitchIntegration();
    
    // Create and start Twitch client
    twitchClient_ = std::make_unique<TwitchIrcClient>(config);
    if (twitchClient_->start()) {
        // Create adapter
        twitchAdapter_ = std::make_unique<TwitchCommandAdapter>(twitchClient_.get(), inputSystem_);
        
        // Setup Twitch command handlers
        SetupTwitchCommands();
        
        std::cout << "Twitch integration enabled for channel: #" << config.channel << std::endl;
    } else {
        std::cout << "Failed to start Twitch integration" << std::endl;
        twitchClient_.reset();
    }
}

void InputManager::DisableTwitchIntegration() {
    if (twitchClient_) {
        twitchClient_->stop();
        twitchClient_.reset();
        twitchAdapter_.reset();
        std::cout << "Twitch integration disabled" << std::endl;
    }
}

void InputManager::SetupTwitchCommands() {
    if (!twitchAdapter_) {
        return;
    }
    
    // Register material selection commands
    twitchAdapter_->RegisterCommandCallback("sand", 
        [this](const std::string& username, const std::string&, const std::string&) {
            selectedMaterial_ = MaterialType::Sand;
            std::cout << "[Twitch] " << username << " selected Sand" << std::endl;
        });
    
    twitchAdapter_->RegisterCommandCallback("water", 
        [this](const std::string& username, const std::string&, const std::string&) {
            selectedMaterial_ = MaterialType::Water;
            std::cout << "[Twitch] " << username << " selected Water" << std::endl;
        });
    
    twitchAdapter_->RegisterCommandCallback("stone", 
        [this](const std::string& username, const std::string&, const std::string&) {
            selectedMaterial_ = MaterialType::Stone;
            std::cout << "[Twitch] " << username << " selected Stone" << std::endl;
        });
    
    twitchAdapter_->RegisterCommandCallback("air", 
        [this](const std::string& username, const std::string&, const std::string&) {
            selectedMaterial_ = MaterialType::Air;
            std::cout << "[Twitch] " << username << " selected Air" << std::endl;
        });
    
    // Clear world command
    twitchAdapter_->RegisterCommandCallback("clear", 
        [this](const std::string& username, const std::string&, const std::string&) {
            auto command = std::make_unique<ClearWorldCommand>(world_);
            inputSystem_->QueueCommand(std::move(command));
            std::cout << "[Twitch] " << username << " cleared the world" << std::endl;
        });
    
    // Brush size commands
    twitchAdapter_->RegisterCommandCallback("brush", 
        [this](const std::string& username, const std::string&, const std::string& params) {
            std::istringstream iss(params);
            int size;
            if (iss >> size && size >= 1 && size <= 50) {
                brushSize_ = size;
                std::cout << "[Twitch] " << username << " set brush size to " << size << std::endl;
            }
        });
    
    // Spawn command (example of parameterized command)
    twitchAdapter_->RegisterCommandCallback("spawn", 
        [this](const std::string& username, const std::string&, const std::string& params) {
            std::istringstream iss(params);
            std::string material;
            int x, y;
            if (iss >> material >> x >> y) {
                // Convert material string to MaterialType
                MaterialType mat = MaterialType::Air;
                if (material == "sand") mat = MaterialType::Sand;
                else if (material == "water") mat = MaterialType::Water;
                else if (material == "stone") mat = MaterialType::Stone;
                
                // Convert to world coordinates (assuming same scaling as mouse)
                int worldX = x / 4;
                int worldY = y / 4;
                
                if (worldX >= 0 && worldX < world_->GetWidth() && 
                    worldY >= 0 && worldY < world_->GetHeight()) {
                    auto command = std::make_unique<PlaceMaterialCommand>(world_, worldX, worldY, mat);
                    inputSystem_->QueueCommand(std::move(command));
                    std::cout << "[Twitch] " << username << " spawned " << material 
                              << " at (" << worldX << ", " << worldY << ")" << std::endl;
                }
            }
        });
    
    // Help command
    twitchAdapter_->RegisterCommandCallback("help", 
        [](const std::string& username, const std::string&, const std::string&) {
            std::cout << "[Twitch] " << username << " requested help. Available commands: "
                      << "!sand, !water, !stone, !air, !clear, !brush [size], "
                      << "!spawn [material] [x] [y]" << std::endl;
        });
    
    // Set default handler to log unrecognized commands
    twitchAdapter_->SetDefaultHandler(
        [](const TwitchCommand& cmd) -> InputCommandPtr {
            std::cout << "[Twitch] Unknown command from " << cmd.username 
                      << ": !" << cmd.command;
            if (!cmd.parameters.empty()) {
                std::cout << " " << cmd.parameters;
            }
            std::cout << std::endl;
            return nullptr;  // Don't create a command for unrecognized inputs
        });
}

} // namespace Funhouse