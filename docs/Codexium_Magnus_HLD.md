# Codexium Magnus High-Level Design (HLD) v1.0 (Draft)

## 1. Introduction

### 1.1 Purpose
This High-Level Design (HLD) document describes the major subsystems, modules, and data flows for **Codexium Magnus**, based on the Requirements Specification v1.2 (Draft) and the System Architecture Specification v1.0 (Draft). It maps requirements to concrete components and clarifies responsibilities so that detailed design and implementation can proceed without ambiguity.

### 1.2 Scope
This HLD covers:
- **Viewer Subsystem** (Avalonia + CEF)
- **Authoring Subsystem** (optional, separate mode/app)
- **Build / CLI Subsystem** (cartridge generator & signer)
- **Shared Services** (theming, storage, signing, search)
- Integration points for printing and external link handling

### 1.3 References
- Codexium Magnus Requirements Specification v1.2 (Draft)
- Codexium Magnus System Architecture Specification v1.0 (Draft)
- Avalonia UI Docs
- CEF Reference
- SQLite FTS5 Reference
- GPL-3 License

---

## 2. Design Principles

1. **Separation of concerns**: Viewer reads, Author builds, CLI packages.
2. **Content immutability**: Cartridges are read-only at runtime.
3. **Single rendering engine**: All HTML is rendered by CEF to ensure consistent behavior across platforms.
4. **Token-based theming**: Content uses CSS variables injected by the app.
5. **Untrusted-by-default**: Any cartridge not explicitly verified is sandboxed.
6. **Determinism**: Normalization rules are centralized and reused by Authoring and CLI.
7. **Cross-platform first**: All platform-specific behaviors (printing, external links) are abstracted behind services.

---

## 3. Subsystem Overview

- **S1 Viewer Subsystem** — desktop app users interact with every day.
- **S2 Authoring Subsystem** — assists non-technical authors in creating valid content.
- **S3 Build/CLI Subsystem** — turns validated content into `.ruleset` cartridges.
- **S4 Shared Services** — theming, signing, storage, logging, diagnostics.

The following sections describe each subsystem.

---

## 4. S1 — Viewer Subsystem

### 4.1 Responsibilities
- Load and display cartridge content (FR-1).
- Apply token-based theme to UI and content (FR-2).
- Execute search queries and display results (FR-3).
- Verify and display cartridge trust level (FR-4).
- Open external links in OS browser (FR-9).
- Print content via visible or off-screen path (FR-10).
- Persist user preferences (FR-7).
- Enforce accessibility options (FR-8).

### 4.2 Major Components

1. **MainWindow (Avalonia)**
   - Hosts navigation tree / search panel / document view.
   - Binds to `MainViewModel`.

2. **DocumentView (Avalonia + CEF Control)**
   - Embedded CEF browser.
   - Receives HTML fragments from Content Service.
   - Injects theme tokens.
   - Forwards link-click and print events to Viewer Services.

3. **NavigationPane**
   - Shows corpus → volume → document hierarchy.
   - Populated by Cartridge Service.

4. **SearchPane**
   - Textbox, filters (fuzzy, case, wildcard).
   - Binds to Search Service.
   - Displays results with snippet/heading.

5. **Viewer Services Layer**
   - `IThemeService`
   - `ICartridgeService`
   - `ISearchService`
   - `ILinkService`
   - `IPrintService`
   - `IPreferencesService`

All services are designed for DI (dependency injection) and testability.

### 4.3 Key Services

#### 4.3.1 ICartridgeService
**Responsibilities**
- Open `.ruleset` file.
- Validate manifest and signature.
- Expose corpora, volumes, documents.
- Serve HTML fragment (normalized).

**Key Methods**
- `OpenCartridge(path: string): CartridgeHandle`
- `GetDocument(docId: string): DocumentDto`
- `GetAssets(docId: string): IEnumerable<AssetRef>`

**Requirements Traced**
- FR-1, FR-4, FR-3 (data source), SDR

