# Core Module

## Purpose
The core module provides fundamental engine functionality including:
- Application lifecycle management
- Main game loop with fixed timestep
- Window management
- Event processing

## Key Components
- `Application`: Main application class that manages the window and game loop
- Fixed timestep implementation for deterministic physics

## Dependencies
- SDL2 for window management
- OpenGL context creation

## Design Decisions
- Single Application instance manages entire lifecycle
- Fixed 60 FPS timestep for consistent physics simulation
- Event processing happens before physics updates