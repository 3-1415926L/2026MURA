CXX = g++
CXXFLAGS = -std=c++20 -O3 -Isrc/Headers

PROGRAM_SOURCES := $(wildcard src/TerminalPrograms/*.cc)
COMMON_SOURCES := src/stb_image_impl.cc

TARGETS := $(patsubst src/TerminalPrograms/%.cc,%,$(PROGRAM_SOURCES))

all: $(TARGETS)


%: src/TerminalPrograms/%.cc $(COMMON_SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@


# Build the Qt GUI using CMake
gui:
	cmake -S . -B build-gui -DBUILD_GUI=ON
	cmake --build build-gui


clean:
	rm -f $(TARGETS)
	rm -f main_Qt.exe
	rm -rf build-gui


.PHONY: all gui clean