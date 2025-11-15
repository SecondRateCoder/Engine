#! For Linux Systems:
    #!Packages:
        # sudo apt-get update && sudo apt-get install libglfw3 libglfw3-dev
    #! or
        # git clone https://github.com/glfw/glfw.git
        # cd glfw
        # mkdir build && cd build
        # cmake ..
        # make
        # sudo make install

    #! Power-shell Installation:
        #! Install Dependencies:
            # sudo apt-get update && sudo apt-get install -y wget apt-transport-https
        #!Install Power-Shell:
            # sudo apt-get install -y powershell
        #!Run it:
            # pwsh


#! Move to current Directory.
# cd C:/Users/olusa/OneDrive/Documents/GitHub/Engine/
#!Compile Source Directory.
# ./Compilation-Scripts/Make.ps1 -SourceDirectory "./engine" -OutputExecutableName "engine.exe"
#!Compile Source with additional Source Directory.
# ./Compilation-Scripts/Make.ps1 -SourceDirectory "./engine" -AdditionalSourceDirectory "./EXTRA_DIR" -OutputExecutableName "engine.exe"
#!Compile Source with Additional Source Directory and Libraries.
# ./Compilation-Scripts/Make.ps1 -SourceDirectory "./engine" -OutputExecutableName "app.exe" -IncludePaths "./engine/Libraries/include" -LibraryPaths "./engine/Libraries/lib" -Libraries "glfw3", "glfw3dll"


#! My Compilation command.
# ./Compilation-Scripts/Make.ps1 -SourceDirectory "./engine" -OutputExecutableName "engine.exe"  -LibraryPaths "./engine/Libraries/lib" -Libraries "glfw3dll" -IncludePaths "./engine/Libraries/include", "./engine", "./engine/graphics" -GccFlags "-O1", "-Wextra", "-std=c99", "-Wall", "-pedantic"

#! Optional GCC Flags:
# -GccFlags "-O1", "-Wextra", "-std=c99", "-Wall", "-pedantic", "-Wl,--verbose"
#! Additional Libraries:
# -LibraryPaths "./Libraries/lib" -Libraries "glfw3", "glfw3dll"
#! Additional Includes:
# -IncludePaths "./Libraries/include", "./engine", "./engine/graphics", "./engine/graphics/window"

#* gcc -c C:\Users\olusa\OneDrive\Documents\GitHub\Engine\engine\main.c 
#* -L C:\Users\olusa\OneDrive\Documents\GitHub\Engine\engine\Libraries\lib
#* -l glfw3 glfw3dll
#* -I C:\Users\olusa\OneDrive\Documents\GitHub\Engine\engine\Libraries\include C:\Users\olusa\OneDrive\Documents\GitHub\Engine\engine C:\Users\olusa\OneDrive\Documents\GitHub\Engine\engine\graphics
#* -o C:\Users\olusa\OneDrive\Documents\GitHub\Engine\build\main.o

# "-O1", "-Wextra", "-std=c99", "-Wall", "-pedantic"

param(
    [Parameter(Mandatory=$true)]
    [string]$SourceDirectory,

    [Parameter(Mandatory=$true)]
    [string]$OutputExecutableName,
    
    [Parameter(Mandatory=$true)]
    [string[]]$Libraries,

    [Parameter(Mandatory=$true)]
    [string[]]$GccFlags, # New parameter for general GCC flags

    [Parameter(Mandatory=$false)]
    [string]$AdditionalSourceDirectory
)

# --- Configuration ---
# $gccPath = "C:\msys64\mingw64\bin\gcc.exe"
$GCC = "gcc"
# $gccPath = "x86_64-w64-mingw32-gcc"
# $Date = (Get-Date -Format "yyyy") + "-" + ((Get-Date -Format "MM").ToInt32() * (Get-Date -Format "dd").ToInt32()).ToString() + (Get-Date -Format "hh-mm-ss")
$Date = (Get-Date -Format "yyyy-MM-dd_HH-mm-ss")
$buildDir = (Join-Path (Get-Location) "build\\build-$($Date)\\")
$ObjDir = Join-Path $buildDir "\\objects"
$logDir = Join-Path ($buildDir) "\\logs"
$logFile = Join-Path $logDir ("log_" + ($Date) + ".txt")
$IncludePaths = @((Join-Path (Get-Location) "\\engine\\Libraries\\include"), (Join-Path (Get-Location) "\\engine\\"), (Join-Path (Get-Location) "\\engine\\graphics\\"))
$LibraryPaths = @((Join-Path (Get-Location) "\\engine\\Libraries\\lib"))
# --- Functions ---

