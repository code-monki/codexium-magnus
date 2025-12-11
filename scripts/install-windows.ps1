# Codexium Magnus - Windows Installation Script
# PowerShell script to install dependencies and set up the development environment

param(
    [switch]$SkipQt,
    [switch]$SkipCMake,
    [switch]$SkipCompiler,
    [switch]$SkipLibsodium,
    [switch]$InstallQt,
    [string]$QtVersion = "6.9.3",
    [string]$QtInstallPath = "$env:USERPROFILE\Qt",
    [string]$Compiler = "msvc2019_64"
)

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Codexium Magnus - Windows Setup" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check if running as administrator (needed for some installations)
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

# Function to check if a command exists
function Test-Command {
    param([string]$Command)
    $null = Get-Command $Command -ErrorAction SilentlyContinue
    return $?
}

# Function to check if a directory exists
function Test-Directory {
    param([string]$Path)
    return (Test-Path -Path $Path -PathType Container)
}

# Function to find Qt installation
function Find-QtInstallation {
    param([string]$Version, [string]$Compiler)
    
    $possiblePaths = @(
        "$env:USERPROFILE\Qt\$Version\$Compiler",
        "C:\Qt\$Version\$Compiler",
        "$env:ProgramFiles\Qt\$Version\$Compiler",
        "$env:ProgramFiles(x86)\Qt\$Version\$Compiler"
    )
    
    foreach ($path in $possiblePaths) {
        $cmakePath = Join-Path $path "lib\cmake\Qt6"
        if (Test-Directory $cmakePath) {
            return $path
        }
    }
    
    return $null
}

# Function to get Qt component identifier
function Get-QtComponentId {
    param([string]$Version, [string]$Compiler)
    
    # Map compiler names to Qt component identifiers
    $compilerMap = @{
        "msvc2019_64" = "msvc2019_64"
        "msvc2022_64" = "msvc2022_64"
        "mingw_64" = "mingw_64"
    }
    
    $compilerId = $compilerMap[$Compiler]
    if (-not $compilerId) {
        $compilerId = $Compiler
    }
    
    # Convert version to component format (e.g., "6.9.3" -> "qt.qt6.6903")
    $versionParts = $Version.Split('.')
    $major = $versionParts[0]
    $minor = $versionParts[1]
    $patch = $versionParts[2]
    $versionId = "$major$minor$patch"
    
    return "qt.qt6.$versionId.win64_$compilerId"
}

# Function to install Qt automatically
function Install-Qt {
    param(
        [string]$Version,
        [string]$InstallPath,
        [string]$Compiler
    )
    
    Write-Host "Downloading Qt Online Installer..." -ForegroundColor Yellow
    
    $installerUrl = "https://download.qt.io/official_releases/online_installers/qt-unified-windows-x64-online.exe"
    $installerPath = Join-Path $env:TEMP "qt-unified-windows-x64-online.exe"
    
    try {
        # Download installer
        Invoke-WebRequest -Uri $installerUrl -OutFile $installerPath -UseBasicParsing
        Write-Host "[OK] Download complete" -ForegroundColor Green
    } catch {
        Write-Host "[ERROR] Failed to download Qt installer: $_" -ForegroundColor Red
        return $false
    }
    
    Write-Host ""
    Write-Host "Installing Qt $Version..." -ForegroundColor Yellow
    Write-Host "This may take 10-30 minutes depending on your internet connection." -ForegroundColor Gray
    Write-Host ""
    
    # Get component identifier
    $componentId = Get-QtComponentId -Version $Version -Compiler $Compiler
    $webEngineComponent = "qt.qt6.$($Version.Replace('.','')).win64_$Compiler.qtwebengine"
    
    # Build installer arguments
    $installArgs = @(
        "--root", $InstallPath,
        "--accept-licenses",
        "--accept-messages",
        "--confirm-command",
        "install",
        $componentId,
        $webEngineComponent
    )
    
    try {
        # Run installer
        $process = Start-Process -FilePath $installerPath -ArgumentList $installArgs -Wait -PassThru -NoNewWindow
        
        if ($process.ExitCode -eq 0) {
            Write-Host "[OK] Qt installation completed successfully" -ForegroundColor Green
            
            # Clean up installer
            Remove-Item -Path $installerPath -Force -ErrorAction SilentlyContinue
            
            return $true
        } else {
            Write-Host "[ERROR] Qt installation failed with exit code: $($process.ExitCode)" -ForegroundColor Red
            Write-Host "You may need to run the installer manually." -ForegroundColor Yellow
            return $false
        }
    } catch {
        Write-Host "[ERROR] Failed to run Qt installer: $_" -ForegroundColor Red
        Write-Host "You may need to run the installer manually: $installerPath" -ForegroundColor Yellow
        return $false
    }
}

