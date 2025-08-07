#! Move to current Directory.
# cd C:/Users/olusa/OneDrive/Documents/GitHub/Engine/
#!Compile Source Directory.
# ./Compilation-Scripts/Make.ps1 -SourceDirectory "./engine" -OutputExecutableName "engine.exe"
#!Compile Source with additional Source Directory.
# ./Compilation-Scripts/Make.ps1 -SourceDirectory "./engine" -AdditionalSourceDirectory "./EXTRA_DIR" -OutputExecutableName "engine.exe"
#!Compile Source with Additional Source Directory and Libraries.
# ./Compilation-Scripts/Make.ps1 -SourceDirectory "./engine" -OutputExecutableName "app.exe" -IncludePaths "./Libraries/include" -LibraryPaths "./Libraries/lib" -Libraries "glfw3", "glfw3dll"
#! Optional GCC Flags
# -GccFlags "-O1", "-Wextra", "-std=c99"
#! My Compilation command.
# ./Compilation-Scripts/Make.ps1 -SourceDirectory "./engine" -OutputExecutableName "app.exe" -IncludePaths "./Libraries/include" -LibraryPaths "./Libraries/lib" -Libraries "glfw3", "glfw3dll" -GccFlags "-O1", "-Wextra", "-std=c99", "-Wall", "-pedantic", "-Wl,--verbose"

param(
    [Parameter(Mandatory=$true)]
    [string]$SourceDirectory,

    [Parameter(Mandatory=$false)]
    [string]$AdditionalSourceDirectory,

    [Parameter(Mandatory=$true)]
    [string]$OutputExecutableName,

    [Parameter(Mandatory=$false)]
    [string[]]$IncludePaths,

    [Parameter(Mandatory=$false)]
    [string[]]$LibraryPaths,

    [Parameter(Mandatory=$false)]
    [string[]]$Libraries,

    [Parameter(Mandatory=$false)]
    [string[]]$GccFlags # New parameter for general GCC flags
)

# --- Configuration ---
$gccPath = "C:\msys64\mingw64\bin\gcc.exe"
$buildDir = Join-Path (Get-Location) "Build"
$logDir = Join-Path (Get-Location) "Resources\Logs"
$logFile = Join-Path $logDir ("log_" + (Get-Date -Format "yyyy-MM-dd_HH-mm-ss") + ".txt")

# --- Functions ---

function Write-Log {
    param(
        [string]$Message
    )
    # Write message to the console and to the log file
    Write-Host $Message
    Add-Content -Path $logFile -Value $Message
}

function Test-GccAvailability {
    Write-Log "Checking for GCC availability..."
    try {
        $null = & $gccPath --version 2>&1
        if ($LASTEXITCODE -eq 0) {
            Write-Log "GCC found: $($gccPath)"
            return $true
        } else {
            Write-Log "GCC not found or not callable. Please ensure GCC is installed and in your system's PATH."
            return $false
        }
    } catch {
        Write-Log "Error checking GCC: $($_.Exception.Message)"
        Write-Log "Please ensure GCC is installed and in your system's PATH."
        return $false
    }
}

function C_Compile {
    param(
        [string]$FilePath,
        [string]$BuildDirectory,
        [string]$GccExecutable,
        [string[]]$IncludeDirectories,
        [string[]]$CustomGccFlags
    )
    $fileName = [System.IO.Path]::GetFileNameWithoutExtension($FilePath)
    $outputObjectFile = Join-Path $BuildDirectory "$($fileName).o"
    New-Item -Path $outputObjectFile -ItemType File

    Write-Log "Compiling $($FilePath) to $($outputObjectFile)..."

    $arguments = @(
        "-c"
        "$(Resolve-Path $FilePath)"
    )

    if ($IncludeDirectories) {
        foreach ($dir in $IncludeDirectories) {
            if (Test-Path $dir -PathType Container) {
                $arguments += "-I"
                $arguments += "$(Resolve-Path $dir)"
            } else {
                Write-Log "WARNING: Include path not found or is not a directory: '$dir'. Skipping."
            }
        }
    }

    if ($CustomGccFlags) {
        $arguments += $CustomGccFlags
    }
    
    $arguments += "-o"
    $arguments += "$(Resolve-Path $outputObjectFile)"

    try {
        Write-Log "Command: $($GccExecutable) $($arguments -join ' ')"
        
        # Capture all output streams from GCC
        $gccOutput = & $GccExecutable $arguments 2>&1
        
        # Write the captured output to the log file
        $gccOutput | ForEach-Object { Write-Log $_ }

        if ($LASTEXITCODE -eq 0) {
            Write-Log "Successfully compiled $($FilePath)"
            return $true
        } else {
            Write-Log "FAILED: Failed to compile $($FilePath). GCC Exit Code: $($LASTEXITCODE)"
            return $false
        }
    } catch {
        Write-Log "ERROR: Error during compilation of $($FilePath): $($_.Exception.Message)"
        return $false
    }
}

