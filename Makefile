SOURCE_ROOT := src
BUILD_ROOT  := build
MANDATORY_CXXFLAGS := -std=c++14 -I/usr/include/SDL2 -I/usr/include/GL -Wall -Wpedantic -O3
MANDATORY_LDFLAGS := -lSDL2 -lGLEW -lGL -pthread

sources := $(shell find $(SOURCE_ROOT) -name '*.cpp')
objects := $(patsubst $(SOURCE_ROOT)/%,$(BUILD_ROOT)/%.o,$(sources))
dependency_lists := $(objects:.o=.d)
build_tree := $(sort $(dir $(objects)))

.SECONDARY: $(objects)

main: $(objects)
	$(CXX) -o $@ $^ $(MANDATORY_LDFLAGS)

$(build_tree): 
	mkdir -p $(build_tree)

$(BUILD_ROOT)/%.cpp.o: $(SOURCE_ROOT)/%.cpp | $(build_tree)
	$(CXX) $(MANDATORY_CXXFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_ROOT)/%.cpp.d: $(SOURCE_ROOT)/%.cpp | $(build_tree)
	$(CXX) $(MANDATORY_CXXFLAGS) -MM -MP -MQ $(BUILD_ROOT)/$*.cpp.o -MF $@ $<

-include $(dependency_lists)

.PHONY: clean distclean

clean:
	rm -rf $(BUILD_ROOT)
distclean: clean
	rm main