Write-Host "Checking prerequisites..." -ForegroundColor Yellow
Write-Host ""

# Check for Git
if (-not (Test-Command "git")) {
    Write-Host "ERROR: Git is not installed." -ForegroundColor Red
    Write-Host "Please install Git from: https://git-scm.com/download/win" -ForegroundColor Yellow
    exit 1
}
Write-Host "[OK] Git found" -ForegroundColor Green

# Check for CMake
$cmakeFound = Test-Command "cmake"
if (-not $cmakeFound -and -not $SkipCMake) {
    Write-Host "CMake not found. Attempting to install..." -ForegroundColor Yellow
    
    # Try winget first (Windows 10/11)
    if (Test-Command "winget") {
        Write-Host "Installing CMake via winget..." -ForegroundColor Yellow
        winget install -e --id Kitware.CMake --silent
        if ($LASTEXITCODE -eq 0) {
            $machinePath = [System.Environment]::GetEnvironmentVariable("Path", "Machine")
            $userPath = [System.Environment]::GetEnvironmentVariable("Path", "User")
            $env:Path = $machinePath + ";" + $userPath
            $cmakeFound = Test-Command "cmake"
        }
    }
    
    # Try Chocolatey if winget failed
    if (-not $cmakeFound -and (Test-Command "choco")) {
        Write-Host "Installing CMake via Chocolatey..." -ForegroundColor Yellow
        if ($isAdmin) {
            choco install cmake -y
            $machinePath = [System.Environment]::GetEnvironmentVariable("Path", "Machine")
            $userPath = [System.Environment]::GetEnvironmentVariable("Path", "User")
            $env:Path = $machinePath + ";" + $userPath
            $cmakeFound = Test-Command "cmake"
        } else {
            Write-Host "WARNING: Administrator privileges required for Chocolatey installation." -ForegroundColor Yellow
            Write-Host "Please run this script as Administrator or install CMake manually." -ForegroundColor Yellow
        }
    }
    
    if (-not $cmakeFound) {
        Write-Host "ERROR: Could not install CMake automatically." -ForegroundColor Red
        Write-Host "Please install CMake manually from: https://cmake.org/download/" -ForegroundColor Yellow
        Write-Host "Or install Chocolatey (https://chocolatey.org/) and run: choco install cmake" -ForegroundColor Yellow
        exit 1
    }
}

if ($cmakeFound) {
    $cmakeVersion = (cmake --version | Select-Object -First 1)
    Write-Host "[OK] CMake found: $cmakeVersion" -ForegroundColor Green
} else {
    Write-Host "[WARN] CMake check skipped" -ForegroundColor Yellow
}

