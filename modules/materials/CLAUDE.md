# Materials Module

## Purpose
Defines all material types and their properties:
- Material behavior rules
- Physical properties (density, state, temperature)
- Visual properties (color, texture)
- Interactions between materials

## Key Components
- `MaterialType`: Enum of all material types
- `MaterialProperties`: Struct with material attributes
- Material interaction rules

## Material Categories
- **Solids**: Stone, metal, ice
- **Powders**: Sand, dirt, snow
- **Liquids**: Water, oil, lava
- **Gases**: Steam, smoke, fire

## Design Decisions
- Materials stored as simple enum for performance
- Properties looked up from static table
- Complex state (temperature, pressure) stored separately when needed