#! Move to current Directory.
# cd C:\Users\olusa\OneDrive\Documents\GitHub\Engine\
#!Compile Source Directory.
# .\Compilation Scripts\Make.ps1 -SourceDirectory ".\engine" -OutputExecutableName "engine.exe"
#!Compile Source with additional Source Directory.
# .\Compilation-Scripts\Make.ps1 -SourceDirectory ".\engine" -AdditionalSourceDirectory ".\EXTRA_DIR" -OutputExecutableName "engine.exe"
#!Compile Source with Additional Source Directory and Libraries.
# .\Compilation-Scripts\Make.ps1 -SourceDirectory ".\engine" -OutputExecutableName "app.exe" -IncludePaths ".\Libraries\include" -LibraryPaths ".\Libraries\lib" -Libraries "glfw3", "glfw3dll"
#! Optional GCC Flags
# -GccFlags "-O1", "-Wextra", "-std=c99"
#! My Compilation command.
# .\Compilation-Scripts\Make.ps1 -SourceDirectory ".\engine" -OutputExecutableName "app.exe" -IncludePaths ".\Libraries\include" -LibraryPaths ".\Libraries\lib" -Libraries "glfw3", "glfw3dll" -GccFlags "-O1", "-Wextra", "-std=c99"
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
$gccPath = "C:/msys64/mingw64/bin/gcc.exe" # Assuming gcc is in your PATH. If not, provide the full path, e.g., "C:\MinGW\bin\gcc.exe"
$buildDir = Join-Path (Get-Location) "Build"

# --- Functions ---

