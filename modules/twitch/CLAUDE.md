# Twitch IRC Module - CLAUDE.md

## Overview
This module provides threaded Twitch IRC chat integration for the Funhouse engine. It connects to Twitch chat anonymously and monitors for commands (messages starting with `!`), storing them in a thread-safe circular buffer.

## Architecture
- **TwitchIrcClient**: Main IRC client class that handles connection and message parsing
- **TwitchCommandBuffer**: Thread-safe circular buffer for storing parsed commands
- **TwitchCommand**: Data structure representing a chat command

## Threading Model
- IRC network I/O runs in background thread
- Main thread reads commands from buffer (non-blocking)
- Thread-safe circular buffer with mutex protection
- Automatic reconnection handling

## Key Features
- Anonymous connection (no OAuth required)
- Configurable channel monitoring
- Command prefix filtering (`!` by default)
- Automatic reconnection on network failures
- Statistics tracking (messages, commands, reconnections)
- Ping/pong keepalive handling

## Dependencies
- POSIX sockets (Linux/Unix)
- Standard C++17 threading
- No external libraries required

## Integration Points
- Designed to work with existing input module
- Commands can be translated to input events
- Buffer provides both "new commands" and "all commands" access
- Statistics available for debugging/monitoring

## Configuration
All connection parameters configurable via Config struct:
- Channel name, server, port
- Reconnection behavior
- Buffer capacity
- Timeout settings

## Example Usage
```cpp
TwitchIrcClient client;
client.start();

// In main loop:
auto commands = client.getCommandBuffer().getNewCommands();
// Process commands...

client.stop(); // Clean shutdown
```

## Testing Notes
- Connect to #travisrashguard by default
- Anonymous connection allows immediate testing
- Commands logged to console for debugging
- Statistics show connection health
