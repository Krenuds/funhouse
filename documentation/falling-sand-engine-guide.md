# Building a Falling Sand Game Engine: Complete Guide

*A comprehensive guide to creating a Noita-style falling sand simulation engine*

## Table of Contents

1. [Core Principles](#core-principles)
2. [Implementation Roadmap](#implementation-roadmap)
3. [Technical Architecture](#technical-architecture)
4. [Critical Insights from Noita](#critical-insights-from-noita)
5. [Minimum Viable Engine](#minimum-viable-engine)
6. [Common Pitfalls to Avoid](#common-pitfalls-to-avoid)
7. [Advanced Topics](#advanced-topics)
8. [Performance Optimization](#performance-optimization)
9. [Resources and References](#resources-and-references)

## Core Principles

### 1. Pixel-Based Simulation
- Every pixel is a material with properties
- Single-buffer approach (no double buffering)
- Direct pixel manipulation for immediate feedback
- Each pixel occupies exactly one grid position

### 2. Material State System
- Each pixel knows its material type (sand, water, gas, rigid body, etc.)
- Materials have behaviors, not complex physics equations
- Keep it simple: 95% of the tech is just "look down, move if empty"
- Materials define their own update rules

### 3. Emergent Complexity from Simple Rules
- **Sand**: Falls down, then diagonally
- **Water**: Like sand + horizontal spread
- **Gas**: Inverted gravity (rises up)
- **Stone**: Static, doesn't move
- Each material just needs 5-10 lines of logic

## Implementation Roadmap

### Phase 1: Basic Falling Sand (Start Here!)

The foundation of everything. Get this working before moving on.

```python
# Pseudocode for the core loop
for each pixel from bottom to top:
    if pixel is sand:
        if below is empty:
            move down
        elif below_left is empty:
            move down_left
        elif below_right is empty:
            move down_right
```

**Why start here?** 
- Validates your rendering pipeline
- Tests basic simulation loop
- Immediate visual feedback
- This is 95% of the entire engine

**Success Criteria:**
- Sand falls and piles up naturally
- Forms realistic slopes (angle of repose)
- No visual glitches or stuttering

### Phase 2: Add Water

Extend the sand logic with horizontal movement.

```python
# Water behavior
if pixel is water:
    if below is empty:
        move down
    elif below_left is empty and below_right is empty:
        move randomly left or right
    elif left is empty:
        move left
    elif right is empty:
        move right
```

**Key Differences from Sand:**
- Horizontal spread when can't fall
- Should level out to form flat surfaces
- More fluid movement patterns

### Phase 3: Particle System

Separate high-velocity particles from grid simulation for better liquids.

```python
class Particle:
    x: float
    y: float
    vx: float
    vy: float
    material: MaterialType
    
def update_particle(p):
    # Apply gravity
    p.vy += GRAVITY
    
    # Update position
    p.x += p.vx
    p.y += p.vy
    
    # Check collision with grid
    if grid[int(p.y)][int(p.x)] != EMPTY:
        # Convert back to grid pixel
        place_in_grid(p)
        remove_particle(p)
```

**Benefits:**
- Splashing effects
- Better liquid dynamics
- Explosive materials
- High-velocity impacts

### Phase 4: Optimization with Chunks

Divide world into chunks for massive performance gains.

```cpp
struct Chunk {
    static const int SIZE = 64;
    Pixel pixels[SIZE][SIZE];
    Rectangle dirty_rect;
    bool needs_update;
    
    void mark_dirty(int x, int y) {
        dirty_rect.expand_to_include(x, y);
        needs_update = true;
    }
};
```

**Implementation Strategy:**
1. Start with simple chunk tracking
2. Only update chunks marked as dirty
3. Clear dirty flags after update
4. Expand dirty regions when pixels move between chunks

### Phase 5: Multithreading

Use checker pattern for safe parallel updates.

```cpp
// Four-pass update pattern
void update_world_multithreaded() {
    for (int pass = 0; pass < 4; pass++) {
        parallel_for(chunks) {
            if (chunk_index % 4 == pass) {
                update_chunk(chunk);
            }
        }
        wait_for_threads();
    }
}
```

**Safety Guarantees:**
- Each thread works on chunks 2 chunks apart
- Pixels can move up to 32 pixels without collision
- No mutex needed for pixel updates

### Phase 6: Rigid Bodies (Advanced)

Integration with Box2D for complex physics objects.

**Pipeline:**
1. **Marching Squares** → Extract contours from pixel groups
2. **Douglas-Peucker** → Simplify contours
3. **Triangulation** → Create physics mesh
4. **Box2D Integration** → Simulate rigid body physics

```cpp
void update_rigid_bodies() {
    // Remove rigid body pixels from grid
    extract_rigid_pixels();
    
    // Run physics simulation
    box2d_world->Step(timestep);
    
    // Place pixels back based on new positions
    for (auto& body : rigid_bodies) {
        for (auto& pixel : body.pixels) {
            Vector2 world_pos = body.transform * pixel.local_pos;
            place_pixel_in_grid(world_pos, pixel.material);
        }
    }
}
```

## Technical Architecture

### Key Design Decisions

#### 1. Single Buffer vs Double Buffer

**Choose Single Buffer (like Noita):**
- ✅ Simpler implementation
- ✅ Less memory usage
- ✅ Better cache performance
- ✅ Works naturally with falling sand logic
- ❌ Must be careful about update order

#### 2. Update Order

```cpp
// For falling materials (sand, water)
for (int y = HEIGHT - 2; y >= 0; y--) {
    for (int x = 0; x < WIDTH; x++) {
        update_pixel(x, y);
    }
}

// For rising materials (gas, smoke)
for (int y = 1; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
        update_pixel(x, y);
    }
}
```

**Important:** Randomize left/right bias to avoid visual artifacts:
```cpp
bool prefer_left = rand() % 2;
if (prefer_left) {
    // Check left first, then right
} else {
    // Check right first, then left
}
```

#### 3. Material Storage

```cpp
// Option 1: Simple (8 bits per pixel)
enum MaterialType : uint8_t {
    EMPTY = 0,
    SAND = 1,
    WATER = 2,
    STONE = 3,
    // ... up to 256 materials
};

// Option 2: Rich data (32 bits per pixel)
struct Pixel {
    uint8_t material_id;
    uint8_t temperature;    // For fire/ice mechanics
    uint8_t lifetime;       // For temporary materials
    uint8_t state_data;     // Material-specific data
};

// Option 3: Compact with properties (16 bits)
struct Pixel {
    uint16_t material_id : 8;
    uint16_t temperature : 4;
    uint16_t wetness : 2;
    uint16_t burning : 1;
    uint16_t updated : 1;
};
```

#### 4. Chunk System Architecture

```cpp
class World {
    static const int CHUNK_SIZE = 64;
    std::vector<Chunk> chunks;
    int chunks_wide;
    int chunks_high;
    
    Chunk& get_chunk(int world_x, int world_y) {
        int cx = world_x / CHUNK_SIZE;
        int cy = world_y / CHUNK_SIZE;
        return chunks[cy * chunks_wide + cx];
    }
    
    void set_pixel(int x, int y, MaterialType mat) {
        auto& chunk = get_chunk(x, y);
        int local_x = x % CHUNK_SIZE;
        int local_y = y % CHUNK_SIZE;
        chunk.pixels[local_y][local_x] = mat;
        chunk.mark_dirty(local_x, local_y);
    }
};
```

## Critical Insights from Noita

### What They Learned (So You Don't Have To)

#### 1. Start with Working Particles, Not Physics Accuracy
- "Does it look like water?" > "Is this physically accurate?"
- Players attribute more behavior than actually exists
- Visual feedback is more important than simulation accuracy
- You can always add more realism later

#### 2. The Physics Will Break Everything
- **Accept it**: Buildings will collapse into rubble
- **Design around it**: Make destruction fun, not frustrating
- **Embrace chaos**: It's a feature, not a bug
- **Limit scope**: Not everything needs to be destructible

#### 3. Optimization Order Matters
```
1. Get basic simulation working (any framerate)
2. Add spatial partitioning (chunks)
3. Implement dirty rectangles
4. Profile and optimize hot paths
5. Add multithreading last
```

Their 256×256 prototype was CPU-bound - don't assume GPU is the bottleneck!

#### 4. Debug Visualization is Crucial

Always implement these debug views:
- Dirty rectangles (what's being updated)
- Chunk boundaries
- Update heat map (which areas update most)
- Material type overlay
- Particle system state

## Minimum Viable Engine

Here's the absolute minimum code to get started:

```python
import pygame
import random

# Constants
WIDTH, HEIGHT = 800, 600
CELL_SIZE = 2
GRID_WIDTH = WIDTH // CELL_SIZE
GRID_HEIGHT = HEIGHT // CELL_SIZE

# Materials
EMPTY = 0
SAND = 1
WATER = 2

# Colors
COLORS = {
    EMPTY: (0, 0, 0),
    SAND: (255, 200, 100),
    WATER: (50, 100, 255)
}

# Initialize grid
grid = [[EMPTY for x in range(GRID_WIDTH)] for y in range(GRID_HEIGHT)]

def update_sand(x, y):
    if y + 1 < GRID_HEIGHT:
        if grid[y + 1][x] == EMPTY:
            grid[y][x], grid[y + 1][x] = EMPTY, SAND
        else:
            # Try diagonal movement
            left = x - 1 >= 0 and grid[y + 1][x - 1] == EMPTY
            right = x + 1 < GRID_WIDTH and grid[y + 1][x + 1] == EMPTY
            
            if left and right:
                if random.random() < 0.5:
                    grid[y][x], grid[y + 1][x - 1] = EMPTY, SAND
                else:
                    grid[y][x], grid[y + 1][x + 1] = EMPTY, SAND
            elif left:
                grid[y][x], grid[y + 1][x - 1] = EMPTY, SAND
            elif right:
                grid[y][x], grid[y + 1][x + 1] = EMPTY, SAND

def update():
    # Update from bottom to top
    for y in range(GRID_HEIGHT - 2, -1, -1):
        for x in range(GRID_WIDTH):
            if grid[y][x] == SAND:
                update_sand(x, y)
            elif grid[y][x] == WATER:
                update_water(x, y)  # Implement this!

def draw(screen):
    for y in range(GRID_HEIGHT):
        for x in range(GRID_WIDTH):
            if grid[y][x] != EMPTY:
                color = COLORS[grid[y][x]]
                rect = (x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE)
                pygame.draw.rect(screen, color, rect)

# Main loop
pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
clock = pygame.time.Clock()
running = True

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif pygame.mouse.get_pressed()[0]:
            # Add sand at mouse position
            mx, my = pygame.mouse.get_pos()
            gx, gy = mx // CELL_SIZE, my // CELL_SIZE
            if 0 <= gx < GRID_WIDTH and 0 <= gy < GRID_HEIGHT:
                grid[gy][gx] = SAND
    
    update()
    screen.fill((0, 0, 0))
    draw(screen)
    pygame.display.flip()
    clock.tick(60)

pygame.quit()
```

## Common Pitfalls to Avoid

### 1. Don't Over-Engineer Early
- ❌ Don't start with 20 material types
- ❌ Don't implement temperature on day 1
- ❌ Don't add particles before basic sand works
- ✅ Get sand falling and piling correctly first
- ✅ Add features only when core is stable

### 2. Don't Ignore Chunk Boundaries
```cpp
// Wrong: Doesn't handle chunk boundaries
void update_pixel(int x, int y) {
    if (grid[y + 1][x] == EMPTY) {
        swap(grid[y][x], grid[y + 1][x]);
    }
}

// Right: Handles cross-chunk updates
void update_pixel(int x, int y) {
    int next_y = y + 1;
    Chunk& current_chunk = get_chunk(x, y);
    Chunk& below_chunk = get_chunk(x, next_y);
    
    if (get_pixel(below_chunk, x, next_y) == EMPTY) {
        swap_pixels(current_chunk, x, y, below_chunk, x, next_y);
        below_chunk.mark_dirty(x, next_y);
    }
}
```

### 3. Don't Simulate Everything
- Use dirty rectangles religiously
- Let stable areas sleep
- This is the difference between 256×256 and full-screen
- Profile before optimizing

### 4. Don't Forget the Game
- The tech is the easy part
- Design around your physics, not despite it
- Embrace the chaos
- Make destruction meaningful to gameplay

### 5. Don't Use Wrong Update Patterns
```cpp
// Wrong: Creates directional bias
for (int x = 0; x < WIDTH; x++) {
    // Always processes left to right
}

// Right: Alternates direction
bool even_frame = frame_count % 2 == 0;
if (even_frame) {
    for (int x = 0; x < WIDTH; x++)
} else {
    for (int x = WIDTH - 1; x >= 0; x--)
}
```

## Advanced Topics

### Temperature System
```cpp
struct Pixel {
    MaterialType material;
    uint8_t temperature;
    
    void update_temperature() {
        // Heat rises
        if (temperature > ambient && y > 0) {
            transfer_heat(x, y - 1);
        }
        
        // State changes
        if (material == WATER && temperature > 100) {
            material = STEAM;
        } else if (material == WATER && temperature < 0) {
            material = ICE;
        }
    }
};
```

### Pressure System
- Track local density
- Water flows from high to low pressure
- Can create realistic water jets
- Adds computational complexity

### Chemical Reactions
```cpp
void check_reactions(int x, int y) {
    Material& current = grid[y][x];
    
    // Check neighbors for reactions
    for (auto [dx, dy] : neighbors) {
        Material& neighbor = grid[y + dy][x + dx];
        
        if (current == FIRE && neighbor == WOOD) {
            neighbor = FIRE;
            current.lifetime--;
        } else if (current == WATER && neighbor == LAVA) {
            current = STEAM;
            neighbor = STONE;
        }
    }
}
```

### Electricity Simulation
- Use breadth-first search for conductivity
- Track power sources and sinks
- Water conducts when player is in it
- Great for puzzle mechanics

## Performance Optimization

### Profiling Checklist
1. **Measure first**: Use actual profiler data
2. **Chunk updates**: Are you updating empty chunks?
3. **Dirty rectangles**: Are they too large?
4. **Cache misses**: Is your data layout efficient?
5. **Branch prediction**: Minimize unpredictable branches

### Memory Layout Optimization
```cpp
// Bad: Array of structures (AoS)
struct Pixel {
    uint8_t material;
    uint8_t temperature;
    uint8_t lifetime;
    uint8_t state;
};
Pixel grid[HEIGHT][WIDTH];  // 4 bytes per pixel

// Good: Structure of arrays (SoA) - better cache usage
uint8_t materials[HEIGHT][WIDTH];      // 1 byte per pixel
uint8_t temperatures[HEIGHT][WIDTH];   // Only when needed
```

### SIMD Optimization Opportunities
- Parallel comparison for empty cells
- Bulk memory copies for particle movement
- Temperature diffusion calculations

### GPU Compute Considerations
- Falling sand is inherently sequential
- GPU works better for:
  - Rendering
  - Temperature/pressure diffusion
  - Particle systems
  - Post-processing effects

## Resources and References

### Essential Reading
- **Noita GDC Talk**: The original source of inspiration
- **Dwarf Fortress Liquid Simulation**: Complex water pressure
- **Powder Toy Source Code**: Open source falling sand game
- **Box2D Documentation**: For rigid body integration

### Useful Libraries
- **Box2D**: 2D physics engine for rigid bodies
- **EnTT**: Entity-component system for game objects
- **Tracy Profiler**: Real-time profiling for optimization
- **ImGui**: Debug interface for development

### Community Resources
- **r/cellular_automata**: Related simulation techniques
- **Falling Sand Game Discord**: Community of developers
- **GitHub - awesome-falling-sand**: Curated list of engines

### Mathematics and Algorithms
- **Marching Squares**: Contour extraction algorithm
- **Douglas-Peucker**: Line simplification algorithm
- **Delaunay Triangulation**: For mesh generation
- **Bresenham's Line**: Efficient line drawing

## Final Thoughts

Building a falling sand engine is a journey. Noita took 6-7 years to develop - be patient with your progress. Start simple, iterate constantly, and always prioritize what looks good over what's "correct."

Remember: The magic isn't in complex physics equations. It's in simple rules creating emergent complexity that surprises even you, the creator.

Happy simulating!