# Build Module Command

Build and test the $ARGUMENTS module:

1. Navigate to the module directory
2. Compile all source files in the module
3. Run any unit tests for the module
4. Report compilation errors or warnings
5. Show test results summary

Use appropriate compiler flags for the falling sand engine:
- Enable all warnings (-Wall -Wextra)
- Use optimization level -O2
- Enable C++17 standard
- Link required libraries (OpenGL, SDL2, Box2D as needed)