function Test-GccAvailability {
    Write-Host "Checking for GCC availability..." -ForegroundColor Cyan
    try {
        # Try to run gcc with --version and suppress output, just check exit code
        $null = & $gccPath --version 2>&1
        if ($LASTEXITCODE -eq 0) {
            Write-Host "GCC found: $($gccPath)" -ForegroundColor Green
            return $true
        } else {
            Write-Host "GCC not found or not callable. Please ensure GCC is installed and in your system's PATH." -ForegroundColor Red
            return $false
        }
    } catch {
        Write-Host "Error checking GCC: $($_.Exception.Message)" -ForegroundColor Red
        Write-Host "Please ensure GCC is installed and in your system's PATH." -ForegroundColor Red
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

    Write-Host "Compiling $($FilePath) to $($outputObjectFile)..." -ForegroundColor Yellow

    # Build the argument list as an array
    $arguments = @(
        "-c"
        $FilePath
    )

    if ($IncludeDirectories) {
        foreach ($dir in $IncludeDirectories) {
            if (Test-Path $dir -PathType Container) {
                $arguments += "-I`"$dir`""
            } else {
                Write-Warning "Include path not found or is not a directory: '$dir'. Skipping."
            }
        }
    }

    if ($CustomGccFlags) {
        $arguments += $CustomGccFlags
    }
    
    $arguments += "-o", $outputObjectFile

    try {
        # Use the call operator with the argument array
        # This is the key change for robust execution
        Write-Host "Command: $($GccExecutable) $($arguments)" -ForegroundColor DarkYellow
        & $GccExecutable $arguments
        if ($LASTEXITCODE -eq 0) {
            Write-Host "Successfully compiled $($FilePath)" -ForegroundColor Green
            return $true
        } else {
            Write-Host "Failed to compile $($FilePath). GCC Exit Code: $($LASTEXITCODE)" -ForegroundColor Red
            return $false
        }
    } catch {
        Write-Host "Error during compilation of $($FilePath): $($_.Exception.Message)" -ForegroundColor Red
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
        [string[]]$CustomGccFlags # New parameter for custom GCC flags
    )
    $objectFiles = Get-ChildItem -Path $BuildDirectory -Filter "*.o" -Recurse | Select-Object -ExpandProperty FullName
    if (-not $objectFiles) {
        Write-Host "No .o files found in '$BuildDirectory' to link." -ForegroundColor Red
        return $false
    }

    $objectFilePaths = ($objectFiles | ForEach-Object { "`"$_`"" }) -join " "
    $outputExecutablePath = Join-Path (Get-Location) $OutputName

    # Construct -L flags
    $libraryPathFlags = ""
    if ($LibraryDirectories) {
        foreach ($dir in $LibraryDirectories) {
            if (Test-Path $dir -PathType Container) {
                $libraryPathFlags += " -L`"$dir`""
            } else {
                Write-Warning "Library path not found or is not a directory: '$dir'. Skipping."
            }
        }
    }

    # Construct -l flags and validate library names
    $libraryLinkFlags = ""
    if ($LibraryNames) {
        foreach ($lib in $LibraryNames) {
            if (-not [string]::IsNullOrEmpty($lib)) {
                $libraryLinkFlags += " -l`"$lib`""
            } else {
                Write-Warning "Empty library name provided. Skipping."
            }
        }
    }

    # Construct custom GCC flags string
    $customFlagsString = ""
    if ($CustomGccFlags) {
        $customFlagsString = ($CustomGccFlags | ForEach-Object { "`"$_`"" }) -join " "
    }

    Write-Host "Linking object files into $($outputExecutablePath)..." -ForegroundColor Yellow
    try {
        # Include custom flags in the linking command
        $command = "& `"$GccExecutable`" $objectFilePaths $($libraryPathFlags) $($libraryLinkFlags) $($customFlagsString) -o `"$outputExecutablePath`""
        Write-Host "Linking Command $($command)" -ForegroundColor DarkYellow
        Invoke-Expression $command
        if ($LASTEXITCODE -eq 0) {
            Write-Host "Successfully linked all object files to $($outputExecutablePath)" -ForegroundColor Green
            return $true
        } else {
            Write-Host "Failed to link object files. GCC Exit Code: $($LASTEXITCODE)" -ForegroundColor Red
            return $false
        }
    } catch {
        Write-Host "Error during linking: $($_.Exception.Message)" -ForegroundColor Red
        return $false
    }
}

# --- Main Script Logic ---

# 1. Check GCC Availability
if (-not (Test-GccAvailability)) {
    exit 1 # Exit if GCC is not found
}

# 2. Setup Build Directory
Write-Host "Setting up Build directory: $($buildDir)" -ForegroundColor Cyan
if (Test-Path $buildDir) {
    Write-Host "Clearing existing Build directory contents..." -ForegroundColor DarkYellow
    Remove-Item -Path "$($buildDir)\*" -Recurse -Force -ErrorAction SilentlyContinue
} else {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

# 3. Compile .c files from SourceDirectory
Write-Host "Searching for .c files in '$SourceDirectory'..." -ForegroundColor Cyan
$cFilesPrimary = Get-ChildItem -Path $SourceDirectory -Filter "*.c" -Recurse | Select-Object -ExpandProperty FullName
if (-not $cFilesPrimary) {
    Write-Host "No .c files found in '$SourceDirectory'." -ForegroundColor Red
    # Decide if this is a fatal error or if we can proceed if AdditionalSourceDirectory is provided
    # For now, we'll continue, but linking will fail if no .o files are ever created.
} else {
    foreach ($file in $cFilesPrimary) {
        if (-not (C_Compile -FilePath $file -BuildDirectory $buildDir -GccExecutable $gccPath -IncludeDirectories $IncludePaths -CustomGccFlags $GccFlags)) {
            Write-Host "Compilation failed for $($file). Aborting." -ForegroundColor Red
            exit 1
        }
    }
}

# 4. Compile .c files from AdditionalSourceDirectory (if provided)
if ($AdditionalSourceDirectory) {
    Write-Host "Searching for .c files in '$AdditionalSourceDirectory'..." -ForegroundColor Cyan
    $cFilesAdditional = Get-ChildItem -Path $AdditionalSourceDirectory -Filter "*.c" -Recurse | Select-Object -ExpandProperty FullName
    if (-not $cFilesAdditional) {
        Write-Host "No .c files found in '$AdditionalSourceDirectory'." -ForegroundColor Yellow
    } else {
        foreach ($file in $cFilesAdditional) {
            if (-not (C_Compile -FilePath $file -BuildDirectory $buildDir -GccExecutable $gccPath -IncludeDirectories $IncludePaths -CustomGccFlags $GccFlags)) {
                Write-Host "Compilation failed for $($file). Aborting." -ForegroundColor Red
                exit 1
            }
        }
    }
}

# 5. Link all .o files in the Build directory
Write-Host "Starting linking phase..." -ForegroundColor Cyan
if (-not (Obj_Link -BuildDirectory $buildDir -OutputName $OutputExecutableName -GccExecutable $gccPath -LibraryDirectories $LibraryPaths -LibraryNames $Libraries -CustomGccFlags $GccFlags)) {
    Write-Host "Linking failed. See previous errors." -ForegroundColor Red
    exit 1
}

Write-Host "`nCompilation and Linking process completed successfully!" -ForegroundColor Green
