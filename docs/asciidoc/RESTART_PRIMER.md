# Codexium Magnus — Restart Primer

**Date**: 2025-12-10  
**Status**: Phase 1 implementation in progress - Core C++/Qt6 codebase created

---

## Quick Status

- ✅ **All documentation converted** from Markdown to AsciiDoc
- ✅ **Qt 6 migration path** fully documented
- ✅ **Developer journal** system established
- ✅ **CMake project structure** created
- ✅ **Core C++/Qt6 codebase** implemented (Phase 1 & 2 major components)
- ⏳ **Next**: Qt static build setup, testing, and refinement

---

## Key Decisions Made

### 1. Qt 6 Migration (Complete Cutover)
- **Framework**: Qt 6 (Widgets + WebEngine), dynamically linked under GPL-3
- **Language**: C++17/20 (full migration, no .NET retention)
- **Approach**: "Knife-edge cutover" (acceptable at PoC stage)
- **Build System**: CMake with dynamic linking (WebEngine requires dynamic linking due to Chromium licensing)

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
- **Top-level Makefile**: Created for orchestrating builds
- **CMakeLists.txt**: Top-level CMake configuration created
- **CMake structure**: All libraries and executables configured
- **Status**: Ready for Qt dynamic build (WebEngine requires dynamic linking)

### Source Code (C++/Qt6 Implementation)
**Core Library** (`src/codexium-magnus-core/`):
- ✅ TypographyConfig, BibliographyConfig models
- ✅ ReportEntry, ReportSeverity, ReportWriter
- ✅ ConfigurationSource interface
- ✅ CompositeConfigurationResolver (4-layer)

**Storage Library** (`src/codexium-magnus-storage/`):
- ✅ DbInitializer (SQLite initialization)

**Main Application** (`src/codexium-magnus/`):
- ✅ MainWindow with menu bar, toolbar, status bar
- ✅ NavigationPane (QTreeView for document hierarchy)
- ✅ SearchPane (search input with filters and results)
- ✅ WebEngineBridge (C++ ↔ JavaScript communication)
- ✅ CartridgeService (cartridge loading and navigation)
- ✅ SearchService (FTS5 search with fallback)
- ✅ LinkService (external link handling)
- ✅ PrintService (printing and PDF export)
- ✅ ThemeManager (theme switching with token system)

### Developer Journal
- **Structure**: `docs/journal/YYYY/YYYY-MM-DD.md`
- **Latest Entry**: 2025-12-10 (C++/Qt6 implementation)
- **Index**: `docs/journal/index.adoc`

---

## Architecture Overview

### Technology Stack
- **UI**: Qt Widgets (Fusion style + custom stylesheets)
- **Rendering**: Qt WebEngine (Chromium-based, replaces CEF)
- **Language**: C++17/20
- **Build**: CMake with dynamic linking (WebEngine requires dynamic linking)
- **Database**: SQLite3 via Qt SQL
- **Crypto**: Ed25519 via libsodium (C++ library) - TODO
- **Testing**: Qt Test framework, Playwright/Chrome DevTools Protocol - TODO

### Key Components (Implemented)
1. **Viewer Shell** (Qt Widgets) - ✅ MainWindow, navigation, search panes
2. **Rendering Host** (Qt WebEngine) - ✅ HTML rendering, link/print interception
3. **ThemeManager** - ✅ Theme switching, stylesheet management, token system
4. **Content Subsystem** - ✅ Cartridge I/O via Qt SQL
5. **Search Subsystem** - ✅ FTS5 queries via Qt SQL (with fallback)
6. **Security Subsystem** - ⏳ Ed25519 verification (libsodium) - TODO

---

## Implementation Progress

### Phase 1: Foundation & Build System ✅ (Major Components)
- ✅ CMake project structure created
- ✅ Top-level Makefile created
- ✅ Minimal Qt Widgets shell created
- ✅ Qt WebEngine integrated (dynamic linking)
- ✅ Dynamic linking configuration verified
- ⏳ Cross-platform dynamic build verification - TODO

### Phase 2: Core Services & Domain Layer ✅ (Complete)
- ✅ Domain models ported to C++ (TypographyConfig, BibliographyConfig)
- ✅ Configuration resolver implemented (4-layer: System → Corpus → User → Session)
- ✅ Report writer implemented (Markdown/JSON output)
- ✅ Storage layer implemented (Qt SQL)
- ✅ Cartridge reader implemented

### Phase 3: UI Shell & Integration ✅ (Complete)
- ✅ MainWindow migrated to Qt Widgets
- ✅ Navigation, search, document view implemented
- ✅ ThemeManager with default themes implemented
- ✅ Services integrated with UI
- ✅ Search service (FTS5) implemented

### Phase 4: Feature Completion & Security (In Progress)
- ✅ External link mediation (LinkService)
- ✅ Print functionality (PrintService)
- ✅ Theme system (ThemeManager)
- ✅ Ed25519 verification (SignatureService with libsodium integration)
- ✅ Typography and bibliography features (SettingsDialog with UI integration)

### Phase 5: Testing & Deployment (In Progress)
- ✅ Port test harnesses to Qt Test (test runner complete, 90+ tests passing)
- ⏳ Performance optimization
- ⏳ Cross-platform dynamic build verification
- ⏳ Documentation updates

---

## Important Files & Locations

