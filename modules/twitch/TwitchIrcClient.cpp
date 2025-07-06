#include "TwitchIrcClient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace Funhouse {

// ============================================================================
// TwitchCommandBuffer Implementation
// ============================================================================

TwitchCommandBuffer::TwitchCommandBuffer(size_t capacity)
    : buffer_(capacity)
    , head_{0}
    , tail_{0}
    , count_{0}
    , last_read_position_{0}
    , capacity_{capacity} {
}

void TwitchCommandBuffer::addCommand(const TwitchCommand& command) {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    
    buffer_[head_] = command;
    head_ = (head_ + 1) % capacity_;
    
    if (count_ < capacity_) {
        ++count_;
    } else {
        // Buffer is full, move tail forward (overwrite oldest)
        tail_ = (tail_ + 1) % capacity_;
    }
}

std::vector<TwitchCommand> TwitchCommandBuffer::getNewCommands() {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    
    std::vector<TwitchCommand> new_commands;
    
    if (count_ == 0) {
        return new_commands;
    }
    
    size_t current_pos = last_read_position_;
    size_t current_head = head_;
    
    // Calculate how many new commands there are
    size_t new_count = 0;
    if (current_head >= current_pos) {
        new_count = current_head - current_pos;
    } else {
        new_count = (capacity_ - current_pos) + current_head;
    }
    
    // If we have more new commands than buffer capacity, 
    // we've wrapped around and lost some commands
    if (new_count > count_) {
        new_count = count_;
        current_pos = tail_;
    }
    
    new_commands.reserve(new_count);
    
    for (size_t i = 0; i < new_count; ++i) {
        new_commands.push_back(buffer_[current_pos]);
        current_pos = (current_pos + 1) % capacity_;
    }
    
    last_read_position_ = current_head;
    return new_commands;
}

std::vector<TwitchCommand> TwitchCommandBuffer::getAllCommands() const {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    
    std::vector<TwitchCommand> all_commands;
    all_commands.reserve(count_);
    
    size_t pos = tail_;
    for (size_t i = 0; i < count_; ++i) {
        all_commands.push_back(buffer_[pos]);
        pos = (pos + 1) % capacity_;
    }
    
    return all_commands;
}

void TwitchCommandBuffer::clear() {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    head_ = 0;
    tail_ = 0;
    count_ = 0;
    last_read_position_ = 0;
}

size_t TwitchCommandBuffer::size() const {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    return count_;
}

size_t TwitchCommandBuffer::capacity() const {
    return capacity_;
}

bool TwitchCommandBuffer::isEmpty() const {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    return count_ == 0;
}

bool TwitchCommandBuffer::isFull() const {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    return count_ == capacity_;
}

// ============================================================================
// TwitchIrcClient Implementation
// ============================================================================

TwitchIrcClient::TwitchIrcClient(const Config& config)
    : config_{config}
    , command_buffer_{config.buffer_capacity}
    , should_stop_{false}
    , is_connected_{false}
    , socket_fd_{-1}
    , last_ping_time_{std::chrono::steady_clock::now()} {
}

TwitchIrcClient::~TwitchIrcClient() {
    stop();
}

bool TwitchIrcClient::start() {
    if (irc_thread_ && irc_thread_->joinable()) {
        std::cout << "[TwitchIRC] Client already running\n";
        return false;
    }
    
    should_stop_ = false;
    is_connected_ = false;
    
    irc_thread_ = std::make_unique<std::thread>(&TwitchIrcClient::ircThreadMain, this);
    
    std::cout << "[TwitchIRC] Starting IRC client thread\n";
    return true;
}

void TwitchIrcClient::stop() {
    should_stop_ = true;
    
    if (irc_thread_ && irc_thread_->joinable()) {
        std::cout << "[TwitchIRC] Stopping IRC client...\n";
        irc_thread_->join();
        irc_thread_.reset();
    }
    
    disconnect();
}

bool TwitchIrcClient::isConnected() const {
    return is_connected_.load();
}

