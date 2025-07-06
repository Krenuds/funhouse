# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 🚨 GOLDEN RULE: TEST-DRIVEN DEVELOPMENT (TDD) 🚨
**This is a test-driven development environment. ALWAYS follow TDD practices:**
1. **Write tests FIRST** - Before implementing any new feature or fixing any bug
2. **Red-Green-Refactor** cycle:
   - RED: Write a failing test that defines the desired behavior
   - GREEN: Write minimal code to make the test pass
   - REFACTOR: Improve the code while keeping tests green
3. **No code without tests** - Every public API must have corresponding unit tests
4. **Test coverage target**: Maintain minimum 80% coverage for all modules
5. **Run tests before commits** - Always run `make test` before committing

### TDD Workflow for Claude Code:
1. When asked to implement a feature, FIRST create the test file
2. Define the expected behavior through test cases
3. Run tests to ensure they fail (Red phase)
4. Implement the minimal code to pass tests (Green phase)
5. Refactor if needed while ensuring tests still pass
6. Document test rationale in test files

## Tech Stack
- C++17 (migrating to C++20 for modules support)
- GNU Make build system (future: CMake 3.20+)
- OpenGL 3.3+ for rendering
- Box2D for rigid body physics
- SDL2 for window management and input
- Single-threaded initially, multi-threading planned
- **Testing Framework**: Catch2 v3.7.1

## Architecture Principles
- Data-oriented design for cache efficiency
- Single-buffer pixel simulation (no double buffering)
- Chunk-based world management for scalability
- Interface-based design for testability
- RAII for automatic resource management
- Emergent complexity from simple rules

## Build and Development Commands

### Testing Commands (USE THESE FIRST!)
```bash
# Run all tests
make test

# Run tests with verbose output
make test-verbose

# Build and run tests directly
make run-tests
```

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
- Create executable at `build/funhouse`
- Use optimization level -O2 with all warnings enabled
- **Run tests with `make test` before any commits**

### Test Organization
```
tests/
├── external/           # Catch2 framework files
├── input/             # Input system tests
├── simulation/        # Physics simulation tests (TBD)
├── rendering/         # Rendering tests (TBD)
├── world/            # World management tests (TBD)
└── test_main.cpp     # Test runner entry point
```
**Every module in `modules/` MUST have corresponding tests in `tests/`**

## High-Level Architecture

### Project Overview
Funhouse is a general-purpose falling sand physics engine designed to power various types of games. The engine provides a robust foundation for pixel-based physics simulation, allowing developers to create games with emergent gameplay through material interactions. This is an ENGINE project focused on providing tools and APIs for game developers, not a specific game implementation.

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

### Development Focus
The project follows an engine-first approach:
1. **Core Engine**: Physics simulation, rendering, chunk system
2. **Developer Tools**: Material editor, level editor, debugging tools
3. **API Design**: Clean interfaces for game developers
4. **Example Games**: Demonstrations of engine capabilities

### Engine/Game Separation
- Keep engine code generic and reusable
- No game-specific logic in engine modules
- Examples and demos go in `examples/` directory
- Tools are separate applications using the engine

### Important Design Decisions
- **Single Buffer > Double Buffer**: Better for falling sand, less memory, simpler implementation
- **Chunks for Optimization**: Essential for scaling beyond small prototypes
- **Emergent Complexity**: Simple rules create complex behaviors
- **Performance Target**: 1 million active pixels at 60 FPS

## Key Documentation Files

- `documentation/falling-sand-engine-guide.md`: Comprehensive technical guide for building the engine
- `documentation/noita-tech-design-talk.md`: Technical insights on falling sand implementation
- `ENGINE_ARCHITECTURE.md`: Pure engine design documentation
- `TOOLING_ROADMAP.md`: Development tools planning

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

### Test-Driven Development Example
When implementing a new feature (e.g., "Add gravity to water particles"):
1. **First** create `tests/simulation/test_water_gravity.cpp`
2. Write test cases that define expected behavior
3. Run `make test` to see tests fail (RED)
4. Implement minimal code in `modules/simulation/`
5. Run `make test` until all tests pass (GREEN)
6. Refactor and optimize while keeping tests green
7. Commit with message: "feat(simulation): Add gravity to water particles with TDD"

### Git and Version Control
- **Always run `make test` before committing**
- After completing a task always make a git commit with a detailed description and push it
- Use feature branches for new modules
- Commit messages should follow: "module: description"
- Include "TDD" in commit messages when following test-driven development

## Problem-Solving Strategies
- If a build fails, or something goes wrong more than TWICE, look it up online.

## Important Reminders
- Never make assumptions about features or implementations. If you have questions just ask me!