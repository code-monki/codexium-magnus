# Codexium Magnus - Windows Build Script
# Builds the project using CMake

param(
    [string]$BuildType = "Release",
    [string]$BuildDir = "build",
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Codexium Magnus - Build" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check for CMake
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "ERROR: CMake is not installed or not in PATH." -ForegroundColor Red
    exit 1
}

# Clean if requested
if ($Clean) {
    if (Test-Path $BuildDir) {
        Write-Host "Cleaning build directory..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force $BuildDir
        Write-Host "✓ Build directory cleaned" -ForegroundColor Green
    }
}

# Check if CMake is configured
if (-not (Test-Path (Join-Path $BuildDir "CMakeCache.txt"))) {
    Write-Host "CMake not configured. Running configuration..." -ForegroundColor Yellow
    & "$PSScriptRoot\configure-windows.ps1" -BuildType $BuildType -BuildDir $BuildDir
    if ($LASTEXITCODE -ne 0) {
        exit 1
    }
}

Write-Host "Building project..." -ForegroundColor Yellow
Write-Host "Build type: $BuildType" -ForegroundColor Gray
Write-Host "Build directory: $BuildDir" -ForegroundColor Gray
Write-Host ""

# Build
$process = Start-Process -FilePath "cmake" -ArgumentList "--build", $BuildDir, "--config", $BuildType -NoNewWindow -Wait -PassThru

if ($process.ExitCode -ne 0) {
    Write-Host ""
    Write-Host "ERROR: Build failed." -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "✓ Build successful!" -ForegroundColor Green
Write-Host ""
Write-Host "Executable location:" -ForegroundColor Cyan
$exePath = Join-Path $BuildDir "src\codexium-magnus\$BuildType\codexium-magnus.exe"
if (Test-Path $exePath) {
    Write-Host "  $exePath" -ForegroundColor White
} else {
    # Try alternative path
    $exePath = Join-Path $BuildDir "src\codexium-magnus\codexium-magnus.exe"
    if (Test-Path $exePath) {
        Write-Host "  $exePath" -ForegroundColor White
    }
}
Write-Host ""
