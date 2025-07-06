# World Module

## Purpose
Manages the game world including:
- Pixel storage and access
- Chunk system for optimization
- World streaming and persistence
- Dirty rectangle tracking

## Key Components
- `World`: Main world class containing all pixels
- Chunk management (64×64 pixel chunks)
- Spatial queries and updates

## Design Decisions
- World divided into chunks for performance
- Only dirty chunks get updated each frame
- Keeps 12 512×512 areas in memory, streams rest to disk
- Single array storage for cache efficiency

## Optimization Strategies
- Dirty tracking per chunk
- Spatial hashing for quick lookups
- Multi-threading uses 4-pass checker pattern