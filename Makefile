CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Imodules -pthread
SRCDIR = src
MODULEDIR = modules
BUILDDIR = build
TARGET = $(BUILDDIR)/funhouse
TWITCH_TEST_TARGET = $(BUILDDIR)/twitch_test
TWITCH_EXAMPLE_TARGET = $(BUILDDIR)/twitch_integration_example

# SDL2 and OpenGL flags
SDL2_CFLAGS = $(shell sdl2-config --cflags)
SDL2_LIBS = $(shell sdl2-config --libs)
OPENGL_LIBS = -lGL -lGLEW

CXXFLAGS += $(SDL2_CFLAGS)
LDFLAGS = $(SDL2_LIBS) $(OPENGL_LIBS) -pthread

# Find all source files
MAIN_SOURCES = $(filter-out $(SRCDIR)/twitch_test.cpp, $(wildcard $(SRCDIR)/*.cpp))
TWITCH_TEST_SOURCE = $(SRCDIR)/twitch_test.cpp
MODULE_SOURCES = $(wildcard $(MODULEDIR)/*/[!.]*.cpp)

# Generate object files
MAIN_OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(MAIN_SOURCES))
MODULE_OBJECTS = $(patsubst $(MODULEDIR)/%.cpp,$(BUILDDIR)/modules/%.o,$(MODULE_SOURCES))
TWITCH_TEST_OBJECT = $(BUILDDIR)/twitch_test.o

# Test configuration
TESTDIR = tests
TEST_TARGET = $(BUILDDIR)/test_runner
TEST_SOURCES = $(wildcard $(TESTDIR)/*.cpp $(TESTDIR)/*/*.cpp)
TEST_OBJECTS = $(patsubst $(TESTDIR)/%.cpp,$(BUILDDIR)/tests/%.o,$(TEST_SOURCES))

# Test-specific modules (only what's needed for testing)
TEST_MODULE_SOURCES = $(wildcard $(MODULEDIR)/input/*.cpp $(MODULEDIR)/world/*.cpp $(MODULEDIR)/twitch/*.cpp)
TEST_MODULE_OBJECTS = $(patsubst $(MODULEDIR)/%.cpp,$(BUILDDIR)/modules/%.o,$(TEST_MODULE_SOURCES))

all: $(TARGET)

# Main application target
$(TARGET): $(MAIN_OBJECTS) $(MODULE_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Twitch test target (only requires twitch module)
twitch-test: $(BUILDDIR)/twitch_test
$(BUILDDIR)/twitch_test: $(TWITCH_TEST_OBJECT) $(BUILDDIR)/modules/twitch/TwitchIrcClient.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -pthread

# Twitch integration example
twitch-example: $(TWITCH_EXAMPLE_TARGET)
$(TWITCH_EXAMPLE_TARGET): $(BUILDDIR)/twitch_integration_example.o $(BUILDDIR)/modules/input/InputSystem.o $(BUILDDIR)/modules/input/InputManager.o $(BUILDDIR)/modules/input/InputContext.o $(BUILDDIR)/modules/input/InputContextManager.o $(BUILDDIR)/modules/world/World.o $(BUILDDIR)/modules/twitch/TwitchIrcClient.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/twitch_integration_example.o: examples/twitch_integration_example.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Test targets
test: $(TEST_TARGET)
$(TEST_TARGET): $(TEST_OBJECTS) $(TEST_MODULE_OBJECTS) $(BUILDDIR)/tests/external/catch_amalgamated.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR)/modules/%.o: $(MODULEDIR)/%.cpp | $(BUILDDIR)/modules
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR)/tests/%.o: $(TESTDIR)/%.cpp | $(BUILDDIR)/tests
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR)/tests/external/catch_amalgamated.o: $(TESTDIR)/external/catch_amalgamated.cpp | $(BUILDDIR)/tests/external
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/modules:
	mkdir -p $(BUILDDIR)/modules

$(BUILDDIR)/tests:
	mkdir -p $(BUILDDIR)/tests

$(BUILDDIR)/tests/external:
	mkdir -p $(BUILDDIR)/tests/external

clean:
	rm -rf $(BUILDDIR)

run: $(TARGET)
	./$(TARGET)

run-twitch-test: twitch-test
	./$(BUILDDIR)/twitch_test

run-twitch-example: twitch-example
	./$(TWITCH_EXAMPLE_TARGET)

run-tests: test
	./$(TEST_TARGET)

test-verbose: test
	./$(TEST_TARGET) -v high

.PHONY: all clean run twitch-test run-twitch-test twitch-example run-twitch-example test run-tests test-verbose