# Check for C++ compiler
$compilerFound = $false
if (-not $SkipCompiler) {
    # Check for MSVC (cl.exe)
    if (Test-Command "cl") {
        $compilerFound = $true
        Write-Host "[OK] MSVC compiler found" -ForegroundColor Green
    } else {
        # Check for Visual Studio installation
        $vsPaths = @(
            "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community",
            "${env:ProgramFiles}\Microsoft Visual Studio\2022\Professional",
            "${env:ProgramFiles}\Microsoft Visual Studio\2022\Enterprise",
            "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Community",
            "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Professional",
            "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Enterprise"
        )
        
        $vsFound = $false
        foreach ($vsPath in $vsPaths) {
            if (Test-Directory $vsPath) {
                $vsFound = $true
                Write-Host "[OK] Visual Studio found at: $vsPath" -ForegroundColor Green
                Write-Host "  Note: Use 'Developer Command Prompt' or 'x64 Native Tools Command Prompt' to build" -ForegroundColor Yellow
                break
            }
        }
        
        if (-not $vsFound) {
            Write-Host "[WARN] C++ compiler not found in PATH" -ForegroundColor Yellow
            Write-Host "  Install Visual Studio Build Tools or Visual Studio with C++ workload" -ForegroundColor Yellow
            Write-Host "  Download: https://visualstudio.microsoft.com/downloads/" -ForegroundColor Yellow
        }
    }
} else {
    Write-Host "[WARN] Compiler check skipped" -ForegroundColor Yellow
}

