# Physics Module

## Purpose
Handles the falling sand simulation physics including:
- Material movement rules
- Particle interactions
- Density-based displacement
- Collision detection

## Key Components
- Physics update loop
- Material behavior rules
- Particle state management

## Core Concepts
- **Single Buffer**: No double buffering for better performance
- **Update Order**: Bottom-to-top for falling materials
- **Randomized Bias**: Alternating left/right to avoid artifacts

## Design Decisions
- Materials update based on simple neighbor checking
- Density determines which materials can displace others
- Each material type has specific movement patterns:
  - Sand: Falls down, then diagonally
  - Water: Falls down, diagonally, then horizontally
  - Gas: Inverted gravity (rises)