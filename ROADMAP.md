# Funhouse Development Roadmap

## Phase 1: Foundation (Weeks 1-4)

### Week 1-2: Project Setup & Core Systems
- [ ] Set up CMake build system
- [ ] Configure development environment
- [ ] Implement basic window creation (SDL2/GLFW)
- [ ] Set up OpenGL context
- [ ] Create basic game loop with fixed timestep
- [ ] Implement basic input handling

### Week 3-4: Basic Rendering
- [ ] Implement pixel buffer rendering
- [ ] Create texture atlas system
- [ ] Basic camera system
- [ ] Simple sprite rendering
- [ ] Debug overlay system

## Phase 2: Physics Engine (Weeks 5-8)

### Week 5-6: Pixel Simulation Foundation
- [ ] Design pixel data structure (position, velocity, material type)
- [ ] Implement spatial partitioning (chunk system)
- [ ] Basic falling sand simulation
- [ ] Collision detection between pixels

### Week 7-8: Material System
- [ ] Create material definition system
- [ ] Implement basic materials:
  - [ ] Sand (powder)
  - [ ] Water (liquid)
  - [ ] Stone (solid)
  - [ ] Air (gas)
- [ ] Material state transitions
- [ ] Basic temperature system

## Phase 3: Advanced Physics (Weeks 9-12)

### Week 9-10: Fluid Dynamics
- [ ] Implement liquid flow simulation
- [ ] Pressure system
- [ ] Gas diffusion
- [ ] Buoyancy

### Week 11-12: Interactions
- [ ] Material reactions (water + lava = steam + stone)
- [ ] Explosions and force propagation
- [ ] Fire propagation
- [ ] Electrical conductivity

## Phase 4: Game Systems (Weeks 13-16)

### Week 13-14: Entity System
- [ ] Implement ECS architecture
- [ ] Player entity with physics body
- [ ] Basic movement and controls
- [ ] Collision with pixel world

### Week 15-16: Core Gameplay
- [ ] Wand/spell system basics
- [ ] Projectile system
- [ ] Basic enemies
- [ ] Health and damage

## Phase 5: World Generation (Weeks 17-20)

### Week 17-18: Procedural Generation
- [ ] Noise-based terrain generation
- [ ] Biome system
- [ ] Cave generation
- [ ] Material distribution

### Week 19-20: Level Structure
- [ ] Room templates
- [ ] Enemy placement
- [ ] Treasure generation
- [ ] Holy Mountain areas

## Phase 6: Polish & Optimization (Weeks 21-24)

### Week 21-22: Performance
- [ ] Multithreading for physics
- [ ] GPU acceleration investigation
- [ ] Memory optimization
- [ ] Profiling and bottleneck fixes

### Week 23-24: Polish
- [ ] Particle effects
- [ ] Sound system integration
- [ ] UI/HUD
- [ ] Save system

## Future Phases

### Phase 7: Advanced Features
- Complex spell system
- Alchemy mechanics
- Advanced AI
- Mod support

### Phase 8: Content
- More materials
- More enemies
- More spells
- More biomes

## Technical Milestones

1. **Milestone 1**: Basic pixel simulation running at 60 FPS
2. **Milestone 2**: Water flowing realistically
3. **Milestone 3**: Player can move and shoot in pixel world
4. **Milestone 4**: Procedurally generated playable level
5. **Milestone 5**: Full game loop with progression

## Success Metrics

- Performance: 1 million active pixels at 60 FPS
- Stability: No crashes in 1-hour play sessions
- Gameplay: Core loop is fun and engaging
- Moddability: Easy to add new materials and spells