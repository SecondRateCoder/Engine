#!/bin/bash
# A script to compile and link C source files using GCC in a Linux environment.
# It handles source directories, include paths, library paths, and logging.

# --- Configuration & Logging Setup ---

# Set -e to exit immediately if a command exits with a non-zero status.
set -e

# Define GCC path. Assuming 'gcc' is in the system's PATH.
GCC_PATH="gcc"

# Define the build and log directories.
BUILD_DIR="./Build"
LOG_DIR="./Resources/Logs"

# Create a unique log file name with a timestamp.
LOG_FILE="$LOG_DIR/log_$(date +'%Y-%m-%d_%H-%M-%S').txt"

# This function writes a message to both the console (standard output) and the log file.
# It is a direct replacement for the Write-Log PowerShell function.
function write_log {
    local message="$1"
    echo "$message" | tee -a "$LOG_FILE"
}

# --- Command-line Argument Parsing ---
# This section uses Bash's getopts or manual parsing to handle script parameters.
# Since your PowerShell script uses a param block, we'll manually parse arguments
# to replicate that behavior.

# Default values for optional parameters
SOURCE_DIR=""
ADDITIONAL_SOURCE_DIR=""
OUTPUT_EXECUTABLE_NAME=""
INCLUDE_PATHS=()
LIBRARY_PATHS=()
LIBRARIES=()
GCC_FLAGS=()

# Function to display help menu
function display_help {
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  --source-dir <path>          Mandatory. Directory containing C source files."
    echo "  --additional-source-dir <path> Optional. Another directory for C source files."
    echo "  --output-executable-name <name> Mandatory. The name of the final executable (e.g., 'app.exe')."
    echo "  --include-paths <paths>      Optional. Comma-separated list of include directories."
    echo "  --library-paths <paths>      Optional. Comma-separated list of library directories."
    echo "  --libraries <libs>           Optional. Comma-separated list of library names (e.g., 'glfw3,glfw3dll')."
    echo "  --gcc-flags <flags>          Optional. Comma-separated list of additional GCC flags (e.g., '-O1,-Wall')."
    echo "  --help                       Show this help message and exit."
}

# Parse command-line arguments.
while [ "$#" -gt 0 ]; do
    case "$1" in
        --source-dir)
            SOURCE_DIR="$2"
            shift 2
            ;;
        --additional-source-dir)
            ADDITIONAL_SOURCE_DIR="$2"
            shift 2
            ;;
        --output-executable-name)
            OUTPUT_EXECUTABLE_NAME="$2"
            shift 2
            ;;
        --include-paths)
            IFS=',' read -ra INCLUDE_PATHS <<< "$2"
            shift 2
            ;;
        --library-paths)
            IFS=',' read -ra LIBRARY_PATHS <<< "$2"
            shift 2
            ;;
        --libraries)
            IFS=',' read -ra LIBRARIES <<< "$2"
            shift 2
            ;;
        --gcc-flags)
            IFS=',' read -ra GCC_FLAGS <<< "$2"
            shift 2
            ;;
        --help)
            display_help
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            display_help
            exit 1
            ;;
    esac
done

# Check for mandatory parameters
if [ -z "$SOURCE_DIR" ] || [ -z "$OUTPUT_EXECUTABLE_NAME" ]; then
    echo "Error: Missing mandatory arguments."
    display_help
    exit 1
fi

# --- Functions for Compilation and Linking ---

# Function to check for GCC availability.
function test_gcc_availability {
    write_log "Checking for GCC availability..."
    if command -v "$GCC_PATH" &>/dev/null; then
        write_log "GCC found: $($GCC_PATH --version | head -n 1)"
        return 0 # Success
    else
        write_log "GCC not found. Please ensure GCC is installed and in your system's PATH."
        return 1 # Failure
    fi
}

# Function to compile a single C file.
function c_compile {
    local file_path="$1"
    local build_dir="$2"
    local gcc_executable="$3"
    local include_dirs=("$4")
    local custom_flags=("$5")
    
    local file_name=$(basename "$file_path" .c)
    local output_object_file="$build_dir/$file_name.o"

    write_log "Compiling $file_path to $output_object_file..."

    local arguments=("-c" "$file_path")

    for dir in "${include_dirs[@]}"; do
        if [ -d "$dir" ]; then
            arguments+=("-I$dir")
        else
            write_log "WARNING: Include path not found or is not a directory: '$dir'. Skipping."
        fi
    done

    # Add custom GCC flags to the arguments.
    arguments+=("${custom_flags[@]}")
    
    arguments+=("-o" "$output_object_file")

    write_log "Command: $gcc_executable ${arguments[*]}"

    # Execute GCC and capture all output to a temporary log file.
    local temp_log_file
    temp_log_file=$(mktemp)
    if ! "$gcc_executable" "${arguments[@]}" &> "$temp_log_file"; then
        # If compilation fails, log the contents of the temp file and return an error.
        write_log "FAILED: Failed to compile $file_path."
        write_log "$(cat "$temp_log_file")"
        rm "$temp_log_file"
        return 1
    fi
    # If successful, log the output and remove the temp file.
    write_log "$(cat "$temp_log_file")"
    rm "$temp_log_file"

    write_log "Successfully compiled $file_path"
    return 0 # Success
}

