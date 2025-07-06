#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <memory>
#include <string>

class PixelBuffer;
class World;

class Application {
public:
    Application(const std::string& title, int width, int height);
    ~Application();

    bool Initialize();
    void Run();
    void Shutdown();

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    
    bool IsRunning() const { return m_running; }
    void Quit() { m_running = false; }

private:
    void ProcessEvents();
    void Update(float deltaTime);
    void Render();

    std::string m_title;
    int m_width;
    int m_height;
    bool m_running;
    bool m_initialized;

    SDL_Window* m_window;
    SDL_GLContext m_glContext;

    static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
    float m_accumulator;
    
    std::unique_ptr<PixelBuffer> m_pixelBuffer;
    std::unique_ptr<World> m_world;
};