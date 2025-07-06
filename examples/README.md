# Funhouse Examples

This directory contains example programs demonstrating various features of the Funhouse engine.

## Available Examples

### Twitch Integration Example

Demonstrates how to integrate Twitch chat commands with the Funhouse input system.

**Build and run:**
```bash
make twitch-example
make run-twitch-example
```

**Features demonstrated:**
- Enabling Twitch integration through InputManager
- Registering custom Twitch commands
- Processing Twitch commands through the InputCommand system
- Thread-safe command polling and execution

**Available commands:**
- `!sand`, `!water`, `!stone`, `!air` - Select materials
- `!clear` - Clear the world
- `!brush [1-50]` - Set brush size
- `!spawn [material] [x] [y]` - Spawn material at position
- `!gravity [value]` - Custom gravity command (example)
- `!color [name]` - Custom color command (example)
- `!rain [type]` - Make it rain!
- `!help` - Show available commands

## Creating New Examples

When adding new examples:

1. Create a new `.cpp` file in this directory
2. Add build rules to the Makefile
3. Update this README with build instructions
4. Follow the existing examples for structure and style

## Example Structure

Examples should:
- Be self-contained and runnable
- Demonstrate specific features clearly
- Include comments explaining what's happening
- Handle errors gracefully
- Clean up resources properly