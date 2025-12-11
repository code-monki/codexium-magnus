# Codexium Magnus - Installation Guide

This guide provides step-by-step instructions for installing dependencies and building Codexium Magnus on Windows, macOS, and Linux.

## Prerequisites

### All Platforms

- **CMake** 3.20 or later
- **C++17/20 compiler**:
  - Windows: MSVC 2019+ or MinGW-w64
  - macOS: Xcode Command Line Tools (Clang)
  - Linux: GCC 8+ or Clang 8+
- **Git** (for cloning the repository)
- **Qt 6.9.3** (or compatible version) with the following modules:
  - Qt6::Core
  - Qt6::Widgets
  - Qt6::WebEngineWidgets
  - Qt6::Sql
  - Qt6::Network
  - Qt6::Test (for building tests)

### Optional Dependencies

- **libsodium** (for Ed25519 signature verification)
  - Windows: vcpkg or pre-built binaries
  - macOS: Homebrew (`brew install libsodium`)
  - Linux: Package manager (`apt-get install libsodium-dev` or equivalent)

---

## Windows Installation

### Option 1: Qt Online Installer (Recommended)

1. **Download Qt 6.9.3**
   - Visit: https://www.qt.io/download-open-source
   - Download the Qt Online Installer for Windows
   - Run the installer and sign in (create free account if needed)

2. **Install Qt Components**
   - Select installation path (e.g., `C:\Qt` or `%USERPROFILE%\Qt`)
   - Choose Qt 6.9.3 (or latest 6.x)
   - Select the following components:
     - **MSVC 2019 64-bit** (or MinGW 11.2.0 64-bit)
     - **Qt WebEngine** (required)
     - **Qt 5 Compatibility Module** (optional, for legacy code)
   - Complete installation

3. **Set Environment Variables**
   ```cmd
   set Qt6_DIR=%USERPROFILE%\Qt\6.9.3\msvc2019_64\lib\cmake\Qt6
   ```
   Or for MinGW:
   ```cmd
   set Qt6_DIR=%USERPROFILE%\Qt\6.9.3\mingw_64\lib\cmake\Qt6
   ```

4. **Install CMake**
   - Download from: https://cmake.org/download/
   - Install and add to PATH
   - Or use Chocolatey: `choco install cmake`

5. **Install Visual Studio Build Tools** (if using MSVC)
   - Download: https://visualstudio.microsoft.com/downloads/
   - Install "Desktop development with C++" workload
   - Or use Chocolatey: `choco install visualstudio2019buildtools`

6. **Install libsodium** (Optional)
   - Using vcpkg:
     ```cmd
     git clone https://github.com/Microsoft/vcpkg.git
     cd vcpkg
     .\bootstrap-vcpkg.bat
     .\vcpkg install libsodium:x64-windows
     ```
   - Or download pre-built binaries from: https://download.libsodium.org/libsodium/releases/

### Option 2: Using vcpkg (Alternative)

1. **Install vcpkg**
   ```cmd
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```

2. **Install Qt and dependencies**
   ```cmd
   .\vcpkg install qt6-base qt6-webengine qt6-sql qt6-network libsodium:x64-windows
   ```

3. **Set vcpkg toolchain**
   ```cmd
   set CMAKE_TOOLCHAIN_FILE=C:\path\to\vcpkg\scripts\buildsystems\vcpkg.cmake
   ```

### Building on Windows

1. **Open Developer Command Prompt** (for MSVC) or **Git Bash** (for MinGW)

2. **Configure and Build**
   ```cmd
   git clone <repository-url>
   cd codexium-magnus
   make configure
   make build
   ```

   Or manually:
   ```cmd
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release -DQt6_DIR=%USERPROFILE%\Qt\6.9.3\msvc2019_64\lib\cmake\Qt6
   cmake --build . --config Release
   ```

3. **Run the Application**
   ```cmd
   make run
   ```
   Or:
   ```cmd
   .\build\src\codexium-magnus\Release\codexium-magnus.exe
   ```

---

## macOS Installation

### Using Homebrew (Recommended)

1. **Install Homebrew** (if not already installed)
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

2. **Install Qt 6**
   ```bash
   brew install qt@6
   ```

3. **Install CMake**
   ```bash
   brew install cmake
   ```

4. **Install Xcode Command Line Tools**
   ```bash
   xcode-select --install
   ```

5. **Install libsodium** (Optional)
   ```bash
   brew install libsodium
   ```

6. **Set Qt Path** (if Qt is not in default location)
   ```bash
   export Qt6_DIR=$(brew --prefix qt@6)/lib/cmake/Qt6
   ```

### Using Qt Online Installer

1. **Download Qt 6.9.3**
   - Visit: https://www.qt.io/download-open-source
   - Download the Qt Online Installer for macOS
   - Run the installer

2. **Install Qt Components**
   - Select installation path (e.g., `~/Qt`)
   - Choose Qt 6.9.3
   - Select **macOS** components
   - Select **Qt WebEngine**

3. **Set Environment Variable**
   ```bash
   export Qt6_DIR=$HOME/Qt/6.9.3/macos/lib/cmake/Qt6
   ```

### Building on macOS

1. **Clone and Build**
   ```bash
   git clone <repository-url>
   cd codexium-magnus
   make configure
   make build
   ```

2. **Create App Bundle** (Optional)
   ```bash
   make bundle
   ```
   This runs `macdeployqt` to bundle Qt frameworks into the `.app`.