# Check for Qt
$qtPath = Find-QtInstallation -Version $QtVersion -Compiler $Compiler
if (-not $qtPath -and -not $SkipQt) {
    Write-Host ""
    Write-Host "Qt $QtVersion not found." -ForegroundColor Yellow
    Write-Host ""
    
    if ($InstallQt) {
        # Automatic installation requested
        Write-Host "Automatic Qt installation requested..." -ForegroundColor Cyan
        $installSuccess = Install-Qt -Version $QtVersion -InstallPath $QtInstallPath -Compiler $Compiler
        
        if ($installSuccess) {
            # Verify installation
            $qtPath = Find-QtInstallation -Version $QtVersion -Compiler $Compiler
            if (-not $qtPath) {
                Write-Host "[WARN] Qt installed but not found. You may need to restart your terminal." -ForegroundColor Yellow
            }
        } else {
            Write-Host ""
            Write-Host "Automatic installation failed. Please install Qt manually:" -ForegroundColor Yellow
            Write-Host "1. Download Qt Online Installer: https://www.qt.io/download-open-source" -ForegroundColor White
            Write-Host "2. Install Qt $QtVersion with $Compiler components" -ForegroundColor White
            Write-Host "3. Install Qt WebEngine component (required)" -ForegroundColor White
            Write-Host ""
            $response = Read-Host "Continue anyway? (y/N)"
            if ($response -ne "y" -and $response -ne "Y") {
                exit 1
            }
        }
    } else {
        # Manual installation instructions
        Write-Host "Qt installation options:" -ForegroundColor Cyan
        Write-Host "1. Automatic installation (recommended):" -ForegroundColor White
        Write-Host "   .\scripts\install-windows.ps1 -InstallQt" -ForegroundColor Cyan
        Write-Host ""
        Write-Host "2. Manual installation:" -ForegroundColor White
        Write-Host "   - Download Qt Online Installer: https://www.qt.io/download-open-source" -ForegroundColor Gray
        Write-Host "   - Install Qt $QtVersion with $Compiler components" -ForegroundColor Gray
        Write-Host "   - Install Qt WebEngine component (required)" -ForegroundColor Gray
        Write-Host ""
        Write-Host "After installation, run this script again or set Qt6_DIR manually:" -ForegroundColor Yellow
        Write-Host "  `$env:Qt6_DIR = `"$QtInstallPath\$QtVersion\$Compiler\lib\cmake\Qt6`"" -ForegroundColor White
        Write-Host ""
        
        $response = Read-Host "Would you like to install Qt automatically now? (Y/n)"
        if ($response -ne "n" -and $response -ne "N") {
            $installSuccess = Install-Qt -Version $QtVersion -InstallPath $QtInstallPath -Compiler $Compiler
            if ($installSuccess) {
                $qtPath = Find-QtInstallation -Version $QtVersion -Compiler $Compiler
            } else {
                $response = Read-Host "Continue anyway? (y/N)"
                if ($response -ne "y" -and $response -ne "Y") {
                    exit 1
                }
            }
        } else {
            $response = Read-Host "Continue without Qt? (y/N)"
            if ($response -ne "y" -and $response -ne "Y") {
                exit 1
            }
        }
    }
}

if ($qtPath) {
    $qt6Dir = Join-Path $qtPath "lib\cmake\Qt6"
    Write-Host "[OK] Qt found at: $qtPath" -ForegroundColor Green
    
    # Set environment variable for current session
    $env:Qt6_DIR = $qt6Dir
    Write-Host "  Qt6_DIR set to: $qt6Dir" -ForegroundColor Gray
    
    # Try to set user environment variable (persistent)
    try {
        [System.Environment]::SetEnvironmentVariable("Qt6_DIR", $qt6Dir, "User")
        Write-Host "  Qt6_DIR saved to user environment variables" -ForegroundColor Gray
    } catch {
        Write-Host "  Warning: Could not save Qt6_DIR to environment (may need admin)" -ForegroundColor Yellow
    }
}

# Check for libsodium (optional)
if (-not $SkipLibsodium) {
    $libsodiumFound = $false
    
    # Check vcpkg
    if (Test-Command "vcpkg") {
        $libsodiumFound = $true
        Write-Host "[OK] vcpkg found (libsodium can be installed via vcpkg)" -ForegroundColor Green
    } else {
        # Check if libsodium is in common locations
        $possiblePaths = @(
            "$env:ProgramFiles\libsodium",
            "$env:ProgramFiles(x86)\libsodium",
            "$env:USERPROFILE\libsodium"
        )
        
        foreach ($path in $possiblePaths) {
            if (Test-Directory $path) {
                $libsodiumFound = $true
                Write-Host "[OK] libsodium found at: $path" -ForegroundColor Green
                break
            }
        }
    }
    
    if (-not $libsodiumFound) {
        Write-Host "[WARN] libsodium not found (optional, for signature verification)" -ForegroundColor Yellow
        Write-Host "  Install via vcpkg: vcpkg install libsodium:x64-windows" -ForegroundColor Yellow
        Write-Host "  Or download from: https://download.libsodium.org/libsodium/releases/" -ForegroundColor Yellow
    }
} else {
    Write-Host "[WARN] libsodium check skipped" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Verification" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Verify CMake can find Qt
if ($env:Qt6_DIR -and (Test-Command "cmake")) {
    Write-Host "Verifying Qt6_DIR..." -ForegroundColor Yellow
    if (Test-Path $env:Qt6_DIR) {
        Write-Host "[OK] Qt6_DIR is valid: $env:Qt6_DIR" -ForegroundColor Green
    } else {
        Write-Host "[ERROR] Qt6_DIR path does not exist: $env:Qt6_DIR" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Next Steps" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "1. If Qt was just installed, you may need to restart your terminal" -ForegroundColor White
Write-Host "2. Configure the build:" -ForegroundColor White
Write-Host "   .\scripts\configure-windows.ps1" -ForegroundColor Cyan
Write-Host "   OR" -ForegroundColor White
Write-Host "   make configure" -ForegroundColor Cyan
Write-Host ""
Write-Host "3. Build the project:" -ForegroundColor White
Write-Host "   make build" -ForegroundColor Cyan
Write-Host "   OR" -ForegroundColor White
Write-Host "   .\scripts\build-windows.ps1" -ForegroundColor Cyan
Write-Host ""
Write-Host "4. Run tests:" -ForegroundColor White
Write-Host "   make test" -ForegroundColor Cyan
Write-Host ""
Write-Host "5. Run the application:" -ForegroundColor White
Write-Host "   make run" -ForegroundColor Cyan
Write-Host ""
