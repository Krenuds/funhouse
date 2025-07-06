/**
 * Example demonstrating Twitch integration with the Funhouse input system
 * 
 * This example shows how to:
 * 1. Enable Twitch integration in InputManager
 * 2. Register custom Twitch commands
 * 3. Process Twitch commands through the InputCommand system
 */

#include "../modules/world/World.h"
#include "../modules/input/InputSystem.h"
#include "../modules/input/InputManager.h"
#include "../modules/twitch/TwitchIrcClient.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace Funhouse;

int main() {
    std::cout << "=== Funhouse Twitch Integration Example ===" << std::endl;
    std::cout << "This example demonstrates Twitch chat commands being processed" << std::endl;
    std::cout << "through the InputCommand system." << std::endl << std::endl;
    
    // Create the world and input system
    auto world = std::make_unique<World>(320, 240);
    auto inputSystem = std::make_unique<InputSystem>();
    inputSystem->SetWorld(world.get());
    
    // Create and initialize InputManager
    auto inputManager = std::make_unique<InputManager>(inputSystem.get(), world.get());
    inputManager->Initialize();
    
    // Configure Twitch integration
    TwitchIrcClient::Config twitchConfig;
    twitchConfig.channel = "travisrashguard";  // Change to your channel
    twitchConfig.command_prefix = "!";
    twitchConfig.auto_reconnect = true;
    
    // Enable Twitch integration
    std::cout << "Enabling Twitch integration for channel: #" << twitchConfig.channel << std::endl;
    inputManager->EnableTwitchIntegration(twitchConfig);
    
    // Add some custom commands
    auto adapter = inputManager->GetTwitchAdapter();
    if (adapter) {
        // Custom gravity command
        adapter->RegisterCommandCallback("gravity",
            [](const std::string& username, const std::string&, const std::string& params) {
                std::cout << "[Custom] " << username << " wants to set gravity to: " << params << std::endl;
                // In a real implementation, you would parse params and update physics
            });
        
        // Custom color command
        adapter->RegisterCommandCallback("color",
            [](const std::string& username, const std::string&, const std::string& params) {
                std::cout << "[Custom] " << username << " wants to change color to: " << params << std::endl;
                // In a real implementation, you would update rendering colors
            });
        
        // Fun emoji rain command
        adapter->RegisterCommandCallback("rain",
            [&world](const std::string& username, const std::string&, const std::string& params) {
                std::cout << "[Custom] " << username << " triggered rain with: " << params << std::endl;
                // Spawn particles at top of world
                for (int x = 0; x < world->GetWidth(); x += 10) {
                    world->SetPixel(x, 0, MaterialType::Water);
                }
            });
    }
    
    std::cout << std::endl;
    std::cout << "Available commands in Twitch chat:" << std::endl;
    std::cout << "  !sand, !water, !stone, !air - Select materials" << std::endl;
    std::cout << "  !clear - Clear the world" << std::endl;
    std::cout << "  !brush [1-50] - Set brush size" << std::endl;
    std::cout << "  !spawn [material] [x] [y] - Spawn material at position" << std::endl;
    std::cout << "  !gravity [value] - Custom gravity command" << std::endl;
    std::cout << "  !color [name] - Custom color command" << std::endl;
    std::cout << "  !rain [type] - Make it rain!" << std::endl;
    std::cout << "  !help - Show available commands" << std::endl;
    std::cout << std::endl;
    std::cout << "Waiting for Twitch connection..." << std::endl;
    
    // Wait for connection
    int connectionAttempts = 0;
    while (!adapter->IsConnected() && connectionAttempts < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        connectionAttempts++;
    }
    
    if (adapter->IsConnected()) {
        std::cout << "Connected to Twitch chat!" << std::endl;
    } else {
        std::cout << "Failed to connect to Twitch chat. Continuing anyway..." << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Running for 60 seconds. Type commands in Twitch chat!" << std::endl;
    std::cout << "Press Ctrl+C to stop early." << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // Main loop - run for 60 seconds
    auto startTime = std::chrono::steady_clock::now();
    auto lastUpdateTime = startTime;
    
    while (true) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
        
        if (elapsed.count() >= 60) {
            break;
        }
        
        // Update at 30 Hz
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastUpdateTime);
        if (deltaTime.count() >= 33) {
            // Poll Twitch commands
            inputManager->Update();
            
            // Process any queued commands
            inputSystem->ExecuteCommands();
            
            lastUpdateTime = currentTime;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::cout << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Demo complete!" << std::endl;
    
    // Show some stats
    std::cout << std::endl;
    std::cout << "Final state:" << std::endl;
    std::cout << "  Selected material: " << static_cast<int>(inputManager->GetSelectedMaterial()) << std::endl;
    std::cout << "  Brush size: " << inputManager->GetBrushSize() << std::endl;
    
    // Clean up
    std::cout << std::endl;
    std::cout << "Disabling Twitch integration..." << std::endl;
    inputManager->DisableTwitchIntegration();
    
    std::cout << "Example finished!" << std::endl;
    
    return 0;
}