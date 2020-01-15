
MK_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

COMPILER := clang
#COMPILER := clang++ -fsanitize=undefined,unsigned-integer-overflow,float-divide-by-zero,implicit-conversion -fsanitize-trap=undefined,unsigned-integer-overflow,float-divide-by-zero,implicit-conversion
#COMPILER := clang++ -O1 -fsanitize=address -fno-omit-frame-pointer

BUILD_DIR := build

# Source files.
SRC_DIR := src
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp) \
			 $(wildcard $(SRC_DIR)/sdl/*.cpp) \
			 $(wildcard $(SRC_DIR)/opengl/*.cpp) \
			 $(wildcard $(SRC_DIR)/lbm/*.cpp)



#OPTIMISE_FLAGS = -O3 -flto -g3
OPTIMISE_FLAGS = -O0 -g3

COMPILER_FLAGS = -std=c++14 $(OPTIMISE_FLAGS) -MD \
				-Wall \
				-I"$(SRC_DIR)/overture/include" \
				`sdl2-config --cflags`


LINKER_FLAGS = `sdl2-config --cflags --libs` \
				-lSDL2_image -lGL $(OPTIMISE_FLAGS)
LINKER_FLAGS_EXTRA = -lm -lstdc++


SDL_FLAGS := ""
ifeq ($(detected_OS),Darwin)  # Mac OS X
	SDL_FLAGS += --without-x
endif


MAIN_OBJ_FILES := $(patsubst ${SRC_DIR}/%.shader, ${BUILD_DIR}/main/%.o, \
					$(patsubst ${SRC_DIR}/%.cpp, ${BUILD_DIR}/main/%.o, $(SRC_FILES)))

MAIN_OBJ_FILES_EM := $(patsubst ${SRC_DIR}/%.cpp, ${BUILD_DIR}/em/%.em.o, $(SRC_FILES))


main: $(MAIN_OBJ_FILES)
	@echo 'Linking main...'
	@echo $(LINKER_FLAGS) $(LINKER_FLAGS_EXTRA)
	@$(COMPILER) -o $(BUILD_DIR)/$@.o $^ $(LINKER_FLAGS) $(LINKER_FLAGS_EXTRA)
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
