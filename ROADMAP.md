# Funhouse Engine Development Roadmap

## Current Status (Updated: January 2025)

### ✅ Completed
- GNU Make build system (C++17)
- SDL2 window creation with OpenGL 3.3 context
- Fixed timestep game loop (60 FPS)
- Basic input handling (window events, ESC key)
- Pixel buffer GPU rendering system
- Basic falling sand simulation (Sand, Water, Stone)
- Material system with density-based interactions
- Module-based architecture

### 🚧 In Progress
- Transitioning from prototype to full architecture
- Implementing chunk system for optimization

## Immediate Next Steps (Based on Architecture)

### Priority 1: Chunk System Implementation
- [ ] Implement 64×64 chunk structure
- [ ] Add dirty rectangle tracking per chunk
- [ ] Update only dirty chunks for performance
- [ ] Prepare for multi-threading with 4-pass checker pattern

### Priority 2: Enhanced Material System
- [ ] Add more material properties (temperature, lifetime, state data)
- [ ] Implement gas materials with inverted gravity
- [ ] Add material state transitions
- [ ] Improve liquid horizontal spread

### Priority 3: Performance & Input
- [ ] Profile current performance baseline
- [ ] Add mouse input for material placement
- [ ] Implement debug overlay with FPS counter
- [ ] Add camera panning controls

## Phase 1: Foundation (Weeks 1-4) ✅ COMPLETED

### Week 1-2: Project Setup & Core Systems
- [ ] Set up CMake build system *(using Make currently)*
- [x] Configure development environment
- [x] Implement basic window creation (SDL2/GLFW)
- [x] Set up OpenGL context
- [x] Create basic game loop with fixed timestep
- [x] Implement basic input handling

### Week 3-4: Basic Rendering
- [x] Implement pixel buffer rendering
- [ ] Create texture atlas system
- [ ] Basic camera system
- [ ] Simple sprite rendering
- [ ] Debug overlay system

## Phase 2: Core Simulation Engine 🚧 IN PROGRESS

### Chunk System Architecture
- [ ] Implement 64×64 chunk structure **PRIORITY**
- [ ] Dirty rectangle tracking per chunk
- [ ] Chunk-based update optimization
- [ ] Prepare for multi-threading architecture

### Enhanced Material System
- [ ] Extended material properties (temperature, pressure, lifetime)
- [ ] Material state transitions (solid ↔ liquid ↔ gas)
- [ ] Advanced liquid flow (pressure-based)
- [ ] Gas diffusion and buoyancy
- [ ] Material loading from configuration files

## Phase 3: Performance & Optimization

### Multi-threading Implementation
- [ ] 4-pass checker pattern for parallel chunk updates
- [ ] Thread pool management
- [ ] Lock-free data structures
- [ ] Performance profiling tools

### GPU Acceleration
- [ ] Investigate compute shader feasibility
- [ ] GPU-based particle system
- [ ] Texture-based material storage
- [ ] Hybrid CPU-GPU simulation

### Memory Optimization
- [ ] Custom memory allocators
- [ ] Object pooling for particles
- [ ] Chunk streaming system
- [ ] Memory usage profiling

## Phase 4: Developer Tools & API

### Engine API Design
- [ ] Public C++ API for game developers
- [ ] Material definition API
- [ ] Simulation control interface
- [ ] Event system for material interactions
- [ ] Lua scripting bindings (optional)

### Material Editor
- [ ] GUI for material property editing
- [ ] Visual reaction rule designer
- [ ] Real-time preview window
- [ ] Import/export material packs

### Level Editor
- [ ] Material painting tools
- [ ] Brush system with sizes/shapes
- [ ] Save/load world formats
- [ ] Copy/paste regions
- [ ] Undo/redo system

## Phase 5: Advanced Features & Integration

### Rigid Body Physics
- [ ] Box2D integration
- [ ] Marching squares contour extraction
- [ ] Pixel ↔ rigid body conversion
- [ ] Mixed simulation handling

### Advanced Material Physics
- [ ] Electricity conduction system
- [ ] Heat transfer and thermodynamics
- [ ] Complex chemical reactions
- [ ] Explosion and shockwave propagation
- [ ] Magnetism and attraction forces

## Phase 6: Example Games & Documentation

### Example Game Demos
- [ ] Simple sandbox demo
- [ ] Destruction physics demo
- [ ] Fluid simulation showcase
- [ ] "Alchemy" puzzle game
- [ ] Basic platformer with physics

### Documentation & Tutorials
- [ ] API reference documentation
- [ ] Getting started guide
- [ ] Material creation tutorial
- [ ] Performance optimization guide
- [ ] Example code snippets

## Future Considerations

### Engine Extensions
- Networking support for multiplayer
- VR/AR integration
- Mobile platform optimization
- Cloud-based simulation
- Machine learning for material behaviors

### Community & Ecosystem
- Plugin system architecture
- Asset marketplace
- Community material packs
- Game jam support

## Engine Milestones

1. **Milestone 1**: Chunk system with 1M+ pixels at 60 FPS ✅
2. **Milestone 2**: Multi-threaded simulation scaling to 8+ cores
3. **Milestone 3**: Complete material editor with visual preview
4. **Milestone 4**: First external game built with the engine
5. **Milestone 5**: Public API release v1.0

## Success Metrics

- **Performance**: 2+ million active pixels at 60 FPS on modern hardware
- **Stability**: 99.9% uptime in 24-hour stress tests
- **Developer Experience**: Build a basic game in under 1 hour
- **Extensibility**: Add new materials without engine recompilation
- **Documentation**: 100% API coverage with examples