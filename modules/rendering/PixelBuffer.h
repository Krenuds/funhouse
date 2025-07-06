#pragma once

#include <GL/glew.h>
#include <vector>
#include <cstdint>

class PixelBuffer {
public:
    PixelBuffer(int width, int height);
    ~PixelBuffer();
    
    // Upload pixel data to GPU
    void Update(const std::vector<uint32_t>& pixels);
    void Update(const uint32_t* pixels, size_t count);
    
    // Clear the buffer
    void Clear(uint32_t color = 0xFF000000);
    
    // Render the pixel buffer to screen
    void Render();
    
    // Get buffer dimensions
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    
    // Direct pixel access (CPU side)
    void SetPixel(int x, int y, uint32_t color);
    uint32_t GetPixel(int x, int y) const;
    
private:
    void InitializeGL();
    void CleanupGL();
    
    int m_width;
    int m_height;
    
    // CPU-side pixel data
    std::vector<uint32_t> m_pixels;
    
    // OpenGL resources
    GLuint m_texture;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_shaderProgram;
    
    // Shader source
    static const char* s_vertexShaderSource;
    static const char* s_fragmentShaderSource;
};