#pragma once

#include <cstdint>

enum class MaterialType : uint8_t {
    Air = 0,
    Sand = 1,
    Water = 2,
    Stone = 3
};

struct MaterialProperties {
    bool isSolid;
    bool isLiquid;
    float density;
    uint32_t color;
};

const MaterialProperties MATERIAL_PROPERTIES[] = {
    {false, false, 0.0f,    0x000000FF},  // Air
    {true,  false, 2.0f,    0xC2B280FF},  // Sand
    {false, true,  1.0f,    0x0080FFFF},  // Water
    {true,  false, 10.0f,   0x808080FF}   // Stone
};