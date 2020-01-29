
MK_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

COMPILER := clang

BUILD_DIR := build

# Source files.
SRC_DIR := src
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp) \
			 $(wildcard $(SRC_DIR)/sdl/*.cpp) \
			 $(wildcard $(SRC_DIR)/opengl/*.cpp) \
			 $(wildcard $(SRC_DIR)/lbm/*.cpp)



OPTIMISE_FLAGS = -O3 -flto -g3
# OPTIMISE_FLAGS = -O0 -g3

COMPILER_FLAGS = -std=c++11 $(OPTIMISE_FLAGS) -MD -Wall `sdl2-config --cflags`


LINKER_FLAGS = `sdl2-config --cflags --libs` -lGL -lm -lstdc++ $(OPTIMISE_FLAGS)


MAIN_OBJ_FILES := $(patsubst ${SRC_DIR}/%.shader, ${BUILD_DIR}/main/%.o, \
					$(patsubst ${SRC_DIR}/%.cpp, ${BUILD_DIR}/main/%.o, $(SRC_FILES)))


main: $(MAIN_OBJ_FILES)
	@echo 'Linking main...'
	@echo $(LINKER_FLAGS)
	@$(COMPILER) -o $(BUILD_DIR)/$@.o $^ $(LINKER_FLAGS)
	@echo ' '
	@echo 'Finished making main!'

$(BUILD_DIR)/main/%.o: $(SRC_DIR)/%.cpp
	@echo 'Building $(patsubst ${SRC_DIR}/%,%,$<).'
	@mkdir -p ${@D}
	$(COMPILER) $< -o $@ -c $(COMPILER_FLAGS)
	@echo ' '


run:
	make main -j16
	@echo ' '
	@$(BUILD_DIR)/main.o


clean:
	rm -rf "$(BUILD_DIR)/main"
	rm -f "$(BUILD_DIR)/main.o"
