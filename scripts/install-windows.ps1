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
    
    # Convert version to component format (e.g., "6.9.3" -> "693")
    # Note: Qt component IDs use version without dots
    # Format may vary by Qt version - if installation fails, use search command to find correct IDs
    $versionParts = $Version.Split('.')
    $major = $versionParts[0]
    $minor = $versionParts[1]
    $patch = $versionParts[2]
    $versionId = "$major$minor$patch"
    
    return "qt.qt6.$versionId.win64_$compilerId"
}

# Function to list available Qt versions
function Get-AvailableQtVersions {
    param([string]$InstallerPath)
    
    Write-Host "Searching for available Qt versions..." -ForegroundColor Yellow
    
    try {
        # Search for all Qt 6.x versions
        $searchArgs = @("search", "--filter-packages", "DisplayName=Qt")
        $searchOutput = & $InstallerPath $searchArgs 2>&1 | Out-String
        
        # Extract version numbers from output
        $versions = @()
        if ($searchOutput -match "Version=(\d+\.\d+\.\d+)") {
            $matches = [regex]::Matches($searchOutput, "Version=(\d+\.\d+\.\d+)")
            foreach ($match in $matches) {
                $version = $match.Groups[1].Value
                if ($versions -notcontains $version) {
                    $versions += $version
                }
            }
        }
        
        if ($versions.Count -gt 0) {
            Write-Host "Available Qt versions found:" -ForegroundColor Green
            $versions | Sort-Object -Descending | ForEach-Object {
                Write-Host "  - $_" -ForegroundColor White
            }
            return $versions
        } else {
            Write-Host "[WARN] Could not determine available Qt versions from search" -ForegroundColor Yellow
            return @()
        }
    } catch {
        Write-Host "[WARN] Could not search for Qt versions: $_" -ForegroundColor Yellow
        return @()
    }
}

# Function to find correct component IDs by searching
function Find-QtComponentIds {
    param(
        [string]$InstallerPath,
        [string]$Version,
        [string]$Compiler
    )
    
    Write-Host "Attempting to find correct component IDs..." -ForegroundColor Yellow
    
    # Try different possible component ID formats
    $versionParts = $Version.Split('.')
    $major = $versionParts[0]
    $minor = $versionParts[1]
    $patch = $versionParts[2]
    
    # Possible version formats
    $versionFormats = @(
        "$major$minor$patch",      # 693
        "$major$($minor.PadLeft(2, '0'))$patch",  # 6093
        "$major.$minor.$patch"     # 6.9.3 (with dots)
    )
    
    $compilerId = $Compiler
    if ($Compiler -eq "msvc2019_64") {
        $compilerId = "msvc2019_64"
    } elseif ($Compiler -eq "msvc2022_64") {
        $compilerId = "msvc2022_64"
    }
    
    $foundComponents = @{
        Base = $null
        WebEngine = $null
    }
    
    # Try searching for components - handle errors gracefully
    foreach ($versionId in $versionFormats) {
        $baseId = "qt.qt6.$versionId.win64_$compilerId"
        $webEngineId = "qt.qt6.$versionId.win64_$compilerId.qtwebengine"
        
        # Try to search for these components
        try {
            # Search for base component
            $searchArgs = @("search", "--filter-packages", "Id=$baseId")
            $searchOutput = & $InstallerPath $searchArgs 2>&1 | Out-String
            
            if ($LASTEXITCODE -eq 0 -and ($searchOutput -match $baseId -or $searchOutput -match "qt\.qt6")) {
                $foundComponents.Base = $baseId
                Write-Host "  [OK] Found base component: $baseId" -ForegroundColor Green
                
                # Try to find WebEngine - try multiple search patterns
                $webEnginePatterns = @(
                    $webEngineId,
                    "qtwebengine",
                    "WebEngine"
                )
                
                foreach ($pattern in $webEnginePatterns) {
                    try {
                        $webEngineSearchArgs = @("search", "--filter-packages", "Id=$pattern")
                        $webEngineOutput = & $InstallerPath $webEngineSearchArgs 2>&1 | Out-String
                        
                        if ($LASTEXITCODE -eq 0 -and ($webEngineOutput -match $webEngineId -or $webEngineOutput -match "webengine" -or $webEngineOutput -match "WebEngine")) {
                            $foundComponents.WebEngine = $webEngineId
                            Write-Host "  [OK] Found WebEngine component: $webEngineId" -ForegroundColor Green
                            break
                        }
                    } catch {
                        continue
                    }
                }
                
                # If we found base but not WebEngine, that's okay - user can install it manually
                if ($foundComponents.Base) {
                    break
                }
            }
        } catch {
            # Silently continue to next format
            continue
        }
    }
    
    if (-not $foundComponents.Base) {
        Write-Host "  [INFO] Could not verify component IDs via search, will use calculated format" -ForegroundColor Yellow
    } elseif (-not $foundComponents.WebEngine) {
        Write-Host "  [WARN] Base component found but WebEngine ID not verified" -ForegroundColor Yellow
        Write-Host "         WebEngine may need to be installed separately via GUI" -ForegroundColor Yellow
    }
    
    return $foundComponents
}