3. **Run the Application**
   ```bash
   make run
   ```
   Or:
   ```bash
   open build/src/codexium-magnus/codexium-magnus.app
   ```

---

## Linux Installation

### Ubuntu/Debian

1. **Install System Dependencies**
   ```bash
   sudo apt-get update
   sudo apt-get install -y \
     build-essential \
     cmake \
     git \
     libgl1-mesa-dev \
     libglu1-mesa-dev \
     libxkbcommon-dev \
     libxcb-xinerama0-dev \
     libxcb-cursor-dev \
     libxcb-icccm4-dev \
     libxcb-image0-dev \
     libxcb-shm0-dev \
     libxcb-keysyms1-dev \
     libxcb-randr0-dev \
     libxcb-render-util0-dev \
     libxcb-xfixes0-dev \
     libxcb-xinerama0-dev \
     libxcb-xkb-dev \
     libxkbcommon-x11-dev \
     libxkbcommon-dev \
     libfontconfig1-dev \
     libfreetype6-dev \
     libx11-dev \
     libx11-xcb-dev \
     libxext-dev \
     libxfixes-dev \
     libxi-dev \
     libxrender-dev \
     libxcb1-dev \
     libxcb-glx0-dev \
     libxcb-keysyms1-dev \
     libxcb-image0-dev \
     libxcb-shm0-dev \
     libxcb-icccm4-dev \
     libxcb-sync-dev \
     libxcb-xfixes0-dev \
     libxcb-shape0-dev \
     libxcb-randr0-dev \
     libxcb-render-util0-dev \
     libxcb-util-dev \
     libxcb-xinerama0-dev \
     libxcb-xkb-dev \
     libxkbcommon-x11-dev \
     libxkbcommon-dev \
     libasound2-dev \
     libpulse-dev \
     libnss3-dev \
     libdbus-1-dev \
     libatk-bridge2.0-dev \
     libdrm-dev \
     libxcomposite-dev \
     libxdamage-dev \
     libxrandr-dev \
     libgbm-dev \
     libxss1 \
     libasound2 \
     libgconf-2-4 \
     libxshmfence-dev \
     libsodium-dev
   ```

2. **Install Qt 6**

   **Option A: Using Qt Online Installer**
   ```bash
   # Download from https://www.qt.io/download-open-source
   # Run installer and install Qt 6.9.3 to ~/Qt
   export Qt6_DIR=$HOME/Qt/6.9.3/gcc_64/lib/cmake/Qt6
   ```

   **Option B: Using Package Manager** (may have older version)
   ```bash
   sudo apt-get install qt6-base-dev qt6-webengine-dev qt6-sql-dev qt6-network-dev
   ```

### Fedora/RHEL/CentOS

1. **Install System Dependencies**
   ```bash
   sudo dnf install -y \
     gcc-c++ \
     cmake \
     git \
     mesa-libGL-devel \
     libxkbcommon-devel \
     fontconfig-devel \
     freetype-devel \
     libX11-devel \
     libXext-devel \
     libXi-devel \
     libXrender-devel \
     libxcb-devel \
     alsa-lib-devel \
     pulseaudio-libs-devel \
     nss-devel \
     dbus-devel \
     atk-devel \
     libsodium-devel
   ```

2. **Install Qt 6**
   ```bash
   sudo dnf install qt6-qtbase-devel qt6-qtwebengine-devel qt6-qtsql-devel qt6-qtnetwork-devel
   ```

### Building on Linux

1. **Clone and Build**
   ```bash
   git clone <repository-url>
   cd codexium-magnus
   make configure
   make build
   ```

2. **Run the Application**
   ```bash
   make run
   ```
   Or:
   ```bash
   ./build/src/codexium-magnus/codexium-magnus
   ```

---

## Verifying Installation

### Check Qt Installation

```bash
# Windows (PowerShell)
$env:Qt6_DIR

# macOS/Linux
echo $Qt6_DIR
```

### Verify CMake Can Find Qt

```bash
cmake --find-package -DNAME=Qt6 -DCOMPILER_ID=GNU -DLANGUAGE=CXX -DMODE=COMPILE
```

### Test Build

```bash
make configure
make build
make test  # Run test suite
```

---

## Troubleshooting

### Qt Not Found

- **Windows**: Ensure `Qt6_DIR` environment variable is set correctly
- **macOS**: Check `brew --prefix qt@6` or verify Qt installation path
- **Linux**: Verify Qt6 packages are installed or set `Qt6_DIR` manually

### WebEngine Not Available

- Ensure Qt WebEngine component is installed
- On Linux, install additional WebEngine dependencies (see system dependencies above)
- Verify with: `qmake -query QT_INSTALL_PREFIX`

### libsodium Not Found

- Install libsodium using platform-specific package manager
- Or build from source: https://github.com/jedisct1/libsodium
- CMake will continue without libsodium (signature verification will be disabled)

### Build Errors

- Ensure all system dependencies are installed
- Check CMake output for missing dependencies
- Verify compiler supports C++17 (use `-std=c++17` flag if needed)

---

## Next Steps

After successful installation:

1. **Build the project**: `make build`
2. **Run tests**: `make test`
3. **Build documentation**: `cd docs && make all`
4. **Run the application**: `make run`

For more information, see:
- `docs/asciidoc/RESTART_PRIMER.md` - Development primer
- `docs/asciidoc/qt6-migration.adoc` - Qt 6 migration details
- `docs/asciidoc/implementation-plan.adoc` - Implementation plan
