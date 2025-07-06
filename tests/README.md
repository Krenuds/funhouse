# Unit Testing for Funhouse

This project now includes a comprehensive unit testing framework using **Catch2 v3.4.0**.

## Overview

The testing framework is set up to test the input module and can be easily extended to test other modules in the future.

## Structure

```
tests/
├── external/                   # Third-party testing dependencies
│   ├── catch_amalgamated.hpp   # Catch2 header
│   ├── catch_amalgamated.cpp   # Catch2 implementation
│   └── README.md               # Info about external dependencies
├── input/                      # Input module tests
│   ├── test_input_command.cpp   # Tests for InputCommand base class
│   ├── test_input_system.cpp    # Tests for InputSystem
│   ├── test_keyboard_commands.cpp # Tests for keyboard commands
│   └── test_mouse_commands.cpp   # Tests for mouse commands
└── test_main.cpp               # Test runner main function
```

## Building and Running Tests

### Build the tests:
```bash
make test
```

### Run all tests:
```bash
make run-tests
```

### Run tests with verbose output:
```bash
make test-verbose
```

### Manual execution with Catch2 options:
```bash
./build/test_runner --help          # Show all options
./build/test_runner -t InputSystem  # Run only InputSystem tests
./build/test_runner -v high         # High verbosity
./build/test_runner --success       # Show successful tests too
```

## Current Test Coverage

### InputCommand Base Class
- Command creation and execution
- Timestamp functionality
- Command cloning
- Replayability settings
- Undo functionality (default no-op)

### InputSystem
- Creation and initialization
- Mouse state tracking
- Keyboard state tracking
- Event processing
- Command factory registration

### Mouse Commands
- PlaceMaterialCommand: Place materials in world
- RemoveMaterialCommand: Remove materials (set to Air)
- Command inheritance and base functionality
- Null pointer handling

### Keyboard Commands
- SelectMaterialCommand: Material selection with callbacks
- ToggleRecordingCommand: Recording state management
- ClearWorldCommand: World clearing functionality
- Callback handling and null safety

## Test Features

- **Mocking**: Tests use real World instances rather than complex mocks for simplicity
- **Edge Cases**: All tests include null pointer and error condition handling
- **Behavior Verification**: Tests verify actual behavior rather than just method calls
- **Comprehensive Coverage**: Tests cover normal operation, edge cases, and inheritance

## Adding New Tests

### For a new module:
1. Create a directory under `tests/` for your module
2. Add test files with descriptive names
3. Update the Makefile to include any necessary source files
4. Follow the existing pattern of test organization

### Test file structure:
```cpp
#include "../external/catch_amalgamated.hpp"
#include "../../modules/your_module/YourClass.h"

TEST_CASE("YourClass basic functionality", "[YourClass]") {
    SECTION("Can be created") {
        auto instance = std::make_unique<YourClass>();
        REQUIRE(instance != nullptr);
    }
    
    // More sections...
}
```

### Running specific tests:
- Use tags: `./build/test_runner -t [YourClass]`
- Use test names: `./build/test_runner "YourClass basic functionality"`
- Use sections: `./build/test_runner "YourClass basic functionality" -c "Can be created"`

## Best Practices

1. **Use descriptive test names** that explain what is being tested
2. **Group related tests** using `SECTION` within test cases
3. **Test both success and failure paths**
4. **Include edge cases** like null pointers, invalid inputs
5. **Keep tests independent** - each test should set up its own state
6. **Use appropriate assertions** (`REQUIRE` for critical, `CHECK` for non-fatal)

## Dependencies

- **Catch2 v3.4.0**: Header-only testing framework
- **SDL2**: Required for input system tests
- **OpenGL/GLEW**: Required for building (inherited from main project)

The test framework is self-contained and downloads Catch2 automatically during setup.
