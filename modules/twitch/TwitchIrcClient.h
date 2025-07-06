#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <queue>
#include <memory>

namespace Funhouse {

/**
 * @brief Represents a Twitch chat command
 */
struct TwitchCommand {
    std::string username;
    std::string command;
    std::string parameters;
    std::chrono::steady_clock::time_point timestamp;
    
    TwitchCommand() = default;
    TwitchCommand(const std::string& user, const std::string& cmd, const std::string& params)
        : username{user}, command{cmd}, parameters{params}, timestamp{std::chrono::steady_clock::now()} {}
};

/**
 * @brief Thread-safe circular buffer for storing Twitch commands
 */
class TwitchCommandBuffer {
public:
    explicit TwitchCommandBuffer(size_t capacity = 1000);
    ~TwitchCommandBuffer() = default;

    // Add a command to the buffer
    void addCommand(const TwitchCommand& command);
    
    // Get all commands since last call (non-destructive)
    std::vector<TwitchCommand> getNewCommands();
    
    // Get all commands in buffer
    std::vector<TwitchCommand> getAllCommands() const;
    
    // Clear the buffer
    void clear();
    
    // Get buffer stats
    size_t size() const;
    size_t capacity() const;
    bool isEmpty() const;
    bool isFull() const;

private:
    mutable std::mutex buffer_mutex_;
    std::vector<TwitchCommand> buffer_;
    size_t head_;
    size_t tail_;
    size_t count_;
    size_t last_read_position_;
    const size_t capacity_;
};

/**
 * @brief Twitch IRC client that connects to chat and monitors commands
 */
class TwitchIrcClient {
public:
    struct Config {
        std::string channel;
        std::string server;
        int port;
        std::string nickname;
        std::string command_prefix;
        bool auto_reconnect;
        int reconnect_delay_seconds;
        int ping_timeout_seconds;
        size_t buffer_capacity;
        
        Config()
            : channel("travisrashguard")
            , server("irc.chat.twitch.tv")
            , port(6667)
            , nickname("justinfan12345")
            , command_prefix("!")
            , auto_reconnect(true)
            , reconnect_delay_seconds(5)
            , ping_timeout_seconds(300)
            , buffer_capacity(1000) {}
    };

    explicit TwitchIrcClient(const Config& config = Config());
    ~TwitchIrcClient();

    // Start the IRC client in its own thread
    bool start();
    
    // Stop the IRC client and join the thread
    void stop();
    
    // Check if connected
    bool isConnected() const;
    
    // Get the command buffer for reading commands
    const TwitchCommandBuffer& getCommandBuffer() const;
    TwitchCommandBuffer& getCommandBuffer();
    
    // Get connection stats
    struct Stats {
        std::chrono::steady_clock::time_point connect_time;
        uint64_t messages_received{0};
        uint64_t commands_received{0};
        uint64_t reconnection_count{0};
        bool is_connected{false};
    };
    Stats getStats() const;

private:
    // IRC connection and message handling
    bool connectToIrc();
    void disconnect();
    bool sendIrcMessage(const std::string& message);
    bool authenticate();
    bool joinChannel();
    void handleIrcMessage(const std::string& message);
    void parsePrivmsg(const std::string& message);
    
    // Thread functions
    void ircThreadMain();
    void handleIncomingData();
    void sendPing();
    
    // Socket operations
    int createSocket();
    bool connectSocket(int socket, const std::string& server, int port);
    void closeSocket();
    
    Config config_;
    TwitchCommandBuffer command_buffer_;
    
    // Threading
    std::unique_ptr<std::thread> irc_thread_;
    std::atomic<bool> should_stop_;
    std::atomic<bool> is_connected_;
    
    // Socket
    int socket_fd_;
    
    // Statistics
    mutable std::mutex stats_mutex_;
    Stats stats_;
    
    // Message buffer for partial IRC messages
    std::string message_buffer_;
    std::chrono::steady_clock::time_point last_ping_time_;
};

} // namespace Funhouse
