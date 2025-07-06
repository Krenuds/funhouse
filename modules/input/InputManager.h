#pragma once

#include "InputSystem.h"
#include "Commands/MouseCommands.h"
#include "Commands/KeyboardCommands.h"
#include "Commands/TwitchCommandAdapter.h"
#include "../materials/Materials.h"
#include "../twitch/TwitchIrcClient.h"
#include <memory>

namespace Funhouse {

class InputManager {
public:
    InputManager(InputSystem* inputSystem, ::World* world);
    
    void Initialize();
    
    // Update method to poll Twitch commands
    void Update();
    
    MaterialType GetSelectedMaterial() const { return selectedMaterial_; }
    void SetSelectedMaterial(MaterialType material) { selectedMaterial_ = material; }
    
    int GetBrushSize() const { return brushSize_; }
    void SetBrushSize(int size) { brushSize_ = size; }
    
    void HandleMouseDraw(int x, int y, bool leftButton, bool rightButton);
    
    // Twitch integration
    void EnableTwitchIntegration(const TwitchIrcClient::Config& config = TwitchIrcClient::Config());
    void DisableTwitchIntegration();
    bool IsTwitchEnabled() const { return twitchClient_ != nullptr; }
    TwitchCommandAdapter* GetTwitchAdapter() { return twitchAdapter_.get(); }
    
private:
    void SetupDefaultBindings();
    void SetupContextBindings();
    void SetupTwitchCommands();
    
    InputSystem* inputSystem_;
    ::World* world_;
    MaterialType selectedMaterial_ = MaterialType::Sand;
    int brushSize_ = 5;
    
    int lastMouseX_ = -1;
    int lastMouseY_ = -1;
    
    // Twitch integration
    std::unique_ptr<TwitchIrcClient> twitchClient_;
    std::unique_ptr<TwitchCommandAdapter> twitchAdapter_;
};

} // namespace Funhouse