#### 4.3.2 ISearchService
**Responsibilities**
- Run SQLite FTS queries against the currently open cartridge.
- Post-process for case-sensitive and fuzzy.
- Return ranked hits with section anchors.

**Key Methods**
- `Search(query: SearchQuery): SearchResultSet`
- `GetRecentQueries(): IEnumerable<string>`

**Requirements Traced**
- FR-3, NFR-1

#### 4.3.3 IThemeService
**Responsibilities**
- Manage current theme (light/dark/custom).
- Persist theme in preferences.
- Provide CSS token map to CEF.

**Key Methods**
- `SetTheme(ThemeDefinition theme)`
- `GetTokenMap(): IDictionary<string, string>`

**Requirements Traced**
- FR-2, FR-7

#### 4.3.4 ILinkService
**Responsibilities**
- Intercept link-clicks in CEF.
- Determine whether link is internal (`cdoc://…`, `#anchor`) or external (`http://`, `https://`, `mailto:`).
- Route **external** to OS-level launcher.
- Keep document view unchanged for externals.

**Key Methods**
- `HandleLinkClick(Uri uri, LinkContext ctx)`
- `IsExternal(Uri uri): bool`

**Requirements Traced**
- FR-9, NFR-2

#### 4.3.5 IPrintService
**Responsibilities**
- Provide unified “Print” experience across OSes.
- Decide visible-print vs off-screen-print path.
- Apply print CSS (content-safe).
- Call OS print dialog.

**Key Methods**
- `PrintCurrent(DocumentContext ctx)`
- `PrintOffscreen(DocumentContext ctx)`
- `CanPrint(): bool`

**Requirements Traced**
- FR-10, NFR-1, NFR-2

### 4.4 Event / Data Flow (Viewer)

**VF-1: Open Document**
1. User selects document in NavigationPane.
2. `MainViewModel` calls `ICartridgeService.GetDocument(docId)`.
3. Document HTML + metadata returned.
4. `DocumentView` loads HTML in CEF, injects theme tokens.
5. CEF renders.

**VF-2: User clicks external link**
1. CEF raises `OnLinkClicked(uri)`.
2. Viewer asks `ILinkService.IsExternal(uri)` → `true`.
3. Viewer calls OS launcher (platform-specific).
4. CEF stays on current doc.

**VF-3: User prints long document**
1. User selects Print.
2. `IPrintService` examines document length/complexity metadata (available from cartridge).
3. If small → direct CEF print → OS dialog.
4. If large → background/off-screen render to PDF → OS print dialog with PDF.
5. Status reported in UI.

---

## 5. S2 — Authoring Subsystem

### 5.1 Purpose
To allow **non-technical** authors to create content that already conforms to the CM HTML/CSS spec without having to hand-write frontmatter or wrappers.

### 5.2 Deployment
- May be a **separate executable** (recommended) or a **separate mode** in the Viewer.
- Uses the **same rendering host** (CEF) so previews match Viewer behavior.

### 5.3 Major Components

1. **AuthorShell (Avalonia)**
   - Workspace UI with “Metadata” panel and “Content” panel.

2. **MetadataPanel**
   - Form for: Title, Corpus, Volume, Category, Tags, Per-doc CSS, Interactive?
   - Writes YAML or JSON frontmatter structure.

3. **ContentEditor (CEF-based WYSIWYG)**
   - Loads a JS editor (TinyMCE-like) inside CEF.
   - Outputs HTML fragment.
   - Forbids `<html>`, `<head>`, `<body>` tags.
   - Flags disallowed inline styles.

4. **PreviewPane (CEF)**
   - Renders the *normalized* version using the same CSS/token set as the Viewer.
   - Runs the same validation logic.

5. **Validator / Linter**
   - Checks heading ladder.
   - Checks for missing IDs.
   - Checks for missing mandatory metadata.
   - Emits warnings list.

