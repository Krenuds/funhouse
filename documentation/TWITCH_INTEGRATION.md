# Twitch Integration with Input System

This document describes how the Twitch IRC module integrates with the Funhouse input command system.

## Overview

The Twitch integration allows viewers in a Twitch chat to control the Funhouse engine through chat commands. All Twitch commands are routed through the existing InputCommand system, ensuring they work seamlessly with features like recording, playback, and input contexts.

## Architecture

### Key Components

1. **TwitchIrcClient** - The existing IRC client that connects to Twitch chat
2. **TwitchInputCommand** - Base class that bridges TwitchCommand and InputCommand
3. **TwitchChatCommand** - Generic command that executes callbacks
4. **TwitchCommandAdapter** - Polls Twitch client and converts commands to InputCommands

### Data Flow

```
Twitch Chat → TwitchIrcClient → TwitchCommandBuffer
                                        ↓
                              TwitchCommandAdapter.Update()
                                        ↓
                              Convert to InputCommand
                                        ↓
                              InputSystem.QueueCommand()
                                        ↓
                              InputSystem.ExecuteCommands()
```

## Integration Points

### In Application/Main Loop

```cpp
// During initialization
inputManager->EnableTwitchIntegration();

// In update loop
inputManager->Update();  // This polls Twitch commands
inputSystem->ExecuteCommands();  // Executes all queued commands
```

### In InputManager

The InputManager handles:
- Creating and managing TwitchIrcClient
- Setting up TwitchCommandAdapter
- Registering command handlers
- Polling for new commands each frame

## Available Commands

Default commands registered by InputManager:

- `!sand` - Select sand material
- `!water` - Select water material  
- `!stone` - Select stone material
- `!air` - Select air material
- `!clear` - Clear the world
- `!brush [size]` - Set brush size (1-50)
- `!spawn [material] [x] [y]` - Spawn material at position
- `!help` - List available commands

## Creating Custom Commands

### Method 1: Register a Callback

```cpp
adapter->RegisterCommandCallback("jump", 
    [](const std::string& username, const std::string& command, const std::string& params) {
        std::cout << username << " wants to jump!" << std::endl;
        // Trigger jump action
    });
```

### Method 2: Register a Command Factory

```cpp
adapter->RegisterCommandHandler("spawn",
    [world](const TwitchCommand& cmd) -> InputCommandPtr {
        // Parse parameters and create appropriate command
        return std::make_unique<SpawnCommand>(world, cmd.parameters);
    });
```

### Method 3: Create Specialized Command Classes

```cpp
class TwitchGravityCommand : public TwitchInputCommand {
public:
    TwitchGravityCommand(const TwitchCommand& cmd, PhysicsSystem* physics)
        : TwitchInputCommand(cmd), physics_(physics) {}
    
    void Execute() override {
        float gravity = parseFloat(parameters_);
        physics_->SetGravity(gravity);
    }
    
    std::unique_ptr<InputCommand> Clone() const override {
        return std::make_unique<TwitchGravityCommand>(
            TwitchCommand(username_, command_, parameters_), physics_);
    }
    
private:
    PhysicsSystem* physics_;
};
```

## Configuration

### Basic Setup

```cpp
// Use default configuration (connects to #travisrashguard)
inputManager->EnableTwitchIntegration();
```

### Custom Configuration

```cpp
TwitchIrcClient::Config config;
config.channel = "yourchannel";
config.command_prefix = "!";
config.buffer_capacity = 2000;
config.auto_reconnect = true;

inputManager->EnableTwitchIntegration(config);
```

### Disable Integration

```cpp
inputManager->DisableTwitchIntegration();
```

## Benefits of InputCommand Integration

1. **Recording/Playback**: Twitch commands are automatically included in recordings
2. **Input Contexts**: Can be filtered based on active contexts
3. **Unified Processing**: All input goes through the same pipeline
4. **Command Pattern**: Supports undo/redo if implemented
5. **Thread Safety**: Commands are queued and executed on main thread
6. **Testability**: Can mock Twitch commands for testing

## Testing

The integration includes comprehensive unit tests:

```bash
# Run all tests including Twitch integration tests
make test

# Run only Twitch command tests
./build/test_runner "[TwitchCommands]"
```

## Example: Full Integration

```cpp
// In Application::Initialize()
inputManager_ = std::make_unique<InputManager>(&inputSystem_, world_.get());
inputManager_->Initialize();

// Enable Twitch with custom channel
TwitchIrcClient::Config twitchConfig;
twitchConfig.channel = "mychannel";
inputManager_->EnableTwitchIntegration(twitchConfig);

// In Application::Update()
inputManager_->Update();      // Poll Twitch commands
inputSystem_.Update();        // Update input states
inputSystem_.ExecuteCommands(); // Execute all commands

// Custom command registration
auto adapter = inputManager_->GetTwitchAdapter();
if (adapter) {
    adapter->RegisterCommandCallback("pause",
        [this](const std::string& user, const std::string&, const std::string&) {
            isPaused_ = !isPaused_;
            std::cout << user << " toggled pause!" << std::endl;
        });
}
```

## Thread Safety

- TwitchIrcClient runs in a background thread
- TwitchCommandBuffer is thread-safe with mutex protection
- Commands are polled and executed on the main thread
- No race conditions between IRC thread and game thread

## Future Enhancements

- Permission system (mod-only, subscriber-only commands)
- Cooldown system to prevent spam
- Command validation and sanitization
- Visual feedback for Twitch commands
- Integration with Twitch API for user info
- Command queuing with priorities