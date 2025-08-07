#!/bin/bash
# A Bash script to compile and link C/C++ projects using GCC.
# This script is a conversion of a PowerShell build script.

# chmod +x ./Compilation-Scripts/Make.sh
# ./Compilation-Scripts/Make.sh --source-directory "./engine" --output-executable-name "app.exe" --include-paths "./Libraries/include" --library-paths "./Libraries/lib" --libraries "glfw3,glfw3dll" --gcc-flags "-O1,-Wextra,-std=c99,-Wall,-pedantic,-Wl,--verbose"

# --- Strict Mode & Error Handling ---

# Exit immediately if a command exits with a non-zero status.
set -e
# Treat unset variables as an error when substituting.
set -u
# The return value of a pipeline is the status of the last command to exit with a non-zero status.
set -o pipefail

# --- Usage & Help Message ---
usage() {
    echo "Usage: $0 --source-directory <path> --output-executable-name <name.exe> [options]"
    echo ""
    echo "Mandatory options:"
    echo "  --source-directory <path>           The directory containing source (.c) files."
    echo "  --output-executable-name <name.exe> The name of the final executable file."
    echo ""
    echo "Optional options:"
    echo "  --additional-source-directory <path>  An additional directory for source files."
    echo "  --include-paths <path1,path2,...>   Comma-separated list of include paths."
    echo "  --library-paths <path1,path2,...>   Comma-separated list of library paths."
    echo "  --libraries <lib1,lib2,...>         Comma-separated list of library names (e.g., glfw3)."
    echo "  --gcc-flags <flag1,flag2,...>       Comma-separated list of additional GCC flags."
    echo "  --help                              Display this help message and exit."
    exit 1
}

# --- Variable Initialization ---
SOURCE_DIR=""
ADDITIONAL_SOURCE_DIR=""
OUTPUT_EXEC_NAME=""
INCLUDE_PATHS=()
LIBRARY_PATHS=()
LIBRARIES=()
GCC_FLAGS=()

# --- Parameter Parsing ---
# Using a while loop to parse long-form arguments.
while [[ "$#" -gt 0 ]]; do
    case "$1" in
        --source-directory) SOURCE_DIR="$2"; shift ;;
        --additional-source-directory) ADDITIONAL_SOURCE_DIR="$2"; shift ;;
        --output-executable-name) OUTPUT_EXEC_NAME="$2"; shift ;;
        --include-paths) IFS=',' read -ra INCLUDE_PATHS <<< "$2"; shift ;;
        --library-paths) IFS=',' read -ra LIBRARY_PATHS <<< "$2"; shift ;;
        --libraries) IFS=',' read -ra LIBRARIES <<< "$2"; shift ;;
        --gcc-flags) IFS=',' read -ra GCC_FLAGS <<< "$2"; shift ;;
        --help) usage ;;
        *) echo "Unknown parameter passed: $1"; usage ;;
    esac
    shift
done

# --- Mandatory Parameter Check ---
if [[ -z "$SOURCE_DIR" || -z "$OUTPUT_EXEC_NAME" ]]; then
    echo "Error: Missing mandatory parameters."
    usage
fi

# --- Configuration ---
# Assume GCC is in the PATH. Use a full path if needed, e.g., GCC_PATH="/c/msys64/mingw64/bin/gcc.exe"
GCC_CMD="gcc"
BUILD_DIR="$(pwd)/Build"
LOG_DIR="$(pwd)/Resources/Logs"
LOG_FILE="$LOG_DIR/log_$(date +%Y-%m-%d_%H-%M-%S).txt"

# --- Functions ---

# Function to write messages to the console and the log file.
write_log() {
    # Use tee to send output to stdout and append to the log file.
    echo "$1" | tee -a "$LOG_FILE"
}

# Function to check for GCC availability.
test_gcc_availability() {
    write_log "Checking for GCC availability..."
    if command -v "$GCC_CMD" &> /dev/null; then
        write_log "GCC found: $($GCC_CMD --version | head -n 1)"
        return 0
    else
        write_log "ERROR: GCC not found. Please ensure GCC is installed and in your system's PATH."
        return 1
    fi
}

