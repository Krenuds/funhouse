# Module Organization

Each module in this project is self-contained with its own CLAUDE.md file that documents:
- Purpose and responsibilities
- Key components
- Design decisions
- Dependencies

## Current Modules

### core/
Application lifecycle, window management, and main game loop.

### physics/
Falling sand simulation rules and particle interactions.

### world/
World storage, chunk system, and spatial management.

### materials/
Material definitions, properties, and interaction rules.

### rendering/
Graphics rendering, pixel buffer management, and shaders.

### input/
User input handling for keyboard, mouse, and controllers.

## Module Guidelines

1. Each module should have minimal dependencies on other modules
2. Use interfaces/abstract classes for inter-module communication
3. Keep module-specific implementation details private
4. Document all public APIs in the module's CLAUDE.md