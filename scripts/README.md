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
- `-InstallQt` - Automatically download and install Qt (interactive)
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
- **Can automatically download and install Qt** (with `-InstallQt` flag or when prompted)
- Finds Qt installation automatically
- Sets Qt6_DIR environment variable
- Provides clear next steps

**Qt Installation:**
- The script can automatically download and install Qt using the Qt Online Installer
- Use `-InstallQt` flag to force automatic installation
- Or the script will prompt you if Qt is not found
- Installation takes 10-30 minutes depending on internet speed
- Requires accepting Qt's open source license

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

You can use either PowerShell scripts (`.ps1`) or batch files (`.bat`). The batch files are wrappers that automatically bypass PowerShell execution policy.

### Using PowerShell Scripts

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

### Using Batch Files (Alternative)

If you prefer cmd.exe or want to avoid execution policy issues:

1. **Install dependencies:**
   ```cmd
   .\scripts\install-windows.bat
   ```

2. **Configure and build:**
   ```cmd
   .\scripts\configure-windows.bat
   .\scripts\build-windows.bat
   ```

3. **Run:**
   ```cmd
   .\scripts\run-windows.bat
   ```

**Note:** The batch files are simple wrappers that call the PowerShell scripts with execution policy bypassed. All functionality is the same.

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

If you get an execution policy error when running `.ps1` files directly, you have two options:

1. **Use the batch files instead** (recommended):
   ```cmd
   .\scripts\install-windows.bat
   ```
   The batch files automatically bypass execution policy.

2. **Or change PowerShell execution policy:**
   ```powershell
   Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
   ```

3. **Or run PowerShell with bypass flag:**
   ```powershell
   PowerShell -ExecutionPolicy Bypass -File .\scripts\install-windows.ps1
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