6. **Exporter**
   - Exports to source folder (MD/HTML+frontmatter).
   - Optionally calls CLI to build a cartridge (publisher-only).

### 5.4 Data Flow (Authoring)

**AF-1: Create New Document**
1. User clicks “New”.
2. AuthorShell pre-fills `id`, `title` empty, `corpus` default.
3. User edits metadata → YAML/JSON in memory updated.
4. User writes HTML in editor.
5. Validator runs → warnings shown.
6. PreviewPane shows normalized output.

**AF-2: Export for Build**
1. User chooses “Export”.
2. Author app writes:
   - `doc-id.md` (or `.html`) with frontmatter.
   - Assets (images) copied to correct relative folders.
3. Optionally invokes `codexium-magnus-cli build ...`.

### 5.5 Security / Liability Notes
- Authoring app **does not** sign cartridges.
- Only the CLI (or a separate signing helper) performs signing.
- This preserves the separation: shipping the authoring app does **not** mean shipping a publishing tool.

---

## 6. S3 — Build / CLI Subsystem

### 6.1 Purpose
To transform a folder of authoring sources into a single, signed, read-only `.ruleset` cartridge.

### 6.2 Major Commands
- `cm build --src ./content --out ./MyCorpus.ruleset`
- `cm sign --cartridge ./MyCorpus.ruleset --key ./publisher.key`
- `cm validate --src ./content`
- `cm inspect --cartridge ./MyCorpus.ruleset`

### 6.3 Build Pipeline

1. **Ingest**
   - Read all `*.md` / `*.html` / `manifest.yml`
   - Parse frontmatter
   - Assign stable IDs
2. **Normalize**
   - Enforce root `<div class="cdoc" ...>`
   - Strip disallowed tags
   - Generate heading IDs
   - Extract sections
3. **Index**
   - Build SQLite tables: `Documents`, `Sections`, FTS5
   - Store cartridge-level metadata
4. **Package**
   - Write SQLite file
   - Embed assets (if “single-file” mode) OR reference external asset folder
5. **Sign**
   - Compute digest
   - Sign with Ed25519 private key
   - Embed signature block in manifest

### 6.4 Error Handling
- Build must fail-fast on:
  - Missing mandatory metadata
  - Duplicate document IDs
  - Unresolvable asset references
  - Failed signing
- Build must emit a structured report (JSON) for CI use.

---

## 7. S4 — Shared Services

### 7.1 Theming Service
- Loads theme definitions (JSON) from app resources.
- Merges user preferences.
- Exposes final token map to both Viewer and Authoring.

### 7.2 Storage Service
- Provides local, per-user settings store.
- Separates **runtime** (viewer) DB from **cartridge** DB.

### 7.3 Signing Service
- Wraps Ed25519 operations.
- Used by CLI for signing, by Viewer for verification.
- Supports self-managed keys.

---

## 8. Traceability Matrix (High-Level)

| Requirement | Subsystem / Component |
|-------------|------------------------|
| FR-1 Content Display | S1 Viewer → DocumentView, ICartridgeService |
| FR-2 Theming | S1 Viewer → IThemeService; S4 ThemingService |
| FR-3 Search | S1 Viewer → ISearchService; S3 Build (indexing) |
| FR-4 Cartridge Verification | S1 Viewer → Signing Subsystem |
| FR-5 Authoring | S2 Authoring Subsystem |
| FR-6 Build CLI | S3 Build / CLI Subsystem |
| FR-7 Preferences | S1 Viewer → IPreferencesService |
| FR-8 Accessibility | S1 Viewer (Avalonia), Content CSS |
| FR-9 External Link Handling | S1 Viewer → ILinkService, CEF bridge |
| FR-10 Printing Support | S1 Viewer → IPrintService, CEF print path |

---

## 9. Change Log (Newest First)

| Version | Date | Author | Summary |
|---------|------|--------|---------|
| 1.0-draft | 2025-11-02 | System | Initial High-Level Design for Viewer, Authoring, CLI, including external link and printing flows. |
