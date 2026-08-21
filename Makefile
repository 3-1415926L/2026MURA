CXX = g++
CXXFLAGS = -std=c++20 -O3 -Isrc/Headers

PROGRAM_SOURCES := $(wildcard src/TerminalPrograms/*.cc)
COMMON_SOURCES := src/stb_image_impl.cc

TARGETS := $(patsubst src/TerminalPrograms/%.cc,%,$(PROGRAM_SOURCES))

all: $(TARGETS)

%: src/TerminalPrograms/%.cc $(COMMON_SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGETS)

.PHONY: all clean