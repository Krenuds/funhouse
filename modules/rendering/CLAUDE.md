# Rendering Module

## Purpose
Handles all graphics rendering including:
- Pixel buffer to GPU upload
- Shader management
- Post-processing effects
- Debug overlays

## Key Components
- `Renderer`: Main rendering class
- `PixelBuffer`: CPU-side pixel storage
- Shader programs for pixel rendering
- Texture atlas system

## Rendering Pipeline
1. Update pixel buffer from world state
2. Upload changed regions to GPU
3. Render textured quads for each pixel
4. Apply post-processing (lighting, blur, etc.)

## Design Decisions
- Use instanced rendering for performance
- Texture atlas for material visuals
- Dirty rectangle tracking to minimize GPU uploads
- OpenGL 3.3 core profile for compatibility