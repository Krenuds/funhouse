# Twitch IRC Module

This module provides a threaded Twitch IRC client that connects to Twitch chat and monitors commands typed by users. Commands are stored in a thread-safe circular buffer for consumption by other parts of the engine.

## Features

- **Threaded Operation**: Runs in its own background thread, non-blocking
- **Thread-Safe Circular Buffer**: Stores commands with automatic overflow handling
- **Automatic Reconnection**: Handles connection drops and reconnects automatically
- **Command Parsing**: Detects commands starting with `!` prefix
- **Statistics Tracking**: Monitors connection stats and message counts
- **Anonymous Connection**: No OAuth required, uses anonymous Twitch IRC access

## Usage

### Basic Setup

```cpp
#include "modules/twitch/TwitchIrcClient.h"

// Create client with default config (connects to #travisrashguard)
Funhouse::TwitchIrcClient twitch_client;

// Start the IRC client in background thread
twitch_client.start();

// Main game loop
while (running) {
    // Get new commands since last check
    auto new_commands = twitch_client.getCommandBuffer().getNewCommands();
    
    for (const auto& cmd : new_commands) {
        std::cout << "User: " << cmd.username 
                  << " Command: " << cmd.command 
                  << " Params: " << cmd.parameters << std::endl;
    }
    
    // Your game logic here...
}

// Stop the client when shutting down
twitch_client.stop();
```

### Custom Configuration

```cpp
Funhouse::TwitchIrcClient::Config config;
config.channel = "somestreamername";
config.command_prefix = "!";
config.buffer_capacity = 2000;
config.auto_reconnect = true;
config.reconnect_delay_seconds = 10;

Funhouse::TwitchIrcClient twitch_client(config);
```

### Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `channel` | `"travisrashguard"` | Twitch channel to monitor (without #) |
| `server` | `"irc.chat.twitch.tv"` | IRC server address |
| `port` | `6667` | IRC server port |
| `nickname` | `"justinfan12345"` | IRC nickname (anonymous) |
| `command_prefix` | `"!"` | Character that commands must start with |
| `auto_reconnect` | `true` | Automatically reconnect on disconnect |
| `reconnect_delay_seconds` | `5` | Delay between reconnection attempts |
| `ping_timeout_seconds` | `300` | Disconnect if no data for this long |
| `buffer_capacity` | `1000` | Maximum commands stored in buffer |

## API Reference

### TwitchCommand Structure

```cpp
struct TwitchCommand {
    std::string username;        // Twitch username who sent command
    std::string command;         // Command name (without prefix)
    std::string parameters;      // Everything after the command
    std::chrono::steady_clock::time_point timestamp; // When received
};
```

### TwitchCommandBuffer Methods

- `addCommand(const TwitchCommand& command)` - Add command to buffer
- `getNewCommands()` - Get all new commands since last call
- `getAllCommands()` - Get all commands currently in buffer
- `clear()` - Clear the entire buffer
- `size()` - Current number of commands in buffer
- `capacity()` - Maximum buffer capacity
- `isEmpty()` / `isFull()` - Buffer state checks

### TwitchIrcClient Methods

- `start()` - Start IRC client thread
- `stop()` - Stop IRC client and wait for thread to finish
- `isConnected()` - Check connection status
- `getCommandBuffer()` - Access to the command buffer
- `getStats()` - Get connection statistics

### Statistics

```cpp
struct Stats {
    std::chrono::steady_clock::time_point connect_time;
    uint64_t messages_received;   // Total IRC messages
    uint64_t commands_received;   // Commands starting with prefix
    uint64_t reconnection_count;  // Number of reconnections
    bool is_connected;            // Current connection state
};
```

## Examples

### Example Commands in Chat

When users type in Twitch chat:
- `!jump` → Command: "jump", Parameters: ""
- `!spawn sand 100 200` → Command: "spawn", Parameters: "sand 100 200"
- `!gravity 0.5` → Command: "gravity", Parameters: "0.5"
- `hello world` → (ignored, no ! prefix)

### Integration with Input System

```cpp
// In your main loop
auto commands = twitch_client.getCommandBuffer().getNewCommands();
for (const auto& cmd : commands) {
    if (cmd.command == "jump") {
        // Trigger jump in game
    } else if (cmd.command == "spawn") {
        std::istringstream iss(cmd.parameters);
        std::string material;
        int x, y;
        if (iss >> material >> x >> y) {
            // Spawn material at x, y
        }
    }
}
```

## Thread Safety

- All public methods are thread-safe
- The circular buffer uses mutex protection
- IRC operations run in background thread
- No locking required when reading commands from main thread

## Error Handling

- Automatic reconnection on network failures
- Graceful handling of malformed IRC messages
- Buffer overflow protection (oldest commands discarded)
- Socket timeout handling

## Dependencies

- Standard C++17 libraries
- POSIX sockets (Linux/Unix systems)
- No external dependencies required

## Future Enhancements

- OAuth authentication for better chat access
- Rate limiting for command processing
- Command validation and filtering
- Integration with Twitch API for subscriber-only commands
- Configurable command cooldowns per user
