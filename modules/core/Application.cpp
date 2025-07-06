#include "core/Application.h"
#include "rendering/PixelBuffer.h"
#include "world/World.h"
#include "materials/Materials.h"
#include "input/InputSystem.h"
#include "input/InputManager.h"
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ctime>

Application::Application(const std::string& title, int width, int height)
    : m_title(title)
    , m_width(width)
    , m_height(height)
    , m_running(false)
    , m_initialized(false)
    , m_window(nullptr)
    , m_glContext(nullptr)
    , m_accumulator(0.0f) {
}

Application::~Application() {
    Shutdown();
}

bool Application::Initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_window = SDL_CreateWindow(
        m_title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        m_width,
        m_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!m_window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }

    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_SetSwapInterval(1);

    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(glewError) << std::endl;
        return false;
    }

    glViewport(0, 0, m_width, m_height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    // Initialize random seed
    std::srand(std::time(nullptr));
    
    // Create pixel buffer (lower resolution for performance)
    int simWidth = m_width / 4;
    int simHeight = m_height / 4;
    m_pixelBuffer = std::make_unique<PixelBuffer>(simWidth, simHeight);
    
    // Create world
    m_world = std::make_unique<World>(simWidth, simHeight);
    
    // Create input system
    m_inputSystem = std::make_unique<Funhouse::InputSystem>();
    m_inputManager = std::make_unique<Funhouse::InputManager>(m_inputSystem.get(), m_world.get());
    m_inputManager->Initialize();
    
    // Print controls
    std::cout << "\n=== Funhouse Controls ===" << std::endl;
    std::cout << "Mouse: Left click to draw, Right click to erase" << std::endl;
    std::cout << "1-4: Select materials (Air, Sand, Water, Stone)" << std::endl;
    std::cout << "+/-: Increase/decrease brush size" << std::endl;
    std::cout << "C: Clear world" << std::endl;
    std::cout << "R: Toggle recording" << std::endl;
    std::cout << "P: Playback recording" << std::endl;
    std::cout << "ESC: Exit" << std::endl;
    std::cout << "========================\n" << std::endl;
    
    // Add some initial materials
    // Create ground
    for (int x = 0; x < simWidth; x++) {
        m_world->SetPixel(x, simHeight - 1, MaterialType::Stone);
        m_world->SetPixel(x, simHeight - 2, MaterialType::Stone);
    }
    
    // Add some sand
    for (int x = simWidth/4; x < simWidth/2; x++) {
        for (int y = 0; y < 20; y++) {
            m_world->SetPixel(x, y + 10, MaterialType::Sand);
        }
    }
    
    // Add some water
    for (int x = simWidth/2; x < 3*simWidth/4; x++) {
        for (int y = 0; y < 15; y++) {
            m_world->SetPixel(x, y + 20, MaterialType::Water);
        }
    }

    m_initialized = true;
    m_running = true;
    return true;
}

void Application::Run() {
    if (!m_initialized) {
        std::cerr << "Application not initialized!" << std::endl;
        return;
    }

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto previousTime = currentTime;

    while (m_running) {
        previousTime = currentTime;
        currentTime = std::chrono::high_resolution_clock::now();
        
        float frameTime = std::chrono::duration<float>(currentTime - previousTime).count();
        
        if (frameTime > 0.25f) {
            frameTime = 0.25f;
        }

        m_accumulator += frameTime;

        ProcessEvents();

        while (m_accumulator >= FIXED_TIMESTEP) {
            Update(FIXED_TIMESTEP);
            m_accumulator -= FIXED_TIMESTEP;
        }

        Render();
        
        SDL_GL_SwapWindow(m_window);
    }
}

void Application::Shutdown() {
    if (m_glContext) {
        SDL_GL_DeleteContext(m_glContext);
        m_glContext = nullptr;
    }

    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }

    SDL_Quit();
    m_initialized = false;
}

void Application::ProcessEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Let input system process the event first
        if (m_inputSystem) {
            m_inputSystem->ProcessEvent(event, m_world.get());
        }
        
        // Handle application-level events
        switch (event.type) {
            case SDL_QUIT:
                m_running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    m_running = false;
                }
                break;
        }
    }
    
    // Handle continuous mouse input
    if (m_inputManager) {
        const auto& mouseState = m_inputSystem->GetMouseState();
        m_inputManager->HandleMouseDraw(mouseState.x, mouseState.y, 
                                        mouseState.leftPressed, 
                                        mouseState.rightPressed);
    }
}

void Application::Update(float deltaTime) {
    (void)deltaTime;
    
    // Update input system
    if (m_inputSystem) {
        m_inputSystem->Update();
        m_inputSystem->ExecuteCommands();
    }
    
    // Update the world simulation
    if (m_world) {
        m_world->Update();
    }
}

void Application::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (m_pixelBuffer && m_world) {
        // Convert world materials to colors
        std::vector<uint32_t> pixels(m_pixelBuffer->GetWidth() * m_pixelBuffer->GetHeight());
        
        for (int y = 0; y < m_world->GetHeight(); y++) {
            for (int x = 0; x < m_world->GetWidth(); x++) {
                MaterialType mat = m_world->GetPixel(x, y);
                uint32_t color = 0xFF000000; // Default black with full alpha
                
                switch (mat) {
                    case MaterialType::Air:
                        color = 0xFF1A1A1A; // Dark gray background
                        break;
                    case MaterialType::Sand:
                        color = 0xFFE3B778; // Sandy yellow (ABGR format)
                        break;
                    case MaterialType::Water:
                        color = 0xFFB87843; // Blue water (ABGR format)
                        break;
                    case MaterialType::Stone:
                        color = 0xFF808080; // Gray stone
                        break;
                }
                
                pixels[y * m_pixelBuffer->GetWidth() + x] = color;
            }
        }
        
        m_pixelBuffer->Update(pixels);
        m_pixelBuffer->Render();
    }
}