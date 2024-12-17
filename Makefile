# compiler settings
CXX = g++
CXXFLAGS = -Wall -std=c++20 $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)

# source files
SRCS = src/*.cpp
BUILD = build/
TARGET = $(BUILD)/raycaster

# build rules
all: $(TARGET)

$(TARGET): $(SRCS)
	mkdir -p build && $(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# run the application
run: $(TARGET)
	./$(TARGET)

# clean build files
clean:
	rm -rf $(BUILD)

.PHONY: all run clean
