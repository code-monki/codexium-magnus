# Codexium Magnus - Windows Run Script
# Builds and runs the application

param(
    [string]$BuildType = "Release",
    [string]$BuildDir = "build"
)

$ErrorActionPreference = "Stop"

Write-Host "Building and running Codexium Magnus..." -ForegroundColor Cyan
Write-Host ""

# Build first
& "$PSScriptRoot\build-windows.ps1" -BuildType $BuildType -BuildDir $BuildDir
if ($LASTEXITCODE -ne 0) {
    exit 1
}

# Find executable
$exePaths = @(
    Join-Path $BuildDir "src\codexium-magnus\$BuildType\codexium-magnus.exe",
    Join-Path $BuildDir "src\codexium-magnus\codexium-magnus.exe"
)

$exePath = $null
foreach ($path in $exePaths) {
    if (Test-Path $path) {
        $exePath = $path
        break
    }
}

if (-not $exePath) {
    Write-Host "ERROR: Executable not found." -ForegroundColor Red
    Write-Host "Expected locations:" -ForegroundColor Yellow
    foreach ($path in $exePaths) {
        Write-Host "  $path" -ForegroundColor Gray
    }
    exit 1
}

Write-Host "Running: $exePath" -ForegroundColor Green
Write-Host ""

# Run the application
& $exePath
