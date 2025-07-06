# Funhouse - Falling Sand Simulation Engine

A C++ physics-based roguelike engine inspired by Noita's falling sand simulation and pixel-based physics.

## Current Status - Proof of Concept ✓

We've successfully implemented a basic falling sand simulation with:
- **Sand**: Falls down, then diagonally if blocked
- **Water**: Falls and spreads horizontally  
- **Stone**: Static solid blocks
- **Air**: Empty space

### Console Demo

A working console demo is available that shows the core simulation:

```bash
# Build the console demo
make -f Makefile.console

# Run it
./build/console_demo
```

The demo displays:
- `.` for sand
- `~` for water
- `#` for stone
- ` ` for air

## Overview

This project aims to recreate the core mechanics of Noita's engine, including:
- Falling sand simulation with per-pixel physics
- Fluid dynamics for liquids and gases
- Material interactions and transformations
- Spell system with projectile physics
- Procedural world generation

## Technical Stack

- **Language**: C++17/20
- **Graphics**: OpenGL 4.5+ / Vulkan
- **Physics**: Custom pixel-based simulation
- **Build System**: CMake
- **Platform**: Cross-platform (Linux, Windows, macOS)

## Core Features

### 1. Pixel Physics Engine
- Every pixel is simulated with physics properties
- Materials have different behaviors (solid, liquid, gas, powder)
- Real-time particle simulation at 60+ FPS

### 2. Material System
- Extensible material definitions
- Chemical reactions between materials
- Temperature and state changes

### 3. Rendering Pipeline
- Efficient pixel rendering with instancing
- Post-processing effects
- Lighting system

### 4. Game Systems
- Entity component system (ECS)
- Spell crafting mechanics
- Enemy AI
- Procedural level generation

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Project Structure

```
funhouse/
├── src/
│   ├── engine/        # Core engine systems
│   ├── physics/       # Physics simulation
│   ├── rendering/     # Graphics and rendering
│   ├── game/          # Game logic
│   └── utils/         # Utilities
├── include/           # Public headers
├── assets/            # Game assets
├── tests/             # Unit tests
└── docs/              # Documentation
```

## Contributing

See CONTRIBUTING.md for guidelines.

## License

TBD