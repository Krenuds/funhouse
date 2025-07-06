#include "World.h"
#include <iostream>
#include <algorithm>

World::World(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_pixels(width * height, MaterialType::Air)
    , m_updateDirection(false) {
}

void World::Update() {
    m_updateDirection = !m_updateDirection;
    
    for (int y = m_height - 2; y >= 0; y--) {
        if (m_updateDirection) {
            for (int x = 0; x < m_width; x++) {
                UpdatePixel(x, y);
            }
        } else {
            for (int x = m_width - 1; x >= 0; x--) {
                UpdatePixel(x, y);
            }
        }
    }
}

void World::SetPixel(int x, int y, MaterialType material) {
    if (InBounds(x, y)) {
        m_pixels[y * m_width + x] = material;
    }
}

MaterialType World::GetPixel(int x, int y) const {
    if (InBounds(x, y)) {
        return m_pixels[y * m_width + x];
    }
    return MaterialType::Stone;
}

void World::Clear() {
    std::fill(m_pixels.begin(), m_pixels.end(), MaterialType::Air);
}

void World::Print() const {
    std::cout << "\033[2J\033[H";
    
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            MaterialType mat = GetPixel(x, y);
            switch (mat) {
                case MaterialType::Air:
                    std::cout << " ";
                    break;
                case MaterialType::Sand:
                    std::cout << ".";
                    break;
                case MaterialType::Water:
                    std::cout << "~";
                    break;
                case MaterialType::Stone:
                    std::cout << "#";
                    break;
            }
        }
        std::cout << "\n";
    }
    std::cout << std::flush;
}

bool World::InBounds(int x, int y) const {
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

void World::UpdatePixel(int x, int y) {
    MaterialType current = GetPixel(x, y);
    
    if (current == MaterialType::Air || current == MaterialType::Stone) {
        return;
    }
    
    const MaterialProperties& props = MATERIAL_PROPERTIES[static_cast<int>(current)];
    
    if (current == MaterialType::Sand) {
        MaterialType below = GetPixel(x, y + 1);
        
        if (below == MaterialType::Air || 
            (below == MaterialType::Water && MATERIAL_PROPERTIES[static_cast<int>(below)].density < props.density)) {
            SwapPixels(x, y, x, y + 1);
            return;
        }
        
        int dir = (rand() % 2) * 2 - 1;
        MaterialType diag1 = GetPixel(x + dir, y + 1);
        if (diag1 == MaterialType::Air || 
            (diag1 == MaterialType::Water && MATERIAL_PROPERTIES[static_cast<int>(diag1)].density < props.density)) {
            SwapPixels(x, y, x + dir, y + 1);
            return;
        }
        
        MaterialType diag2 = GetPixel(x - dir, y + 1);
        if (diag2 == MaterialType::Air || 
            (diag2 == MaterialType::Water && MATERIAL_PROPERTIES[static_cast<int>(diag2)].density < props.density)) {
            SwapPixels(x, y, x - dir, y + 1);
        }
    }
    else if (current == MaterialType::Water) {
        MaterialType below = GetPixel(x, y + 1);
        
        if (below == MaterialType::Air) {
            SwapPixels(x, y, x, y + 1);
            return;
        }
        
        int dir = (rand() % 2) * 2 - 1;
        
        MaterialType diag1 = GetPixel(x + dir, y + 1);
        if (diag1 == MaterialType::Air) {
            SwapPixels(x, y, x + dir, y + 1);
            return;
        }
        
        MaterialType diag2 = GetPixel(x - dir, y + 1);
        if (diag2 == MaterialType::Air) {
            SwapPixels(x, y, x - dir, y + 1);
            return;
        }
        
        MaterialType side1 = GetPixel(x + dir, y);
        if (side1 == MaterialType::Air) {
            SwapPixels(x, y, x + dir, y);
            return;
        }
        
        MaterialType side2 = GetPixel(x - dir, y);
        if (side2 == MaterialType::Air) {
            SwapPixels(x, y, x - dir, y);
        }
    }
}

void World::SwapPixels(int x1, int y1, int x2, int y2) {
    if (InBounds(x1, y1) && InBounds(x2, y2)) {
        std::swap(m_pixels[y1 * m_width + x1], m_pixels[y2 * m_width + x2]);
    }
}