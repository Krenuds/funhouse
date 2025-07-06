# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Tech Stack
- C++17 (migrating to C++20 for modules support)
- GNU Make build system (future: CMake 3.20+)
- OpenGL 3.3+ for rendering
- Box2D for rigid body physics
- SDL2 for window management and input
- Single-threaded initially, multi-threading planned

## Architecture Principles
- Data-oriented design for cache efficiency
- Single-buffer pixel simulation (no double buffering)
- Chunk-based world management for scalability
- Interface-based design for testability
- RAII for automatic resource management
- Emergent complexity from simple rules

## Build and Development Commands

### Building the Project
```bash
# Build the project
make

# Clean build artifacts
make clean

# Build and run
make run
```

### Development Workflow
The project currently uses GNU Make with C++17. The Makefile is configured to:
- Compile all `.cpp` files from `src/` directory
- Output object files to `build/` directory
- Create executable at `build/hello`
- Use optimization level -O2 with all warnings enabled

## High-Level Architecture

### Project Overview
Funhouse is a Noita-inspired falling sand simulation engine implementing pixel-based physics. The core concept is that every pixel in the world is simulated with physics properties.

### Key Technical Concepts

#### 1. **Falling Sand Simulation Core**
The foundation is a single-buffer pixel simulation where materials update based on simple rules:
- **Sand**: Falls down, then diagonally if blocked
- **Water**: Like sand but with horizontal spread
- **Gas**: Inverted gravity (rises)
- **Rigid Bodies**: Integration with Box2D for complex physics

#### 2. **Update Order and Single Buffer**
- Uses bottom-to-top update order for falling materials
- Single buffer approach (no double buffering) for better performance
- Randomized left/right bias to avoid visual artifacts

#### 3. **Chunk System Architecture**
- World divided into 64×64 pixel chunks
- Dirty rectangle tracking per chunk for optimization
- Only chunks marked as dirty get updated
- Multi-threading uses 4-pass checker pattern for safe parallel updates

#### 4. **Particle System**
- Separate high-velocity particle simulation
- Particles convert back to grid pixels on collision
- Enables splashing effects and better liquid dynamics

#### 5. **Rigid Body Integration**
- Marching squares algorithm extracts contours from pixel groups
- Douglas-Peucker simplifies contours
- Triangulation creates physics mesh for Box2D
- Pixels track their position within rigid bodies

### Critical Implementation Details

1. **Material Storage**: Each pixel stores material type, with potential for additional properties (temperature, lifetime, state data)

2. **Multi-threading Safety**: The 4-pass checker pattern ensures threads work on chunks 2 chunks apart, preventing race conditions without mutexes

3. **World Streaming**: Keeps 12 512×512 areas in memory, streaming to/from disk as player moves

### Development Phases (from ROADMAP.md)
Currently targeting Phase 1: Foundation
- Window creation and OpenGL context
- Basic game loop with fixed timestep
- Input handling
- Pixel buffer rendering

### Important Design Decisions
- **Single Buffer > Double Buffer**: Better for falling sand, less memory, simpler implementation
- **Chunks for Optimization**: Essential for scaling beyond small prototypes
- **Emergent Complexity**: Simple rules create complex behaviors
- **Performance Target**: 1 million active pixels at 60 FPS

## Key Documentation Files

- `documentation/falling-sand-engine-guide.md`: Comprehensive technical guide for building the engine
- `documentation/noita-tech-design-talk.md`: Insights from Noita developers on implementation and design challenges

## Module Structure

### Core Systems (`modules/core/`)
- **memory/**: Custom allocators, memory pools
- **containers/**: Spatial hash maps, chunk arrays
- **math/**: Vector math, SIMD operations

### Simulation (`modules/simulation/`)  
- **falling_sand/**: Material update rules, physics simulation
- **chunk_system/**: 64×64 chunk management, dirty tracking
- **materials/**: Material definitions and properties
- **particles/**: High-velocity particle system

### Rendering (`modules/rendering/`)
- **opengl/**: OpenGL context, shader management
- Pixel buffer rendering
- Texture management

### Physics (`modules/physics/`)
- **box2d/**: Rigid body integration
- Marching squares contour extraction
- Physics mesh generation

### Shared (`modules/shared/`)
- **interfaces/**: Module interfaces
- **types/**: Common type definitions
- **utilities/**: Helper functions

## Performance Guidelines
- Profile before optimizing
- Minimize allocations in hot paths
- Use data-oriented design for cache efficiency
- Batch operations when possible
- Target: 1 million active pixels at 60 FPS

## Code Style
- PascalCase for classes: `class MaterialSystem`
- camelCase for functions: `void updatePixel()`
- snake_case for variables: `chunk_size`
- SCREAMING_SNAKE_CASE for constants: `MAX_CHUNKS`
- Always use `{}` for initialization
- Prefer `auto` when type is clear from context
- Use `constexpr` for compile-time constants

## Context Management
- Use `/clear` between major features
- Use `/compact` when context reaches 80%
- Reference specific files with `@filename`
- Keep module work focused and isolated

## Workflow Practices

### Git and Version Control
- After completing a task always make a git commit with a detailed description and push it
- Use feature branches for new modules
- Commit messages should follow: "module: description"