function Obj_Link {
    param(
        [string]$BuildDirectory,
        [string]$OutputName,
        [string]$GccExecutable,
        [string[]]$LibraryDirectories,
        [string[]]$LibraryNames,
        [string[]]$CustomGccFlags
    )
    $objectFiles = Get-ChildItem -Path $BuildDirectory -Filter "*.o" -Recurse | Select-Object -ExpandProperty FullName
    if (-not $objectFiles) {
        Write-Log "No .o files found in '$BuildDirectory' to link."
        return $false
    }

    $outputExecutablePath = Join-Path $BuildDirectory $OutputName
    New-Item -Path $outputExecutablePath -ItemType File
    
    $arguments = @()

    foreach ($file in $objectFiles) {
        $arguments += "$(Resolve-Path $file)"
    }

    if ($LibraryDirectories) {
        foreach ($dir in $LibraryDirectories) {
            if (Test-Path $dir -PathType Container) {
                $arguments += "-L"
                $arguments += "$(Resolve-Path $dir)"
            } else {
                Write-Log "WARNING: Library path not found or is not a directory: '$dir'. Skipping."
            }
        }
    }

    if ($LibraryNames) {
        foreach ($lib in $LibraryNames) {
            if (-not [string]::IsNullOrEmpty($lib)) {
                $arguments += "-l"
                $arguments += $lib
            } else {
                Write-Log "WARNING: Empty library name provided. Skipping."
            }
        }
    }

    if ($CustomGccFlags) {
        $arguments += $CustomGccFlags
    }

    $arguments += "-o"
    $arguments += "$(Resolve-Path $outputExecutablePath)"

    Write-Log "Linking object files into $($outputExecutablePath)..."
    try {
        Write-Log "Command: $($GccExecutable) $($arguments -join ' ')"
        
        # Capture all output streams from GCC
        $gccOutput = & $GccExecutable $arguments 2>&1
        
        # Write the captured output to the log file
        $gccOutput | ForEach-Object { Write-Log $_ }

        if ($LASTEXITCODE -eq 0) {
            Write-Log "Successfully linked all object files to $($outputExecutablePath)"
            return $true
        } else {
            Write-Log "FAILED: Failed to link object files. GCC Exit Code: $($LASTEXITCODE)"
            return $false
        }
    } catch {
        Write-Log "ERROR: Error during linking: $($_.Exception.Message)"
        return $false
    }
}

# --- Main Script Logic ---
# 1. Create Log Directory if it doesn't exist
if (-not (Test-Path $logDir)) {
    Write-Host "Creating log directory: $($logDir)"
    New-Item -ItemType Directory -Path $logDir | Out-Null
}

# 2. Check GCC Availability
if (-not (Test-GccAvailability)) {
    exit 1
}

# 3. Setup Build Directory
Write-Log "Setting up Build directory: $($buildDir)"
if (Test-Path $buildDir) {
    Write-Log "Clearing existing Build directory contents..."
    Remove-Item -Path "$($buildDir)\*" -Recurse -Force -ErrorAction SilentlyContinue
} else {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

# 4. Compile .c files from SourceDirectory
Write-Log "Searching for .c files in '$SourceDirectory'..."
$cFilesPrimary = Get-ChildItem -Path $SourceDirectory -Filter "*.c" -Recurse | Select-Object -ExpandProperty FullName
if (-not $cFilesPrimary) {
    Write-Log "No .c files found in '$SourceDirectory'."
} else {
    foreach ($file in $cFilesPrimary) {
        if (-not (C_Compile -FilePath $file -BuildDirectory $buildDir -GccExecutable $gccPath -IncludeDirectories $IncludePaths -CustomGccFlags $GccFlags)) {
            Write-Log "Compilation failed for $($file). Aborting."
            exit 1
        }
    }
}

# 5. Compile .c files from AdditionalSourceDirectory (if provided)
if ($AdditionalSourceDirectory) {
    Write-Log "Searching for .c files in '$AdditionalSourceDirectory'..."
    $cFilesAdditional = Get-ChildItem -Path $AdditionalSourceDirectory -Filter "*.c" -Recurse | Select-Object -ExpandProperty FullName
    if (-not $cFilesAdditional) {
        Write-Log "No .c files found in '$AdditionalSourceDirectory'."
    } else {
        foreach ($file in $cFilesAdditional) {
            if (-not (C_Compile -FilePath $file -BuildDirectory $buildDir -GccExecutable $gccPath -IncludeDirectories $IncludePaths -CustomGccFlags $GccFlags)) {
                Write-Log "Compilation failed for $($file). Aborting."
                exit 1
            }
        }
    }
}

# 6. Link all .o files in the Build directory
Write-Log "Starting linking phase..."
if (-not (Obj_Link -BuildDirectory $buildDir -OutputName $OutputExecutableName -GccExecutable $gccPath -LibraryDirectories $LibraryPaths -LibraryNames $Libraries -CustomGccFlags $GccFlags)) {
    Write-Log "Linking failed. See previous errors."
    exit 1
}

Write-Log "`nCompilation and Linking process completed successfully!"