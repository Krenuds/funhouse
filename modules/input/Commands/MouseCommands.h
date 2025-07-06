#pragma once

#include "../InputCommand.h"
#include "../../world/World.h"
#include "../../materials/Materials.h"

namespace Funhouse {

class PlaceMaterialCommand : public InputCommand {
public:
    PlaceMaterialCommand(::World* world, int x, int y, MaterialType material)
        : world_(world), x_(x), y_(y), material_(material) {}
    
    void Execute() override {
        if (world_) {
            world_->SetPixel(x_, y_, material_);
        }
    }
    
    std::string GetName() const override {
        return "PlaceMaterial";
    }
    
    std::unique_ptr<InputCommand> Clone() const override {
        return std::make_unique<PlaceMaterialCommand>(world_, x_, y_, material_);
    }
    
private:
    ::World* world_;
    int x_;
    int y_;
    MaterialType material_;
};

class RemoveMaterialCommand : public InputCommand {
public:
    RemoveMaterialCommand(::World* world, int x, int y)
        : world_(world), x_(x), y_(y) {}
    
    void Execute() override {
        if (world_) {
            world_->SetPixel(x_, y_, MaterialType::Air);
        }
    }
    
    std::string GetName() const override {
        return "RemoveMaterial";
    }
    
    std::unique_ptr<InputCommand> Clone() const override {
        return std::make_unique<RemoveMaterialCommand>(world_, x_, y_);
    }
    
private:
    ::World* world_;
    int x_;
    int y_;
};

class MouseDrawCommand : public InputCommand {
public:
    MouseDrawCommand(::World* world, int x, int y, int brushSize, MaterialType material, bool isErasing)
        : world_(world), x_(x), y_(y), brushSize_(brushSize), material_(material), isErasing_(isErasing) {}
    
    void Execute() override {
        if (!world_) return;
        
        int halfSize = brushSize_ / 2;
        for (int dy = -halfSize; dy <= halfSize; ++dy) {
            for (int dx = -halfSize; dx <= halfSize; ++dx) {
                if (dx * dx + dy * dy <= halfSize * halfSize) {
                    int px = x_ + dx;
                    int py = y_ + dy;
                    
                    if (px >= 0 && px < world_->GetWidth() && py >= 0 && py < world_->GetHeight()) {
                        world_->SetPixel(px, py, isErasing_ ? MaterialType::Air : material_);
                    }
                }
            }
        }
    }
    
    std::string GetName() const override {
        return isErasing_ ? "MouseErase" : "MouseDraw";
    }
    
    std::unique_ptr<InputCommand> Clone() const override {
        return std::make_unique<MouseDrawCommand>(world_, x_, y_, brushSize_, material_, isErasing_);
    }
    
private:
    ::World* world_;
    int x_;
    int y_;
    int brushSize_;
    MaterialType material_;
    bool isErasing_;
};

} // namespace Funhouse