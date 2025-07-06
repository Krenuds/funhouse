# Input Module

## Purpose
Manages all user input including:
- Keyboard input
- Mouse input and tracking
- Controller support
- Input mapping and configuration

## Key Components
- `Input`: Main input handling class
- Event processing from SDL2
- Action mapping system

## Input Actions
- Movement (WASD/arrows)
- Material selection (number keys)
- Drawing/placing materials (mouse)
- Camera controls
- Debug commands

## Design Decisions
- Poll-based input for game actions
- Event-based for UI interactions
- Configurable key bindings
- Support for multiple input devices