const TwitchCommandBuffer& TwitchIrcClient::getCommandBuffer() const {
    return command_buffer_;
}

TwitchCommandBuffer& TwitchIrcClient::getCommandBuffer() {
    return command_buffer_;
}

TwitchIrcClient::Stats TwitchIrcClient::getStats() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    Stats copy = stats_;
    copy.is_connected = is_connected_.load();
    return copy;
}

void TwitchIrcClient::ircThreadMain() {
    std::cout << "[TwitchIRC] IRC thread started\n";
    
    while (!should_stop_) {
        if (!isConnected()) {
            std::cout << "[TwitchIRC] Attempting to connect...\n";
            
            if (connectToIrc()) {
                std::cout << "[TwitchIRC] Connected successfully!\n";
                
                std::lock_guard<std::mutex> lock(stats_mutex_);
                stats_.connect_time = std::chrono::steady_clock::now();
                stats_.is_connected = true;
            } else {
                std::cout << "[TwitchIRC] Connection failed\n";
                
                if (config_.auto_reconnect && !should_stop_) {
                    std::cout << "[TwitchIRC] Waiting " << config_.reconnect_delay_seconds 
                              << " seconds before retry...\n";
                    
                    std::this_thread::sleep_for(
                        std::chrono::seconds(config_.reconnect_delay_seconds));
                    
                    std::lock_guard<std::mutex> lock(stats_mutex_);
                    stats_.reconnection_count++;
                }
                continue;
            }
        }
        
        // Handle incoming data
        handleIncomingData();
        
        // Check for ping timeout
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_ping_time_);
        
        if (elapsed.count() > config_.ping_timeout_seconds) {
            std::cout << "[TwitchIRC] Ping timeout, disconnecting\n";
            disconnect();
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    disconnect();
    std::cout << "[TwitchIRC] IRC thread terminated\n";
}

bool TwitchIrcClient::connectToIrc() {
    disconnect(); // Ensure clean state
    
    socket_fd_ = createSocket();
    if (socket_fd_ < 0) {
        return false;
    }
    
    if (!connectSocket(socket_fd_, config_.server, config_.port)) {
        closeSocket();
        return false;
    }
    
    if (!authenticate()) {
        closeSocket();
        return false;
    }
    
    if (!joinChannel()) {
        closeSocket();
        return false;
    }
    
    is_connected_ = true;
    last_ping_time_ = std::chrono::steady_clock::now();
    return true;
}

void TwitchIrcClient::disconnect() {
    is_connected_ = false;
    closeSocket();
    
    std::lock_guard<std::mutex> lock(stats_mutex_);
    stats_.is_connected = false;
}

int TwitchIrcClient::createSocket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cout << "[TwitchIRC] Failed to create socket\n";
        return -1;
    }
    
    // Set socket to non-blocking
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags >= 0) {
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    }
    
    return sock;
}

bool TwitchIrcClient::connectSocket(int socket, const std::string& server, int port) {
    struct hostent* host = gethostbyname(server.c_str());
    if (!host) {
        std::cout << "[TwitchIRC] Failed to resolve hostname: " << server << "\n";
        return false;
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr*)host->h_addr);
    
    int result = connect(socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    if (result < 0 && errno != EINPROGRESS) {
        std::cout << "[TwitchIRC] Failed to connect to " << server << ":" << port << "\n";
        return false;
    }
    
    // Wait for connection to complete (for non-blocking socket)
    fd_set write_fds;
    FD_ZERO(&write_fds);
    FD_SET(socket, &write_fds);
    
    struct timeval timeout;
    timeout.tv_sec = 10; // 10 second timeout
    timeout.tv_usec = 0;
    
    result = select(socket + 1, nullptr, &write_fds, nullptr, &timeout);
    if (result <= 0) {
        std::cout << "[TwitchIRC] Connection timeout\n";
        return false;
    }
    
    // Check if connection was successful
    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, &len) < 0 || error != 0) {
        std::cout << "[TwitchIRC] Connection failed with error: " << error << "\n";
        return false;
    }
    
    return true;
}

