# Directories
BUILD_DIR := build

# Compiler and flags
CC := gcc  # Use `gcc` explicitly for MinGW
CFLAGS := -Wall -Wextra
CPPFLAGS := -I include -I lib/sc_map
LDFLAGS := -lncurses

# Source files
SRCS := src/sltool.c src/setlist.c src/songlist.c src/utils.c lib/sc_map/sc_map.c
OBJS := $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Executable
EXECUTABLE := sltool

# Determine the platform
ifeq ($(OS),Windows_NT)
    # If the OS is Windows, add the path to MinGW ncurses include directory
    CPPFLAGS += -I/mingw64/include/ncurses
    # Add the -DNCURSES_STATIC flag
    CFLAGS += -DNCURSES_STATIC
	# Add the -static flag
	LDFLAGS += -static
endif

# Targets
.PHONY: all clean

all: $(BUILD_DIR) $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE)
