# Simulation Module

Core falling sand simulation logic for the Funhouse engine.

## Submodules

### falling_sand/
Core pixel simulation rules and update logic. Implements the single-buffer approach with bottom-to-top update order.

### chunk_system/
Manages the world as 64×64 pixel chunks with dirty rectangle tracking. Handles multi-threaded updates using 4-pass checker pattern.

### materials/
Defines all material types and their properties. Each material has unique behavior rules for movement, state changes, and interactions.

### particles/
High-velocity particle system for effects like explosions and splashing liquids. Particles convert back to pixels on collision.

## Key Concepts

- **Single Buffer**: No double buffering for better performance
- **Update Order**: Bottom-to-top for falling materials, top-to-bottom for rising
- **Dirty Tracking**: Only update chunks that have changed
- **Material Rules**: Simple rules create emergent complexity