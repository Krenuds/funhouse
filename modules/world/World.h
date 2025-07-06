#pragma once

#include "../materials/Materials.h"
#include <vector>
#include <cstdlib>

class World {
public:
    World(int width, int height);
    ~World() = default;

    void Update();
    void SetPixel(int x, int y, MaterialType material);
    MaterialType GetPixel(int x, int y) const;
    
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    void Clear();
    void Print() const;

private:
    bool InBounds(int x, int y) const;
    void UpdatePixel(int x, int y);
    void SwapPixels(int x1, int y1, int x2, int y2);

    int m_width;
    int m_height;
    std::vector<MaterialType> m_pixels;
    bool m_updateDirection;
};