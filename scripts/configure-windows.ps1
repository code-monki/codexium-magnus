# Codexium Magnus - Windows CMake Configuration Script
# Configures the CMake build system for Windows

param(
    [string]$BuildType = "Release",
    [string]$BuildDir = "build",
    [string]$QtVersion = "6.9.3",
    [string]$Compiler = "msvc2019_64"
)

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Codexium Magnus - CMake Configuration" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Find Qt installation
function Find-QtInstallation {
    param([string]$Version, [string]$Compiler)
    
    # Check environment variable first
    if ($env:Qt6_DIR) {
        $qtDir = $env:Qt6_DIR
        if (Test-Path $qtDir) {
            return $qtDir
        }
    }
    
    $possiblePaths = @(
        "$env:USERPROFILE\Qt\$Version\$Compiler\lib\cmake\Qt6",
        "C:\Qt\$Version\$Compiler\lib\cmake\Qt6",
        "$env:ProgramFiles\Qt\$Version\$Compiler\lib\cmake\Qt6",
        "$env:ProgramFiles(x86)\Qt\$Version\$Compiler\lib\cmake\Qt6"
    )
    
    foreach ($path in $possiblePaths) {
        if (Test-Path $path) {
            return $path
        }
    }
    
    return $null
}

# Check for CMake
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "ERROR: CMake is not installed or not in PATH." -ForegroundColor Red
    Write-Host "Please install CMake or run: .\scripts\install-windows.ps1" -ForegroundColor Yellow
    exit 1
}

# Find Qt
$qtDir = Find-QtInstallation -Version $QtVersion -Compiler $Compiler
if (-not $qtDir) {
    Write-Host "ERROR: Qt $QtVersion not found." -ForegroundColor Red
    Write-Host ""
    Write-Host "Please:" -ForegroundColor Yellow
    Write-Host "1. Install Qt $QtVersion with $Compiler components" -ForegroundColor White
    Write-Host "2. Set Qt6_DIR environment variable:" -ForegroundColor White
    Write-Host "   `$env:Qt6_DIR = `"C:\Qt\$QtVersion\$Compiler\lib\cmake\Qt6`"" -ForegroundColor Cyan
    Write-Host "3. Or run: .\scripts\install-windows.ps1" -ForegroundColor White
    exit 1
}

Write-Host "Using Qt6_DIR: $qtDir" -ForegroundColor Green
Write-Host "Build type: $BuildType" -ForegroundColor Green
Write-Host "Build directory: $BuildDir" -ForegroundColor Green
Write-Host ""

# Create build directory
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
    Write-Host "Created build directory: $BuildDir" -ForegroundColor Gray
}

# Configure CMake
Write-Host "Configuring CMake..." -ForegroundColor Yellow
$cmakeArgs = @(
    "-B", $BuildDir,
    "-S", ".",
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DBUILD_SHARED_LIBS=ON",
    "-DBUILD_TESTS=OFF",
    "-DQt6_DIR=$qtDir"
)

$process = Start-Process -FilePath "cmake" -ArgumentList $cmakeArgs -NoNewWindow -Wait -PassThru

if ($process.ExitCode -ne 0) {
    Write-Host ""
    Write-Host "ERROR: CMake configuration failed." -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "✓ CMake configuration successful!" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "  make build" -ForegroundColor White
Write-Host "  OR" -ForegroundColor Gray
Write-Host "  .\scripts\build-windows.ps1" -ForegroundColor White
Write-Host ""
