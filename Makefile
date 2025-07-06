CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Imodules -pthread
SRCDIR = src
MODULEDIR = modules
BUILDDIR = build
TARGET = $(BUILDDIR)/funhouse
TWITCH_TEST_TARGET = $(BUILDDIR)/twitch_test

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

all: $(TARGET)

# Main application target
$(TARGET): $(MAIN_OBJECTS) $(MODULE_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Twitch test target (only requires twitch module)
twitch-test: $(BUILDDIR)/twitch_test
$(BUILDDIR)/twitch_test: $(TWITCH_TEST_OBJECT) $(BUILDDIR)/modules/twitch/TwitchIrcClient.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -pthread

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR)/modules/%.o: $(MODULEDIR)/%.cpp | $(BUILDDIR)/modules
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/modules:
	mkdir -p $(BUILDDIR)/modules

clean:
	rm -rf $(BUILDDIR)

run: $(TARGET)
	./$(TARGET)

run-twitch-test: twitch-test
	./$(BUILDDIR)/twitch_test

.PHONY: all clean run twitch-test run-twitch-test