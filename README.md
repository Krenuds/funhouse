# Funhouse - Falling Sand Physics Engine

A high-performance, general-purpose falling sand simulation engine written in C++. Build games with realistic pixel-based physics, fluid dynamics, and material interactions.

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

Funhouse is a versatile physics engine designed for creating games with pixel-perfect physics simulation. Whether you're building a sandbox game, a destruction simulator, or a unique physics puzzler, Funhouse provides the foundation for complex material interactions and emergent gameplay.

### Engine Capabilities
- **High-Performance Simulation**: Handle millions of active pixels at 60+ FPS
- **Rich Material System**: Define custom materials with unique properties
- **Fluid Dynamics**: Realistic liquid and gas behavior
- **Chemical Reactions**: Materials that interact, transform, and react
- **Chunk-Based Optimization**: Efficient simulation of large worlds
- **Multi-threaded**: Leverages modern CPUs for parallel simulation

## Technical Stack

- **Language**: C++17 (migrating to C++20)
- **Graphics**: OpenGL 3.3+ (Vulkan planned)
- **Window/Input**: SDL2
- **Physics**: Custom pixel simulation + Box2D for rigid bodies
- **Build System**: GNU Make (CMake planned)
- **Platform**: Cross-platform (Linux, Windows, macOS)

## Engine Features

### 1. Advanced Physics Simulation
- Per-pixel physics with customizable update rules
- Powder, liquid, gas, and solid material types
- Density-based displacement and settling
- High-velocity particle system for explosions and projectiles

### 2. Flexible Material System
- Define materials through simple property files
- Temperature-based state transitions
- Chemical reactions and material transformations
- Custom update behaviors per material

### 3. Optimized Rendering
- GPU-accelerated pixel buffer rendering
- Chunk-based dirty rectangle optimization
- Support for millions of on-screen pixels
- Minimal CPU-GPU data transfer

### 4. Developer-Friendly Architecture
- Modular design for easy extension
- Clean separation between engine and game logic
- Comprehensive API for game developers
- Well-documented interfaces

## Planned Development Tools

### Material Editor
- Visual material property editing
- Real-time preview of material behaviors
- Reaction rule designer

### Level Editor  
- Paint materials directly into worlds
- Save/load level formats
- Entity placement tools

### Simulation Debugger
- Chunk activity visualization
- Performance profiling overlay
- Material flow analysis

## Building

```bash
# Current build process
make

# Run the engine demo
make run
```

## Project Structure

```
funhouse/
├── modules/           # Engine modules
│   ├── core/          # Memory, containers, math
│   ├── simulation/    # Physics simulation
│   ├── rendering/     # Graphics pipeline
│   ├── materials/     # Material definitions
│   └── physics/       # Box2D integration
├── examples/          # Example games/demos
├── tools/             # Development tools
├── documentation/     # Technical docs
└── tests/             # Unit tests
```

## Example Applications

The Funhouse engine can power various game types:
- **Sandbox Games**: Players manipulate materials to solve puzzles
- **Destruction Games**: Realistic material destruction and explosions
- **Alchemy Games**: Complex material reactions and transformations
- **Roguelikes**: Procedural worlds with emergent gameplay
- **Educational Sims**: Chemistry and physics demonstrations

## Contributing

See CONTRIBUTING.md for guidelines.

## License

TBD