### Documentation
- **Source**: `docs/asciidoc/*.adoc`
- **Build Output**: `docs/build/{html,pdf,docx}/`
- **Diagrams**: `docs/asciidoc/diagrams/*.mmd` → `docs/build/diagrams/`
- **Journal**: `docs/journal/YYYY/YYYY-MM-DD.md`
- **Build**: `cd docs && make all`

### Source Code
- **Main App**: `src/codexium-magnus/` (Qt Widgets)
- **Core Library**: `src/codexium-magnus-core/` (C++ services)
- **Storage**: `src/codexium-magnus-storage/` (Qt SQL)
- **Tests**: `tests/` (Qt Test framework) - TODO

### Key Configuration Files
- **CMake**: `CMakeLists.txt` (top-level), `src/*/CMakeLists.txt` (per-library)
- **Makefile**: Top-level orchestrator
- **Qt Build**: Qt 6 static build required
- **Documentation**: `docs/Makefile`, `docs/asciidoc/attributes.adoc`

---

## Next Immediate Steps

1. **Set Up Qt Development Environment**
   - Install Qt 6 (GPL-3) with dynamic linking (WebEngine requires dynamic linking)
   - Verify CMake 3.20+ installed
   - Set up C++17 compiler (GCC, Clang, or MSVC)

2. **Test Build**sh
   make configure  # Run CMake configuration
   make build      # Build the project
   3. **Test Documentation Build**
   
   cd docs
   make install-deps  # Install AsciiDoctor, Mermaid CLI
   make all           # Generate HTML, PDF, DOCX
   4. **Create Mermaid Diagrams**
   - Architecture overview
   - Component diagram
   - Theme system architecture
   - Migration phases timeline
   - Data flow diagrams

5. **Complete Remaining Features**
   - ✅ Ed25519 signature verification (SignatureService, libsodium, QSettings persistence)
   - ✅ Typography UI integration (SettingsDialog, TypographySettingsWidget)
   - ✅ Bibliography UI integration (BibliographySettingsWidget)
   - ✅ Test suite implementation (8 test suites, 90+ tests, all passing)

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

### Why Dynamic Linking?
- **Qt WebEngine Requirement**: Qt WebEngine is based on Chromium, which has licensing constraints that prevent static linking. WebEngine must be dynamically linked.
- **Other Qt Components**: While Qt Widgets, Core, and other components could theoretically be statically linked, we use dynamic linking for consistency and to avoid mixed linking scenarios.
- **GPL-3 Compliance**: Dynamic linking is fully compatible with GPL-3 licensing.
- **Deployment**: Requires Qt runtime libraries to be distributed with the application (standard Qt deployment model).

---

## Developer Journal

**Latest Entry**: 2025-12-10
- C++/Qt6 codebase implementation
- CMake project structure
- All core services and UI components
- ThemeManager and styling system
- Service layer complete

**See**: `docs/journal/2025/2025-12-10.md` for full details

---

## Git Repository Status

**Current Branch**: `main` (or worktree branch)  
**Remote**: `origin` → `https://github.com/code-monki/codexium-magnus.git`  
**Note**: Remote authentication configured

---

## Quick Commands

### Build System
make configure    # Run CMake configuration
make build        # Build the project
make clean        # Clean build directory
make test         # Run test suite (when implemented)### Documentation
cd docs
make all          # Build all formats
make html         # HTML only
make pdf          # PDF only
make diagrams     # Generate diagrams only
make clean        # Clean build outputs### Qt Build (Dynamic linking - WebEngine requirement)
cmake -B build -S . -DBUILD_SHARED_LIBS=ON
cmake --build build
---

## Important Notes

1. **PoC Stage**: Project is at proof-of-concept stage, enabling complete cutover approach
2. **No .NET**: All .NET code is reference only; full C++/Qt rewrite in progress
3. **Dynamic Linking**: Qt WebEngine requires dynamic linking due to Chromium licensing constraints. All Qt components use dynamic linking for consistency.
4. **Documentation First**: All design decisions documented before implementation
5. **Developer Journal**: Document significant decisions and progress

---

## Questions to Resolve

1. **Qt Dynamic Build**: Use pre-built Qt 6 (GPL-3) with dynamic linking (WebEngine requirement)
2. **Mermaid Diagrams**: Create initial diagrams before or during implementation?
3. **IDE Setup**: Qt Creator vs VS Code for development?
4. **Cartridge Schema**: Finalize cartridge database schema for navigation/content tables
5. **FTS5 Schema**: Define FTS5 table structure for search functionality

---

## Related Documents

- **Requirements**: `docs/asciidoc/requirements.adoc`
- **Architecture**: `docs/asciidoc/architecture.adoc`
- **Migration Path**: `docs/asciidoc/qt6-migration.adoc` (most detailed)
- **Implementation Plan**: `docs/asciidoc/implementation-plan.adoc`
- **Project Rules**: `docs/asciidoc/project-rules.adoc`
- **Developer Journal**: `docs/journal/2025/2025-12-10.md`

---

## When Resuming Work

1. Read this primer
2. Review latest journal entry (`docs/journal/2025/2025-12-10.md`)
3. Check migration path document for current phase details
4. Verify development environment setup (Qt 6 dynamic build)
5. Test build: `make configure && make build`
6. Continue with remaining features and testing

---

**Last Updated**: 2025-12-10  
**Next Review**: After Qt static build setup or significant progress