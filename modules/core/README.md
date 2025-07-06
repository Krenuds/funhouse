# Core Module

Foundation systems and utilities for the Funhouse engine.

## Submodules

### memory/
Custom memory management for performance-critical systems:
- Pool allocators for particles and chunks
- Stack allocators for temporary data
- Aligned allocation for SIMD operations

### containers/
Specialized data structures optimized for falling sand simulation:
- Spatial hash maps for efficient neighbor queries
- Chunk arrays with fast iteration
- Ring buffers for particle systems

### math/
Mathematical utilities and operations:
- 2D vector operations
- Fast integer math
- Random number generation
- SIMD implementations where beneficial

## Design Principles

- Zero-overhead abstractions
- Cache-friendly data layouts  
- Minimal dynamic allocation
- Compile-time optimization where possible