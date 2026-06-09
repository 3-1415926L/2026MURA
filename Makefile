CXX = g++
CXXFLAGS = -std=c++20 -IHeaders

PROGRAM_SOURCES := $(wildcard Programs/*.cc)
COMMON_SOURCES := stb_image/stb_image_impl.cc

TARGETS := $(patsubst Programs/%.cc,%,$(PROGRAM_SOURCES))

all: $(TARGETS)

%: Programs/%.cc $(COMMON_SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGETS)

.PHONY: all clean