# Function to compile a single C/C++ file into an object file.
c_compile() {
    local file_path="$1"
    local build_dir="$2"
    local output_object_file="$build_dir/$(basename "$file_path" .c).o"

    write_log "Compiling $file_path to $output_object_file..."

    local args=("-c" "$file_path")

    # Add include paths
    for dir in "${INCLUDE_PATHS[@]}"; do
        if [[ -d "$dir" ]]; then
            args+=("-I" "$dir")
        else
            write_log "WARNING: Include path not found: '$dir'. Skipping."
        fi
    done

    # Add custom GCC flags, handling -Wl correctly
    for flag in "${GCC_FLAGS[@]}"; do
        if [[ "$flag" == "-Wl,"* ]]; then
            # If the flag is for the linker, pass it as-is
            args+=("$flag")
        else
            # For other flags, pass them directly
            args+=("$flag")
        fi
    done
    
    args+=("-o" "$output_object_file")

    write_log "Command: $GCC_CMD ${args[*]}"
    
    # Execute the command and capture all output to the log.
    if ! "$GCC_CMD" "${args[@]}" &>> "$LOG_FILE"; then
        write_log "FAILED: Failed to compile $file_path. See log for details."
        return 1
    fi

    write_log "Successfully compiled $file_path"
    return 0
}

# Function to link all object files into the final executable.
obj_link() {
    local build_dir="$1"
    local output_name="$2"
    local output_executable_path="$build_dir/$output_name"

    # Find all object files. Use -print0 and xargs -0 for safety with filenames.
    local object_files=()
    while IFS= read -r -d $'\0' file; do
        object_files+=("$file")
    done < <(find "$build_dir" -name "*.o" -print0)

    if [[ ${#object_files[@]} -eq 0 ]]; then
        write_log "ERROR: No .o files found in '$build_dir' to link. Aborting."
        return 1
    fi

    write_log "Linking object files into $output_executable_path..."

    local args=("${object_files[@]}")
    
    # Add library paths
    for dir in "${LIBRARY_PATHS[@]}"; do
        if [[ -d "$dir" ]]; then
            args+=("-L" "$dir")
        else
            write_log "WARNING: Library path not found: '$dir'. Skipping."
        fi
    done

    # Add libraries
    for lib in "${LIBRARIES[@]}"; do
        if [[ -n "$lib" ]]; then
            args+=("-l" "$lib")
        else
            write_log "WARNING: Empty library name provided. Skipping."
        fi
    done

    # Add custom GCC flags, handling -Wl correctly
    for flag in "${GCC_FLAGS[@]}"; do
        if [[ "$flag" == "-Wl,"* ]]; then
            # If the flag is for the linker, pass it as-is
            args+=("$flag")
        else
            # For other flags, pass them directly
            args+=("$flag")
        fi
    done

    args+=("-o" "$output_executable_path")

    write_log "Command: $GCC_CMD ${args[*]}"
    
    # Execute the command and capture all output to the log.
    if ! "$GCC_CMD" "${args[@]}" &>> "$LOG_FILE"; then
        write_log "FAILED: Failed to link. See log for details."
        return 1
    fi

    write_log "Successfully linked all object files to $output_executable_path"
    return 0
}

# --- Main Script Logic ---

# 1. Create Log Directory
mkdir -p "$LOG_DIR"

# 2. Check GCC Availability
if ! test_gcc_availability; then
    exit 1
fi

# 3. Setup Build Directory
write_log "Setting up Build directory: $BUILD_DIR"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# 4. Compile .c files from SourceDirectory
write_log "Searching for .c files in '$SOURCE_DIR'..."
source_files=$(find "$SOURCE_DIR" -name "*.c")
if [[ -z "$source_files" ]]; then
    write_log "No .c files found in '$SOURCE_DIR'."
else
    # find ... -print0 is used to handle file names with spaces
    find "$SOURCE_DIR" -name "*.c" -print0 | while IFS= read -r -d '' file; do
        if ! c_compile "$file" "$BUILD_DIR"; then
            write_log "Compilation failed. Aborting."
            exit 1
        fi
    done
fi

# 5. Compile .c files from AdditionalSourceDirectory (if provided)
if [[ -n "$ADDITIONAL_SOURCE_DIR" ]]; then
    write_log "Searching for .c files in '$ADDITIONAL_SOURCE_DIR'..."
    additional_files=$(find "$ADDITIONAL_SOURCE_DIR" -name "*.c")
    if [[ -z "$additional_files" ]]; then
        write_log "No .c files found in '$ADDITIONAL_SOURCE_DIR'."
    else
        find "$ADDITIONAL_SOURCE_DIR" -name "*.c" -print0 | while IFS= read -r -d '' file; do
            if ! c_compile "$file" "$BUILD_DIR"; then
                write_log "Compilation failed. Aborting."
                exit 1
            fi
        done
    fi
fi

# 6. Link all .o files
write_log "Starting linking phase..."
if ! obj_link "$BUILD_DIR" "$OUTPUT_EXEC_NAME"; then
    write_log "Linking failed. See previous errors."
    exit 1
fi

write_log "
Compilation and Linking process completed successfully!"