function Write-Log {
    param(
        [Parameter(Mandatory=$true)]
        [string]$Msg,
        [Parameter(Mandatory=$false)]
        [System.ConsoleColor]$Color
    )
    # Write message to the console and to the log file
    if($Color){
        Write-Host $Msg -ForegroundColor $Color
    }else{Write-Host $Msg -ForegroundColor Yellow}
    # Write to Log file
    if(-not (Test-Path $logFile)){
        if(-not (Test-Path $logDir)){New-Item -ItemType Directory -Path $logDir}
        New-Item -ItemType File -Path $logFile
    }
    Add-Content -Path $logFile -Value $Msg
}

function Prepare{
    # Test for GCC.
    Write-Log -Msg "Checking for GCC availability..."
    try{
        $null = & $GCC --version 2>&1
        if ($LASTEXITCODE -eq 0) {
            Write-Log "GCC found: $($GCC)" -Color Green
            return $true
        } else {
            Write-Log "GCC not found or not callable. Please ensure GCC is installed and in your system's PATH." -Color Red
            return $false
        }
    } catch {
        Write-Log "Error checking GCC: $($_.Exception.Message)" -Color Red
        Write-Log "Please ensure GCC is installed and in your system's PATH." -Color Red
        exit 1
    }
    # Test for build Directory.
    Write-Log "Setting up build directory: $($buildDir)"
    if (Test-Path $buildDir) {
        Write-Log "Clearing existing build directory contents..."
        Remove-Item -Path "$($buildDir)\*" -Recurse -Force -ErrorAction SilentlyContinue
    } else {
        New-Item -ItemType Directory -Force -Path $buildDir
    }
    # Test for Object Directory
    if(-not (Test-Path $ObjDir)){
        Write-Host "Creating Object directory: $($ObjDir)"
        New-Item -ItemType Directory -Force -Path $ObjDir 
    }
}

function C_Compile {
    param(
        [string]$FilePath
    )
    $fileName = [System.IO.Path]::GetFileNameWithoutExtension($FilePath)
    $outputObjectFile = Join-Path $ObjDir "$($fileName).o"
    New-Item -Path $outputObjectFile -ItemType File -Force

    Write-Log "Compiling $($FilePath) to $($outputObjectFile) ..."

    $arguments = @(
        "-c",
        "$(Resolve-Path $FilePath)"
    )

    if($IncludePaths){
        foreach($dir in $IncludePaths){
            if (Test-Path $dir -PathType Container){
                $arguments += "-I"
                $arguments += "$(Resolve-Path $dir)"
                Write-Log -Msg "Include path: $($dir)" -Color Green
            } else {
                Write-Log "WARNING: Include path not found or is not a directory: '$($dir)'. Skipping." -Color Red
            }
        }
    }

    $GccFlags | ForEach-Object { $arguments += $_}
    
    $arguments += "-o"
    $arguments += "$(Resolve-Path $outputObjectFile)"

    try {
        Write-Log "Command: $($GCC) $($arguments -join ' ')"
        
        # Capture all output streams from GCC
        $gccOutput = & $GCC $arguments 2>&1
        
        # Check the success status variable $? immediately after execution
        if ($? -or (-not $LASTEXITCODE)) {
            Write-Log "Successfully compiled $($FilePath)" -Color Green
            if ($gccOutput) {
                $gccOutput | ForEach-Object{Write-Log "Output: `t$_"}
            }
            return $true
        } else {
            Write-Log "FAILED: Failed to compile $($FilePath). GCC Exit Code: $($LASTEXITCODE)" -Color Red
            # Write the captured error output to the log file
            if ($gccOutput) {
                $gccOutput | ForEach-Object{Write-Log "$_"}
            }
            return $false
        }
    } catch {
        Write-Log "CRITICAL ERROR: Exception during compilation of $($FilePath):" -Color Red
        Write-Log $_.Exception.ToString()
        return $false
    }
}