void TwitchIrcClient::closeSocket() {
    if (socket_fd_ >= 0) {
        close(socket_fd_);
        socket_fd_ = -1;
    }
}

bool TwitchIrcClient::sendIrcMessage(const std::string& message) {
    if (socket_fd_ < 0) return false;
    
    std::string full_message = message + "\r\n";
    ssize_t sent = send(socket_fd_, full_message.c_str(), full_message.length(), 0);
    
    return sent == static_cast<ssize_t>(full_message.length());
}

bool TwitchIrcClient::authenticate() {
    // For anonymous connection, we don't need authentication
    // Just send NICK and USER commands
    std::string nick_msg = "NICK " + config_.nickname;
    std::string user_msg = "USER " + config_.nickname + " 0 * :" + config_.nickname;
    
    return sendIrcMessage(nick_msg) && sendIrcMessage(user_msg);
}

bool TwitchIrcClient::joinChannel() {
    std::string channel = config_.channel;
    if (channel[0] != '#') {
        channel = "#" + channel;
    }
    
    std::string join_msg = "JOIN " + channel;
    return sendIrcMessage(join_msg);
}

void TwitchIrcClient::handleIncomingData() {
    if (socket_fd_ < 0) return;
    
    char buffer[4096];
    ssize_t bytes_received = recv(socket_fd_, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received <= 0) {
        if (bytes_received == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
            std::cout << "[TwitchIRC] Connection lost\n";
            disconnect();
        }
        return;
    }
    
    buffer[bytes_received] = '\0';
    message_buffer_ += buffer;
    
    // Process complete messages (lines ending with \r\n)
    size_t pos = 0;
    while ((pos = message_buffer_.find("\r\n")) != std::string::npos) {
        std::string message = message_buffer_.substr(0, pos);
        message_buffer_.erase(0, pos + 2);
        
        if (!message.empty()) {
            handleIrcMessage(message);
        }
    }
    
    std::lock_guard<std::mutex> lock(stats_mutex_);
    stats_.messages_received++;
}

void TwitchIrcClient::handleIrcMessage(const std::string& message) {
    // Handle PING messages
    if (message.substr(0, 4) == "PING") {
        std::string pong = "PONG" + message.substr(4);
        sendIrcMessage(pong);
        last_ping_time_ = std::chrono::steady_clock::now();
        return;
    }
    
    // Handle PRIVMSG (chat messages)
    if (message.find(" PRIVMSG ") != std::string::npos) {
        parsePrivmsg(message);
    }
    
    // Update ping time for any message
    last_ping_time_ = std::chrono::steady_clock::now();
}

void TwitchIrcClient::parsePrivmsg(const std::string& message) {
    // IRC PRIVMSG format: :username!user@host PRIVMSG #channel :message
    std::regex privmsg_regex(R"(:([^!]+)![^\s]+ PRIVMSG #[^\s]+ :(.+))");
    std::smatch matches;
    
    if (!std::regex_match(message, matches, privmsg_regex)) {
        return;
    }
    
    std::string username = matches[1].str();
    std::string chat_message = matches[2].str();
    
    // Check if message starts with command prefix
    if (chat_message.empty() || chat_message[0] != config_.command_prefix[0]) {
        return;
    }
    
    // Parse command and parameters
    std::istringstream iss(chat_message.substr(1)); // Remove prefix
    std::string command;
    iss >> command;
    
    std::string parameters;
    std::getline(iss, parameters);
    if (!parameters.empty() && parameters[0] == ' ') {
        parameters = parameters.substr(1); // Remove leading space
    }
    
    // Convert command to lowercase for consistency
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    // Add to command buffer
    TwitchCommand twitch_command(username, command, parameters);
    command_buffer_.addCommand(twitch_command);
    
    std::cout << "[TwitchIRC] Command: " << username << " -> !" << command;
    if (!parameters.empty()) {
        std::cout << " " << parameters;
    }
    std::cout << "\n";
    
    std::lock_guard<std::mutex> lock(stats_mutex_);
    stats_.commands_received++;
}

} // namespace Funhouse
