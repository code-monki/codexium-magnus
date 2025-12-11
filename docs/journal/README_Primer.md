# Codexium Magnus — Restart Primer

**Date**: 2025-11-11  
**Status**: Documentation migration complete, ready for Qt 6 implementation

---

## Quick Status

- ✅ **All documentation converted** from Markdown to AsciiDoc
- ✅ **Qt 6 migration path** fully documented
- ✅ **Developer journal** system established
- ⏳ **Implementation**: Ready to begin Phase 1 (Qt static build setup)

---

## Key Decisions Made

### 1. Qt 6 Migration (Complete Cutover)
- **Framework**: Qt 6 (Widgets + WebEngine), statically linked under GPL-3
- **Language**: C++17/20 (full migration, no .NET retention)
- **Approach**: "Knife-edge cutover" (acceptable at PoC stage)
- **Build System**: CMake with static linking configuration

### 2. UI Styling Strategy
- **Base Style**: Qt Fusion
- **Customization**: Custom stylesheets for modern appearance
- **Theming**: Token-based system (matches FR-2)
- **Default Themes**: Light, Sepia, Dark
- **Custom Themes**: User-defined theme support
- **Design Aesthetic**: Flat design (minimal shadows, clean borders)

### 3. Documentation Format
- **Format**: AsciiDoc (converted from Markdown)
- **Build Tooling**: AsciiDoctor for HTML/PDF, Pandoc for DOCX
- **Diagrams**: Mermaid (SVG preferred, PNG fallback)
- **Location**: `docs/asciidoc/` (sources), `docs/build/` (outputs)

---

## Current Project State

### Documentation (Complete)
All documents converted to AsciiDoc and updated for Qt migration:

1. **requirements.adoc** (v1.3) - Updated for Qt 6, C++17/20, added sepia theme
2. **architecture.adoc** (v1.2) - Qt Widgets/WebEngine, ThemeManager subsystem
3. **qt6-migration.adoc** (v2.0) - Complete migration path with styling strategy
4. **hld.adoc** (v1.1) - Updated subsystems for Qt
5. **detailed-design.adoc** (v1.2) - C++ models, ThemeManager section
6. **implementation-plan.adoc** (v2.0) - Qt/C++ implementation phases
7. **project-rules.adoc** (v1.1) - C++/Qt coding standards

### Build System
- **Makefile**: Created in `docs/` for automated documentation builds
- **Status**: Ready to test (requires AsciiDoctor, Mermaid CLI, Pandoc)

### Developer Journal
- **Structure**: `docs/journal/YYYY/YYYY-MM-DD.md`
- **Initial Entry**: 2025-11-11 (Qt migration planning, documentation conversion)
- **Index**: `docs/journal/index.adoc`

### Source Code
- **Current State**: Avalonia/.NET PoC (reference only)
- **Next Step**: Begin Qt 6 implementation (Phase 1)

---

## Architecture Overview

### Technology Stack
- **UI**: Qt Widgets (Fusion style + custom stylesheets)
- **Rendering**: Qt WebEngine (Chromium-based, replaces CEF)
- **Language**: C++17/20
- **Build**: CMake with static linking
- **Database**: SQLite3 via Qt SQL
- **Crypto**: Ed25519 via libsodium (C++ library)
- **Testing**: Qt Test framework, Playwright/Chrome DevTools Protocol

### Key Components
1. **Viewer Shell** (Qt Widgets) - MainWindow, navigation, search panes
2. **Rendering Host** (Qt WebEngine) - HTML rendering, link/print interception
3. **ThemeManager** - Theme switching, stylesheet management, token system
4. **Content Subsystem** - Cartridge I/O via Qt SQL
5. **Search Subsystem** - FTS5 queries via Qt SQL
6. **Security Subsystem** - Ed25519 verification (libsodium)

---

## Implementation Phases

### Phase 1: Foundation & Build System (2-3 weeks)
- Build Qt 6 statically (GPL-3) or obtain pre-built
- Set up CMake with static linking
- Create minimal Qt Widgets shell
- Integrate Qt WebEngine
- Verify cross-platform static compilation

### Phase 2: Core Services & Domain Layer (3-4 weeks)
- Port domain models to C++
- Implement configuration resolver (4-layer)
- Implement report writer
- Implement storage layer (Qt SQL)
- Implement cartridge reader

### Phase 3: UI Shell & Integration (4-5 weeks)
- Migrate MainWindow to Qt Widgets
- Implement navigation, search, document view
- Implement ThemeManager with default themes
- Integrate services with UI
- Implement search service (FTS5)

### Phase 4: Feature Completion & Security (3-4 weeks)
- Complete all FRs
- Implement Ed25519 verification
- Add custom theme support
- Typography and bibliography features

### Phase 5: Testing & Deployment (3-4 weeks)
- Port test harnesses to Qt Test
- Performance optimization
- Cross-platform static build verification
- Documentation updates

**Total Timeline**: 15-20 weeks (phases can overlap)

