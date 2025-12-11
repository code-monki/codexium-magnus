# Codexium Magnus

A cross-platform archival reader built with Qt 6 and WebEngine.

## Quick Start

1. **Install Dependencies**: See [INSTALL.md](INSTALL.md) for platform-specific instructions
2. **Build**: `make build`
3. **Run**: `make run`
4. **Test**: `make test`

## Installation

For detailed installation instructions, see **[INSTALL.md](INSTALL.md)**.

Quick summary:
- **Windows**: Install Qt 6.9.3 via Qt Online Installer, CMake, and MSVC/MinGW
- **macOS**: `brew install qt@6 cmake` (or use Qt Online Installer)
- **Linux**: Install Qt 6 packages via package manager or Qt Online Installer

## Building

```bash
make configure  # Configure CMake
make build      # Build the project
make test       # Run tests
make run        # Build and run
```

## Documentation

See `docs/asciidoc/RESTART_PRIMER.md` for development information.
