# Funhouse Engine Architecture

## Overview

Funhouse is a high-performance falling sand physics engine designed as a foundation for games featuring pixel-based physics simulation. This document describes the engine's architecture, core systems, and design principles.

## Design Philosophy

### Core Principles

1. **Performance First**: Target 2+ million active pixels at 60 FPS
2. **Modularity**: Clean separation between engine systems
3. **Data-Oriented Design**: Optimize for cache efficiency
4. **Emergent Complexity**: Simple rules create rich behaviors
5. **Developer Experience**: Easy to understand and extend

### Non-Goals

- This is NOT a game, it's an engine
- No built-in game mechanics (health, scores, levels)
- No specific genre assumptions
- No hard-coded material behaviors

## System Architecture

### Layer Structure

```
┌─────────────────────────────────────┐
│         Application Layer           │  ← Your game logic
├─────────────────────────────────────┤
│          Engine API                 │  ← Public interfaces
├─────────────────────────────────────┤
│     Core Engine Systems             │
│  ┌─────────┐ ┌─────────┐ ┌───────┐ │
│  │Simulation│ │Rendering│ │Physics│ │
│  └─────────┘ └─────────┘ └───────┘ │
├─────────────────────────────────────┤
│        Platform Layer               │  ← SDL2, OpenGL, OS
└─────────────────────────────────────┘
```

## Core Systems

### 1. Simulation System

The heart of the engine - manages pixel updates and material interactions.

#### Chunk System
- World divided into 64×64 pixel chunks
- Each chunk tracks:
  - Dirty state (needs update)
  - Active pixel count
  - Bounding box of changes
- Only dirty chunks are processed each frame

#### Update Pipeline
```cpp
for each dirty chunk:
    1. Update materials (bottom-to-top for falling)
    2. Apply particle collisions
    3. Process material reactions
    4. Mark neighboring chunks if affected
```

#### Material System
- Materials defined by properties:
  - Density (determines displacement)
  - State (solid, powder, liquid, gas)
  - Update behavior (function pointer/lambda)
  - Reaction rules
- Extensible through configuration files

### 2. Rendering System

Optimized for massive pixel counts using GPU acceleration.

#### Pixel Buffer Rendering
- Single texture for entire visible world
- Minimal CPU→GPU transfers
- Dirty rectangle updates only
- Chunk-based culling

#### Render Pipeline
```
1. Update dirty chunks in pixel buffer
2. Upload changed regions to GPU texture
3. Render textured quad with pixel shader
4. Apply post-processing (optional)
```

### 3. Physics Integration

Hybrid approach combining pixel and rigid body physics.

#### Pixel Physics
- Per-pixel rules for movement
- Density-based displacement
- Neighbor checking for collisions

#### Rigid Body Physics (Box2D)
- Marching squares for shape extraction
- Dynamic conversion between pixels and bodies
- Mixed simulation handling

### 4. Memory Management

#### Chunk Pool
- Pre-allocated chunk memory
- Spatial hash map for active chunks
- Streaming system for large worlds

#### Particle Pool
- Object pool for high-velocity particles
- Automatic return to pixel grid

## Threading Architecture

### 4-Pass Checker Pattern

Safe parallel chunk updates without locks:

```
Pass 1: Chunks at (0,0), (0,2), (2,0), (2,2)...
Pass 2: Chunks at (0,1), (0,3), (2,1), (2,3)...
Pass 3: Chunks at (1,0), (1,2), (3,0), (3,2)...
Pass 4: Chunks at (1,1), (1,3), (3,1), (3,3)...
```

Each pass updates chunks that are 2 chunks apart, preventing race conditions.

## API Design

### Core Interfaces

```cpp
// Material Definition
class IMaterial {
    virtual void update(Pixel& self, World& world) = 0;
    virtual bool canReactWith(MaterialID other) = 0;
    virtual void onReaction(Pixel& self, Pixel& other) = 0;
};

// World Interface
class IWorld {
    virtual Pixel* getPixel(int x, int y) = 0;
    virtual void setPixel(int x, int y, MaterialID mat) = 0;
    virtual void addParticle(const Particle& p) = 0;
};

// Engine Interface
class IEngine {
    virtual void update(float deltaTime) = 0;
    virtual void render(IRenderer* renderer) = 0;
    virtual IWorld* getWorld() = 0;
};
```

### Extension Points

1. **Custom Materials**: Implement IMaterial interface
2. **Rendering Effects**: Custom post-processing shaders
3. **Physics Behaviors**: Hook into update pipeline
4. **Tool Integration**: Access through engine API

## Performance Considerations

### Optimization Strategies

1. **Cache Efficiency**
   - Chunk size fits in L2 cache
   - Sequential memory access patterns
   - Hot/cold data separation

2. **Parallel Processing**
   - Independent chunk updates
   - SIMD for pixel operations
   - GPU compute shaders (future)

3. **Algorithmic Optimizations**
   - Spatial hashing for lookups
   - Dirty tracking to skip work
   - Early exit conditions

### Profiling Points

- Chunk update time
- Particle collision checks
- Render upload bandwidth
- Thread synchronization overhead

## Future Architecture Considerations

### Planned Enhancements

1. **GPU Compute**: Offload simulation to GPU
2. **Networking**: Deterministic simulation for multiplayer
3. **Scripting**: Lua bindings for materials
4. **Advanced Physics**: Soft bodies, ropes, joints

### Extensibility

The architecture is designed to support:
- New material types without engine changes
- Custom rendering pipelines
- Alternative physics backends
- Platform-specific optimizations

## Development Guidelines

### Adding New Systems

1. Define clear interfaces
2. Minimize dependencies
3. Design for testability
4. Document extension points

### Performance Testing

- Benchmark with 2M+ active pixels
- Profile on target hardware
- Test with various material mixes
- Verify thread scaling

## Conclusion

The Funhouse engine provides a solid foundation for pixel physics games while maintaining flexibility for diverse game types. The architecture prioritizes performance and modularity, enabling developers to focus on creating unique gameplay experiences rather than engine internals.