---

## Important Files & Locations

### Documentation
- **Source**: `docs/asciidoc/*.adoc`
- **Build Output**: `docs/build/{html,pdf,docx}/`
- **Diagrams**: `docs/asciidoc/diagrams/*.mmd` → `docs/build/diagrams/`
- **Journal**: `docs/journal/YYYY/YYYY-MM-DD.md`
- **Build**: `cd docs && make all`

### Source Code (Future)
- **Main App**: `src/codexium-magnus/` (Qt Widgets)
- **Core Library**: `src/codexium-magnus-core/` (C++ services)
- **Storage**: `src/codexium-magnus-storage/` (Qt SQL)
- **Tests**: `tests/` (Qt Test framework)

### Key Configuration Files
- **CMake**: `CMakeLists.txt` (to be created)
- **Qt Build**: Qt 6 static build required
- **Documentation**: `docs/Makefile`, `docs/asciidoc/attributes.adoc`

---

## Next Immediate Steps

1. **Test Documentation Build**
   
   cd docs
   make install-deps  # Install AsciiDoctor, Mermaid CLI
   make all           # Generate HTML, PDF, DOCX
   2. **Create Mermaid Diagrams**
   - Architecture overview
   - Component diagram
   - Theme system architecture
   - Migration phases timeline
   - Data flow diagrams

3. **Set Up Qt Development Environment**
   - Build Qt 6 statically (GPL-3) or obtain pre-built
   - Verify CMake 3.20+ installed
   - Set up C++17 compiler (GCC, Clang, or MSVC)

4. **Begin Phase 1 Implementation**
   - Create CMake project structure
   - Configure static linking
   - Create minimal Qt Widgets MainWindow
   - Integrate Qt WebEngine

---

## Key Decisions Reference

### Why Qt 6?
- GPL-3 compatible with static linking
- Cross-platform (Windows, macOS, Linux)
- Qt WebEngine ≈ CEF capabilities (both Chromium-based)
- Single language stack (C++), no .NET dependency
- Modern, maintainable framework

### Why Fusion + Stylesheets?
- Cross-platform consistency (avoids native platform styles)
- Full control over appearance
- Supports token-based theming (FR-2 requirement)
- No external dependencies
- Can achieve modern, flat design aesthetic

### Why Static Linking?
- Single executable deployment (no DLL dependencies)
- Simplified distribution
- No runtime Qt version conflicts
- Smaller deployment footprint
- GPL-3 permits static linking

---

## Developer Journal

**Latest Entry**: 2025-11-11
- Qt 6 migration planning
- Styling strategy decisions
- Documentation format conversion
- All artifacts converted to AsciiDoc

**See**: `docs/journal/2025/2025-11-11.md` for full details

---

## Git Repository Status

**Current Branch**: `main` (or worktree branch)  
**Remote**: `origin` → `https://github.com/code-monki/codexium-magnus.git`  
**Note**: Remote authentication may need token configuration for push access

---

## Quick Commands

### Documentation
cd docs
make all          # Build all formats
make html         # HTML only
make pdf          # PDF only
make diagrams     # Generate diagrams only
make clean        # Clean build outputs
### Git (when ready)ash
git remote set-url origin https://code-monki:TOKEN@github.com/code-monki/codexium-magnus.git
git push --set-upstream origin main### Qt Build (Phase 1)
# After Qt 6 static build is ready
cmake -B build -S . -DBUILD_SHARED_LIBS=OFF
cmake --build build---

## Important Notes

1. **PoC Stage**: Project is at proof-of-concept stage, enabling complete cutover approach
2. **No .NET**: All .NET code is reference only; full C++/Qt rewrite planned
3. **Static Linking**: Qt must be built statically with GPL-3 configuration
4. **Documentation First**: All design decisions documented before implementation
5. **Developer Journal**: Document significant decisions and progress

---

## Questions to Resolve

1. **Qt Static Build**: Build from source or use pre-built? (Phase 1 decision)
2. **Mermaid Diagrams**: Create initial diagrams before or during implementation?
3. **Git Remote**: Resolve authentication for push access
4. **IDE Setup**: Qt Creator vs VS Code for development?

---

## Related Documents

- **Requirements**: `docs/asciidoc/requirements.adoc`
- **Architecture**: `docs/asciidoc/architecture.adoc`
- **Migration Path**: `docs/asciidoc/qt6-migration.adoc` (most detailed)
- **Implementation Plan**: `docs/asciidoc/implementation-plan.adoc`
- **Project Rules**: `docs/asciidoc/project-rules.adoc`
- **Developer Journal**: `docs/journal/2025/2025-11-11.md`

---

## When Resuming Work

1. Read this primer
2. Review latest journal entry (`docs/journal/2025/Y0YYY-MM-DD.md`)
3. Check migration path document for current phase details
4. Verify development environment setup
5. Continue with next phase tasks

---

**Last Updated**: 2025-11-11  
**Next Review**: When Phase 1 begins or significant progress made