# Function to install Qt automatically
function Install-Qt {
    param(
        [string]$Version,
        [string]$InstallPath,
        [string]$Compiler
    )
    
    Write-Host "Downloading Qt Online Installer..." -ForegroundColor Yellow
    
    # Qt installer URL - updated filename as of 2025
    $installerUrl = "https://download.qt.io/official_releases/online_installers/qt-online-installer-windows-x64-online.exe"
    $installerPath = Join-Path $env:TEMP "qt-online-installer-windows-x64-online.exe"
    
    try {
        # Download installer
        Write-Host "Downloading from: $installerUrl" -ForegroundColor Gray
        Invoke-WebRequest -Uri $installerUrl -OutFile $installerPath -UseBasicParsing -ErrorAction Stop
        Write-Host "[OK] Download complete" -ForegroundColor Green
    } catch {
        Write-Host "[ERROR] Failed to download Qt installer: $_" -ForegroundColor Red
        Write-Host ""
        Write-Host "The automatic download failed. This may be due to:" -ForegroundColor Yellow
        Write-Host "  - Qt has changed their download URL structure" -ForegroundColor White
        Write-Host "  - Network connectivity issues" -ForegroundColor White
        Write-Host "  - The installer URL is temporarily unavailable" -ForegroundColor White
        Write-Host ""
        Write-Host "Please download the Qt Online Installer manually:" -ForegroundColor Yellow
        Write-Host "1. Visit: https://www.qt.io/download-open-source" -ForegroundColor White
        Write-Host "2. Click 'Download the Qt Online Installer' for Windows" -ForegroundColor White
        Write-Host "3. Run the installer" -ForegroundColor White
        Write-Host "   - If prompted for email/password, press Enter to skip (for open source)" -ForegroundColor White
        Write-Host "   - Select 'Open Source' license when prompted" -ForegroundColor White
        Write-Host "4. Install Qt $Version with $Compiler components" -ForegroundColor White
        Write-Host "5. Make sure to install Qt WebEngine component (required)" -ForegroundColor White
        Write-Host "6. After installation, run this script again" -ForegroundColor White
        Write-Host ""
        return $false
    }
    
    Write-Host ""
    Write-Host "Verifying Qt $Version availability..." -ForegroundColor Yellow
    
    # First, try to find correct component IDs by searching
    $foundComponents = Find-QtComponentIds -InstallerPath $installerPath -Version $Version -Compiler $Compiler
    
    $componentId = $null
    $webEngineComponent = $null
    
    # Calculate component IDs as fallback
    $calculatedBaseId = Get-QtComponentId -Version $Version -Compiler $Compiler
    $versionParts = $Version.Split('.')
    $major = $versionParts[0]
    $minor = $versionParts[1]
    $patch = $versionParts[2]
    $versionId = "$major$minor$patch"
    $calculatedWebEngineId = "qt.qt6.$versionId.win64_$Compiler.qtwebengine"
    
    if ($foundComponents.Base) {
        $componentId = $foundComponents.Base
        # Use found WebEngine ID if available, otherwise use calculated
        if ($foundComponents.WebEngine) {
            $webEngineComponent = $foundComponents.WebEngine
        } else {
            $webEngineComponent = $calculatedWebEngineId
            Write-Host "[WARN] Using calculated WebEngine ID (not verified via search)" -ForegroundColor Yellow
        }
        Write-Host "[OK] Using verified component IDs for Qt $Version" -ForegroundColor Green
    } else {
        # Fall back to calculated component IDs
        Write-Host "[INFO] Could not verify component IDs via search, using calculated format" -ForegroundColor Yellow
        $componentId = $calculatedBaseId
        $webEngineComponent = $calculatedWebEngineId
    }
    
    Write-Host ""
    Write-Host "Installing Qt $Version..." -ForegroundColor Yellow
    Write-Host "This may take 10-30 minutes depending on your internet connection." -ForegroundColor Gray
    Write-Host ""
    Write-Host "IMPORTANT: If the installer prompts for an email and password:" -ForegroundColor Cyan
    Write-Host "  - This is for Qt commercial license checking" -ForegroundColor White
    Write-Host "  - For open source development, you can skip this" -ForegroundColor White
    Write-Host "  - Simply press Enter (leave fields empty) when prompted" -ForegroundColor White
    Write-Host "  - The installer will continue with open source components" -ForegroundColor White
    Write-Host ""
    
    Write-Host "Attempting to install components:" -ForegroundColor Gray
    Write-Host "  Base: $componentId" -ForegroundColor Gray
    Write-Host "  WebEngine: $webEngineComponent" -ForegroundColor Gray
    Write-Host ""
    
    # Build installer arguments
    # Note: --email and --password with empty values may skip the prompt in some versions
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
            Write-Host ""
            Write-Host "The automatic installation failed. This usually means:" -ForegroundColor Yellow
            Write-Host "  - Qt $Version may not be available in the repository" -ForegroundColor White
            Write-Host "  - The component IDs may have changed" -ForegroundColor White
            Write-Host "  - The version may be in a different repository" -ForegroundColor White
            Write-Host ""
            
            # Try to list available Qt versions
            Write-Host "Checking for available Qt versions..." -ForegroundColor Yellow
            $availableVersions = Get-AvailableQtVersions -InstallerPath $installerPath
            if ($availableVersions.Count -gt 0) {
                Write-Host ""
                if ($availableVersions -contains $Version) {
                    Write-Host "Note: Qt $Version appears to be available, but component IDs may be incorrect." -ForegroundColor Yellow
                } else {
                    Write-Host "Note: Qt $Version was not found in available versions." -ForegroundColor Yellow
                    Write-Host "Consider using one of the available versions listed above." -ForegroundColor Yellow
                }
                Write-Host ""
            }
            
            Write-Host "RECOMMENDED: Use the GUI installer (most reliable method):" -ForegroundColor Cyan
            Write-Host "  1. Run: $installerPath" -ForegroundColor White
            Write-Host "  2. If prompted for email/password, press Enter to skip" -ForegroundColor White
            Write-Host "  3. Select 'Open Source' license" -ForegroundColor White
            if ($availableVersions.Count -gt 0) {
                Write-Host "  4. Browse available Qt versions and select Qt $Version (or a compatible version)" -ForegroundColor White
            } else {
                Write-Host "  4. Browse available Qt versions and select Qt $Version (or latest available)" -ForegroundColor White
            }
            Write-Host "  5. Select $Compiler components" -ForegroundColor White
            Write-Host "  6. Make sure to install Qt WebEngine component (required)" -ForegroundColor White
            Write-Host ""
            Write-Host "The installer has been saved to: $installerPath" -ForegroundColor Gray
            Write-Host "After manual installation, run this script again to verify setup." -ForegroundColor Gray
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
        Write-Host "This may take several minutes. Please wait..." -ForegroundColor Gray
        $wingetProcess = Start-Process -FilePath "winget" -ArgumentList "install", "-e", "--id", "Kitware.CMake", "--accept-package-agreements", "--accept-source-agreements" -Wait -PassThru -NoNewWindow
        if ($wingetProcess.ExitCode -eq 0) {
            Write-Host "CMake installation completed. Refreshing PATH..." -ForegroundColor Green
            $machinePath = [System.Environment]::GetEnvironmentVariable("Path", "Machine")
            $userPath = [System.Environment]::GetEnvironmentVariable("Path", "User")
            $env:Path = $machinePath + ";" + $userPath
            $cmakeFound = Test-Command "cmake"
        } else {
            Write-Host "WARNING: winget installation returned exit code: $($wingetProcess.ExitCode)" -ForegroundColor Yellow
        }
    }
    
    # Try Chocolatey if winget failed
    if (-not $cmakeFound -and (Test-Command "choco")) {
        Write-Host "Installing CMake via Chocolatey..." -ForegroundColor Yellow
        Write-Host "This may take several minutes. Please wait..." -ForegroundColor Gray
        if ($isAdmin) {
            $chocoProcess = Start-Process -FilePath "choco" -ArgumentList "install", "cmake", "-y" -Wait -PassThru -NoNewWindow
            if ($chocoProcess.ExitCode -eq 0) {
                Write-Host "CMake installation completed. Refreshing PATH..." -ForegroundColor Green
                $machinePath = [System.Environment]::GetEnvironmentVariable("Path", "Machine")
                $userPath = [System.Environment]::GetEnvironmentVariable("Path", "User")
                $env:Path = $machinePath + ";" + $userPath
                $cmakeFound = Test-Command "cmake"
            } else {
                Write-Host "WARNING: Chocolatey installation returned exit code: $($chocoProcess.ExitCode)" -ForegroundColor Yellow
            }
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
            Write-Host "2. Run the installer" -ForegroundColor White
            Write-Host "   - If prompted for email/password (commercial license check), press Enter to skip" -ForegroundColor White
            Write-Host "   - Select 'Open Source' license when prompted" -ForegroundColor White
            Write-Host "3. Install Qt $QtVersion with $Compiler components" -ForegroundColor White
            Write-Host "4. Install Qt WebEngine component (required)" -ForegroundColor White
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
        Write-Host "   - Run the installer" -ForegroundColor Gray
        Write-Host "   - If prompted for email/password, press Enter to skip (for open source)" -ForegroundColor Gray
        Write-Host "   - Select 'Open Source' license when prompted" -ForegroundColor Gray
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

# Check for libsodium (optional - only needed for signature verification)
if (-not $SkipLibsodium) {
    $libsodiumFound = $false
    
    # Check if libsodium is installed via vcpkg
    if (Test-Command "vcpkg") {
        try {
            $vcpkgList = vcpkg list libsodium 2>&1
            if ($vcpkgList -match "libsodium") {
                $libsodiumFound = $true
                Write-Host "[OK] libsodium found (installed via vcpkg)" -ForegroundColor Green
            } else {
                Write-Host "[INFO] vcpkg found, but libsodium is not installed" -ForegroundColor Gray
            }
        } catch {
            Write-Host "[INFO] Could not check vcpkg for libsodium" -ForegroundColor Gray
        }
    }
    
    # Check if libsodium is in common installation locations
    if (-not $libsodiumFound) {
        $possiblePaths = @(
            "$env:ProgramFiles\libsodium",
            "$env:ProgramFiles(x86)\libsodium",
            "$env:USERPROFILE\libsodium",
            "$env:VCPKG_ROOT\installed\x64-windows\include\sodium"
        )
        
        foreach ($path in $possiblePaths) {
            if (Test-Path $path) {
                $libsodiumFound = $true
                Write-Host "[OK] libsodium found at: $path" -ForegroundColor Green
                break
            }
        }
    }
    
    if (-not $libsodiumFound) {
        Write-Host "[INFO] libsodium not found (optional dependency)" -ForegroundColor Cyan
        Write-Host "  Note: libsodium is only needed for signature verification features." -ForegroundColor Gray
        Write-Host "  The application will work without it, but signature verification will be disabled." -ForegroundColor Gray
        Write-Host ""
        Write-Host "  To install libsodium (optional):" -ForegroundColor Yellow
        if (Test-Command "vcpkg") {
            Write-Host "    vcpkg install libsodium:x64-windows" -ForegroundColor White
        } else {
            Write-Host "    Install vcpkg, then run: vcpkg install libsodium:x64-windows" -ForegroundColor White
            Write-Host "    Or download from: https://download.libsodium.org/libsodium/releases/" -ForegroundColor White
        }
    }
} else {
    Write-Host "[INFO] libsodium check skipped" -ForegroundColor Cyan
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