# Function to link object files.
function obj_link {
    local build_dir="$1"
    local output_name="$2"
    local gcc_executable="$3"
    local library_dirs=("$4")
    local library_names=("$5")
    local custom_flags=("$6")

    # Find all object files in the build directory.
    local object_files
    object_files=$(find "$build_dir" -name "*.o")

    if [ -z "$object_files" ]; then
        write_log "No .o files found in '$build_dir' to link."
        return 1
    fi

    local output_executable_path="$build_dir/$output_name"
    
    local arguments=()

    # Add all object files to the arguments.
    arguments+=($object_files)

    for dir in "${library_dirs[@]}"; do
        if [ -d "$dir" ]; then
            arguments+=("-L$dir")
        else
            write_log "WARNING: Library path not found or is not a directory: '$dir'. Skipping."
        fi
    done

    for lib in "${library_names[@]}"; do
        if [ ! -z "$lib" ]; then
            arguments+=("-l$lib")
        else
            write_log "WARNING: Empty library name provided. Skipping."
        fi
    done

    # Add custom GCC flags.
    arguments+=("${custom_flags[@]}")

    arguments+=("-o" "$output_executable_path")

    write_log "Linking object files into $output_executable_path..."
    write_log "Command: $gcc_executable ${arguments[*]}"

    # Execute GCC for linking and capture all output.
    local temp_log_file
    temp_log_file=$(mktemp)
    if ! "$gcc_executable" "${arguments[@]}" &> "$temp_log_file"; then
        write_log "FAILED: Failed to link object files."
        write_log "$(cat "$temp_log_file")"
        rm "$temp_log_file"
        return 1
    fi
    write_log "$(cat "$temp_log_file")"
    rm "$temp_log_file"

    write_log "Successfully linked all object files to $output_executable_path"
    return 0
}

# --- Main Script Logic ---

# 1. Create Log Directory if it doesn't exist
if [ ! -d "$LOG_DIR" ]; then
    echo "Creating log directory: $LOG_DIR"
    mkdir -p "$LOG_DIR"
fi

# 2. Check GCC Availability
if ! test_gcc_availability; then
    exit 1
fi

# 3. Setup Build Directory
write_log "Setting up Build directory: $BUILD_DIR"
if [ -d "$BUILD_DIR" ]; then
    write_log "Clearing existing Build directory contents..."
    rm -rf "$BUILD_DIR"/*
else
    mkdir -p "$BUILD_DIR"
fi

# 4. Compile .c files from SourceDirectory
write_log "Searching for .c files in '$SOURCE_DIR'..."
# Collect all C files from the source directory, including subdirectories.
C_FILES_PRIMARY=$(find "$SOURCE_DIR" -name "*.c")

if [ -z "$C_FILES_PRIMARY" ]; then
    write_log "No .c files found in '$SOURCE_DIR'."
else
    for file in $C_FILES_PRIMARY; do
        if ! c_compile "$file" "$BUILD_DIR" "$GCC_PATH" "${INCLUDE_PATHS[*]}" "${GCC_FLAGS[*]}"; then
            write_log "Compilation failed for $file. Aborting."
            exit 1
        fi
    done
fi

# 5. Compile .c files from AdditionalSourceDirectory (if provided)
if [ ! -z "$ADDITIONAL_SOURCE_DIR" ]; then
    write_log "Searching for .c files in '$ADDITIONAL_SOURCE_DIR'..."
    C_FILES_ADDITIONAL=$(find "$ADDITIONAL_SOURCE_DIR" -name "*.c")

    if [ -z "$C_FILES_ADDITIONAL" ]; then
        write_log "No .c files found in '$ADDITIONAL_SOURCE_DIR'."
    else
        for file in $C_FILES_ADDITIONAL; do
            if ! c_compile "$file" "$BUILD_DIR" "$GCC_PATH" "${INCLUDE_PATHS[*]}" "${GCC_FLAGS[*]}"; then
                write_log "Compilation failed for $file. Aborting."
                exit 1
            fi
        done
    fi
fi

# 6. Link all .o files in the Build directory
write_log "Starting linking phase..."
if ! obj_link "$BUILD_DIR" "$OUTPUT_EXECUTABLE_NAME" "$GCC_PATH" "${LIBRARY_PATHS[*]}" "${LIBRARIES[*]}" "${GCC_FLAGS[*]}"; then
    write_log "Linking failed. See previous errors."
    exit 1
fi

write_log "Compilation and Linking process completed successfully!"
