CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Imodules
SRCDIR = src
MODULEDIR = modules
BUILDDIR = build
TARGET = $(BUILDDIR)/funhouse

# SDL2 and OpenGL flags
SDL2_CFLAGS = $(shell sdl2-config --cflags)
SDL2_LIBS = $(shell sdl2-config --libs)
OPENGL_LIBS = -lGL -lGLEW

CXXFLAGS += $(SDL2_CFLAGS)
LDFLAGS = $(SDL2_LIBS) $(OPENGL_LIBS)

# Find all source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
MODULE_SOURCES = $(wildcard $(MODULEDIR)/*/[!.]*.cpp)
ALL_SOURCES = $(SOURCES) $(MODULE_SOURCES)

# Generate object files
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
MODULE_OBJECTS = $(patsubst $(MODULEDIR)/%.cpp,$(BUILDDIR)/modules/%.o,$(MODULE_SOURCES))
ALL_OBJECTS = $(OBJECTS) $(MODULE_OBJECTS)

all: $(TARGET)

$(TARGET): $(ALL_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

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

.PHONY: all clean run