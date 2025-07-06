# Input Module

## Overview

The Input module provides a command-based input system for the Funhouse engine. It follows the Command pattern to enable features like input recording, playback, and flexible key binding.

## Architecture

### Core Components

1. **InputCommand** - Base interface for all input commands
   - Execute() - Performs the command action
   - Clone() - Creates a copy for recording/playback
   - GetName() - Returns command name for debugging
   - IsReplayable() - Indicates if command should be recorded

2. **InputSystem** - Main input processing system
   - Processes SDL events and converts them to commands
   - Manages command queue and execution
   - Provides recording and playback functionality
   - Tracks mouse and keyboard state

3. **InputManager** - High-level input configuration
   - Sets up default key bindings
   - Manages material selection state
   - Handles continuous mouse drawing

### Command Types

#### Mouse Commands
- **PlaceMaterialCommand** - Places a single pixel of material
- **RemoveMaterialCommand** - Removes material (sets to Air)
- **MouseDrawCommand** - Draws with configurable brush size

#### Keyboard Commands
- **SelectMaterialCommand** - Changes selected material
- **ClearWorldCommand** - Clears entire world
- **ToggleRecordingCommand** - Starts/stops input recording
- **TogglePlaybackCommand** - Plays back recorded inputs

## Usage

### Basic Integration
```cpp
// Create input system
auto inputSystem = std::make_unique<InputSystem>();
auto inputManager = std::make_unique<InputManager>(inputSystem.get(), world.get());
inputManager->Initialize();

// In event loop
SDL_Event event;
while (SDL_PollEvent(&event)) {
    inputSystem->ProcessEvent(event, world.get());
}

// In update loop
inputSystem->Update();
inputSystem->ExecuteCommands();
```

### Adding New Commands
1. Create a class inheriting from InputCommand
2. Implement Execute(), Clone(), and GetName()
3. Register command factory in InputManager

### Recording & Playback
```cpp
// Start recording
inputSystem->StartRecording();

// Stop and get recorded commands
inputSystem->StopRecording();
auto commands = inputSystem->GetRecordedCommands();

// Playback
inputSystem->StartPlayback(commands);
```

## Default Controls

- **Mouse Left** - Draw with selected material
- **Mouse Right** - Erase (set to Air)
- **1-4** - Select materials (Air, Sand, Water, Stone)
- **+/-** - Increase/decrease brush size
- **C** - Clear world
- **R** - Toggle recording
- **P** - Playback recording
- **ESC** - Exit application

## Future Enhancements

- Input contexts for different game states
- Configurable key bindings
- Save/load recorded input sequences
- Network command replication
- Undo/redo support