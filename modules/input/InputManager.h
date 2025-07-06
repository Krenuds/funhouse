#pragma once

#include "InputSystem.h"
#include "Commands/MouseCommands.h"
#include "Commands/KeyboardCommands.h"
#include "../materials/Materials.h"

namespace Funhouse {

class InputManager {
public:
    InputManager(InputSystem* inputSystem, ::World* world);
    
    void Initialize();
    
    MaterialType GetSelectedMaterial() const { return selectedMaterial_; }
    void SetSelectedMaterial(MaterialType material) { selectedMaterial_ = material; }
    
    int GetBrushSize() const { return brushSize_; }
    void SetBrushSize(int size) { brushSize_ = size; }
    
    void HandleMouseDraw(int x, int y, bool leftButton, bool rightButton);
    
private:
    void SetupDefaultBindings();
    
    InputSystem* inputSystem_;
    ::World* world_;
    MaterialType selectedMaterial_ = MaterialType::Sand;
    int brushSize_ = 5;
    
    int lastMouseX_ = -1;
    int lastMouseY_ = -1;
};

} // namespace Funhouse