# Codexium Magnus - Windows Scripts

This directory contains PowerShell scripts to simplify development on Windows, complementing the Makefile functionality available on Linux and macOS.

## Scripts

### `install-windows.ps1`
Installs dependencies and sets up the development environment.

**Usage:**
```powershell
.\scripts\install-windows.ps1
```

**Options:**
- `-SkipQt` - Skip Qt installation check
- `-SkipCMake` - Skip CMake installation check
- `-SkipCompiler` - Skip compiler check
- `-SkipLibsodium` - Skip libsodium check
- `-QtVersion "6.9.3"` - Specify Qt version (default: 6.9.3)
- `-QtInstallPath "C:\Qt"` - Specify Qt installation path
- `-Compiler "msvc2019_64"` - Specify compiler variant

**Features:**
- Checks for Git, CMake, C++ compiler, Qt, and libsodium
- Attempts to install CMake via winget or Chocolatey (if available)
- Finds Qt installation automatically
- Sets Qt6_DIR environment variable
- Provides clear next steps

### `configure-windows.ps1`
Configures the CMake build system.

**Usage:**
```powershell
.\scripts\configure-windows.ps1
```

**Options:**
- `-BuildType "Release"` - Build type (Release/Debug)
- `-BuildDir "build"` - Build directory
- `-QtVersion "6.9.3"` - Qt version
- `-Compiler "msvc2019_64"` - Compiler variant

**Features:**
- Finds Qt installation automatically
- Creates build directory if needed
- Configures CMake with correct settings

### `build-windows.ps1`
Builds the project.

**Usage:**
```powershell
.\scripts\build-windows.ps1
```

**Options:**
- `-BuildType "Release"` - Build type (Release/Debug)
- `-BuildDir "build"` - Build directory
- `-Clean` - Clean build directory before building

**Features:**
- Automatically configures if needed
- Builds the project
- Shows executable location

### `run-windows.ps1`
Builds and runs the application.

**Usage:**
```powershell
.\scripts\run-windows.ps1
```

**Options:**
- `-BuildType "Release"` - Build type
- `-BuildDir "build"` - Build directory

**Features:**
- Builds the project
- Runs the executable

## Quick Start

1. **Install dependencies:**
   ```powershell
   .\scripts\install-windows.ps1
   ```

2. **Configure and build:**
   ```powershell
   .\scripts\configure-windows.ps1
   .\scripts\build-windows.ps1
   ```

3. **Run:**
   ```powershell
   .\scripts\run-windows.ps1
   ```

## Alternative: Using Makefile

If you have Make installed (via Chocolatey, Git Bash, or WSL), you can also use the Makefile:

```bash
make configure
make build
make run
```

## Requirements

- **PowerShell 5.1+** (included with Windows 10/11)
- **Git** (for cloning the repository)
- **CMake 3.20+** (will be installed automatically if possible)
- **Qt 6.9.3** (must be installed manually via Qt Online Installer)
- **C++ Compiler** (MSVC 2019+ or MinGW)

## Troubleshooting

### Script Execution Policy

If you get an execution policy error, run:
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### Qt Not Found

If Qt is not found automatically:
1. Install Qt via Qt Online Installer
2. Set environment variable manually:
   ```powershell
   $env:Qt6_DIR = "C:\Qt\6.9.3\msvc2019_64\lib\cmake\Qt6"
   ```
3. Or specify path in script parameters

### CMake Not Found

The script will attempt to install CMake via:
1. winget (Windows 10/11)
2. Chocolatey (if installed)

If both fail, install CMake manually from https://cmake.org/download/
