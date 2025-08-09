# ==============================================================================
# Makefile for Compiling and Linking C Projects with GCC
# ==============================================================================
# This Makefile automates the build process, including compiling source files
# into object files and then linking them into a final executable.
# It supports multiple source directories, include paths, library paths,
# and custom GCC flags.
#
# To use this Makefile, simply run 'make' in your terminal.
# To clean up the build directory, run 'make clean'.
#
# Command-line examples:
# - To compile with a different output name:
#   make OUTPUT_EXECUTABLE_NAME="my_app"
# - To add more include paths and libraries:
#   make INCLUDE_PATHS="./lib/include" LIBRARIES="glfw3,glfw3dll"
#
# Note: Variables set on the command line override the defaults below.
# ==============================================================================

# --- CONFIGURE YOUR BUILD HERE ---

# The C compiler to use. Assuming 'gcc' is in your system's PATH.
CC = gcc

# The main directory containing your C source files.
SOURCE_DIR = ./engine

# An optional second directory for source files, if needed.
# Leave it empty if you only have one source directory.
ADDITIONAL_SOURCE_DIR = 

# The name of the final executable file.
OUTPUT_EXECUTABLE_NAME = app.exe

# Comma-separated list of include directories.
# Example: -I./lib/include -I./src
INCLUDE_PATHS = ./Libraries/include

# Comma-separated list of library directories.
# Example: -L./lib/static
LIBRARY_PATHS = ./Libraries/lib

# Comma-separated list of library names to link.
# Example: -lglfw3 -lglfw3dll
LIBRARIES = glfw3 glfw3dll

# Custom GCC flags for both compilation and linking.
# Example: -O1 -Wextra -std=c99
GCC_FLAGS = -O1 -Wextra -std=c99 -Wall -pedantic -Wl,--verbose

# --- INTERNAL VARIABLES (DO NOT MODIFY) ---

# The directory where object files will be stored.
BUILD_DIR = ./Build

# VPATH is a special Makefile variable that tells it where to look for source files.
# This makes it possible to have a single object file rule.
VPATH = $(SOURCE_DIR):$(ADDITIONAL_SOURCE_DIR)

# Split the comma-separated lists into space-separated lists for GCC.
# The `-I` and `-L` flags are automatically added to each path.
INCLUDE_FLAGS = $(foreach dir,$(INCLUDE_PATHS),-I$(dir))
LIBRARY_PATH_FLAGS = $(foreach dir,$(LIBRARY_PATHS),-L$(dir))
LIBRARY_FLAGS = $(foreach lib,$(LIBRARIES),-l$(lib))

# Find all C source files in both source directories.
SOURCES = $(shell find $(SOURCE_DIR) $(ADDITIONAL_SOURCE_DIR) -name "*.c")

# Generate a list of object files (.o) from the source files.
# This replaces the source file's extension with .o and puts them in the build directory.
OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SOURCES)))

# ==============================================================================
# --- MAKEFILE TARGETS ---
# ==============================================================================

.PHONY: all clean run

# Default target. It depends on the final executable.
all: $(BUILD_DIR)/$(OUTPUT_EXECUTABLE_NAME)

# Target for the final executable.
# It depends on all the object files.
$(BUILD_DIR)/$(OUTPUT_EXECUTABLE_NAME): $(OBJECTS)
	@echo "----------------------------------------"
	@echo "Starting Linking Phase..."
	@echo "Command: $(CC) $^ $(LIBRARY_PATH_FLAGS) $(LIBRARY_FLAGS) $(GCC_FLAGS) -o $@ -L$(LIBRARY_PATH_FLAGS) -l$(LIBRARY_FLAGS)"
	@mkdir -p $(@D)
	$(CC) $(GCC_FLAGS) $^ $(LIBRARY_PATH_FLAGS) $(LIBRARY_FLAGS) -o $@
	@echo "----------------------------------------"
	@echo "Successfully linked $(notdir $@)!"

# Pattern rule for compiling C files into object files.
# This rule is run for every .o file that is older than its corresponding .c file.
$(BUILD_DIR)/%.o: %.c
	@echo "----------------------------------------"
	@echo "Compiling $< to $(notdir $@)..."
	@echo "Command: $(CC) -c $< $(INCLUDE_FLAGS) $(GCC_FLAGS) -o $@"
	@mkdir -p $(@D)
	$(CC) -c $< $(INCLUDE_FLAGS) $(GCC_FLAGS) -o $@
	@echo "----------------------------------------"

# The 'clean' target removes the build directory and all its contents.
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)

# The 'run' target executes the compiled program.
run: all
	@echo "Running $(BUILD_DIR)/$(OUTPUT_EXECUTABLE_NAME)..."
	@$(BUILD_DIR)/$(OUTPUT_EXECUTABLE_NAME)
