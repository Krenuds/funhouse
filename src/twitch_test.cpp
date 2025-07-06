#include "../modules/twitch/TwitchIrcClient.h"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    std::cout << "=== Funhouse Twitch IRC Test ===" << std::endl;
    std::cout << "Connecting to Twitch chat: twitch.tv/travisrashguard" << std::endl;
    std::cout << "Type !test, !hello, !spawn sand, etc. in chat to see commands" << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;
    std::cout << std::endl;

    // Create Twitch IRC client with default config
    Funhouse::TwitchIrcClient twitch_client;
    
    // Start the IRC client
    if (!twitch_client.start()) {
        std::cerr << "Failed to start Twitch IRC client!" << std::endl;
        return -1;
    }
    
    std::cout << "IRC client started, waiting for connection..." << std::endl;
    
    // Wait for initial connection
    while (!twitch_client.isConnected()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "Connected! Monitoring chat for commands..." << std::endl;
    std::cout << "Commands will appear below:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // Main loop - monitor for commands
    auto last_stats_time = std::chrono::steady_clock::now();
    
    while (true) {
        // Get new commands
        auto new_commands = twitch_client.getCommandBuffer().getNewCommands();
        
        // Process each new command
        for (const auto& cmd : new_commands) {
            auto time_since_epoch = cmd.timestamp.time_since_epoch();
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time_since_epoch);
            
            std::cout << "[" << milliseconds.count() << "] " 
                      << cmd.username << ": !" << cmd.command;
            
            if (!cmd.parameters.empty()) {
                std::cout << " " << cmd.parameters;
            }
            std::cout << std::endl;
            
            // Example: Respond to specific commands
            if (cmd.command == "hello") {
                std::cout << "  -> Hello " << cmd.username << "!" << std::endl;
            } else if (cmd.command == "test") {
                std::cout << "  -> Test command received from " << cmd.username << std::endl;
            } else if (cmd.command == "spawn" && !cmd.parameters.empty()) {
                std::cout << "  -> Would spawn: " << cmd.parameters << std::endl;
            }
        }
        
        // Print stats every 30 seconds
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_stats_time).count() >= 30) {
            auto stats = twitch_client.getStats();
            std::cout << std::endl;
            std::cout << "=== Stats ===" << std::endl;
            std::cout << "Connected: " << (stats.is_connected ? "Yes" : "No") << std::endl;
            std::cout << "Messages received: " << stats.messages_received << std::endl;
            std::cout << "Commands received: " << stats.commands_received << std::endl;
            std::cout << "Reconnections: " << stats.reconnection_count << std::endl;
            std::cout << "Buffer size: " << twitch_client.getCommandBuffer().size() 
                      << "/" << twitch_client.getCommandBuffer().capacity() << std::endl;
            std::cout << "==============" << std::endl;
            std::cout << std::endl;
            
            last_stats_time = now;
        }
        
        // Check if still connected
        if (!twitch_client.isConnected()) {
            std::cout << "Connection lost, attempting to reconnect..." << std::endl;
        }
        
        // Small delay to prevent busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // This won't be reached in normal operation, but good practice
    twitch_client.stop();
    return 0;
}