function Obj_Link {
    $objectFiles = Get-ChildItem -Path $ObjDir -Filter "*.o" -Recurse | Select-Object -ExpandProperty FullName
    if (-not $objectFiles) {
        Write-Log "No .o files found in '$($ObjDir)' to link."
        return $false
    }

    $outputExecutablePath = Join-Path $buildDir $OutputExecutableName
    
    $arguments = @()

    # Coalesce .o files, object files
    foreach ($file in $objectFiles) {
        $arguments += "$(Resolve-Path $file)"
    }
    # Coalesce Library Directories
    if($LibraryPaths){
        foreach ($dir in $LibraryPaths) {
            if (Test-Path $dir -PathType Container) {
                $arguments += "-L"
                $arguments += "$(Resolve-Path $dir)"
                Write-Log "Path: $($dir)" -Color Green
            } else {
                Write-Log "WARNING: Library path not found or is not a directory: '$($dir)'. Skipping."
            }
        }
    }
    # Coalesce library files, .dll files
    if ($Libraries) {
        foreach ($lib in $Libraries) {
            if (-not [string]::IsNullOrEmpty($lib)) {
                $arguments += "-l"
                $arguments += $lib
            } else {
                Write-Log "WARNING: Empty library name provided. Skipping."
            }
        }
    }

    if($CustomGccFlags){$arguments += $CustomGccFlags}

    $arguments += "-o"
    New-Item -Path $outputExecutablePath -ItemType File -Force
    $arguments += "$(Resolve-Path $outputExecutablePath)"

    Write-Log "Linking object files into $($outputExecutablePath)..."
    try{
        Write-Log "Command: $($GCC) $($arguments -join ' ')"
        
        # Capture all output streams from GCC
        $gccOutput = & $GCC $arguments 2>&1
        
        # Check the success status variable $? immediately after execution
        if($?){
            Write-Log "Successfully linked all object files to $($outputExecutablePath)" -Color Green
             if ($gccOutput) {
                $gccOutput | ForEach-Object{Write-Log "Output: `t$_"}
            }
            return $true
        }else{
            Write-Log "FAILED: Failed to link object files. GCC Exit Code: $($LASTEXITCODE)" -Color Red
            if($gccOutput){$gccOutput | ForEach-Object{Write-Log "ERROR: $_" -Color Red}}
            return $false
        }
    }catch{
        Write-Log "CRITICAL ERROR: Exception during linking:" -Color Red
        Write-Log $_.Exception.ToString()
        return $false
    }
}

# --- Main Script Logic ---


(Prepare)
$IncludePaths | ForEach-Object{Write-Log "Output: `t$_" -Color Green}
# 4. Compile .c files from SourceDirectory
Write-Log "Searching for .c files in '$SourceDirectory'..."
$cFilesPrimary = Get-ChildItem -Path $SourceDirectory -Filter "*.c" -Recurse | Select-Object -ExpandProperty FullName
if(-not $cFilesPrimary){
    Write-Log "No .c files found in '$SourceDirectory'."
}else{
    foreach($file in $cFilesPrimary){
        if(-not(C_Compile -FilePath $file -buildDirectory $ObjDir -CustomGccFlags $GccFlags)){
            Write-Log "Compilation failed for $($file). Aborting."
            exit 1
        }
    }
}

# 5. Compile .c files from AdditionalSourceDirectory (if provided)
if($AdditionalSourceDirectory){
    Write-Log "Searching for .c files in '$AdditionalSourceDirectory'..."
    $cFilesAdditional = Get-ChildItem -Path $AdditionalSourceDirectory -Filter "*.c" -Recurse | Select-Object -ExpandProperty FullName
    if(-not $cFilesAdditional){Write-Log "No .c files found in '$AdditionalSourceDirectory'."
    }else{
        foreach($file in $cFilesAdditional){
            if(-not(C_Compile -FilePath $file -buildDirectory $ObjDir -CustomGccFlags $GccFlags)){
                Write-Log "Compilation failed for $($file). Aborting."
                exit 1
            }
        }
    }
}

# 6. Link all .o files in the build directory
Write-Log "Starting linking phase..."
if(-not(Obj_Link)){
    Write-Log "Linking failed. See previous errors."
    exit 1
}

Remove-Item (Join-Path (Get-Location) "\\build\\temp") -Recurse
New-Item (Join-Path (Get-Location) "\\build\\temp") -ItemType Directory

if($Libraries -and $LibraryPaths){
    foreach($path in $LibraryPaths){
        foreach($lib in $Libraries){
            $item = Join-Path $path ($lib + ".dll")
            Write-Log -Msg $item
            if(Test-Path $item){
                Write-Log -Msg "Copying $($item) to $($buildDir + "\\" + $lib + ".dll")"
                Copy-Item -Path $item -Destination ($buildDir + "\\" + $lib + ".dll")
                Copy-Item -Path $item -Destination (Join-Path (Get-Location) "\\build\\temp")
            }
        }
    }
}

Copy-Item -Path (Join-Path $buildDir ($OutputExecutableName + ".exe")) -Destination (Join-Path (Get-Location) "\\build\\temp\\$($OutputExecutableName).exe")

Write-Log "`nCompilation and Linking process completed successfully!"