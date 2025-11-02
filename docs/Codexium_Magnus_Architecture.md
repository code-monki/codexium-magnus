# Codexium Magnus System Architecture Specification v1.1 (Draft)

## 1. Introduction

### 1.1 Purpose
This document defines the **system-level architecture** for **Codexium Magnus (CM)**, a cross-platform, GPL-3 desktop application for viewing, searching, and managing digitally signed content cartridges. It supersedes v1.0 by adding explicit architectural support for:
- OS-level external link handling (from CEF to system browser)
- Dual-path printing (visible vs off-screen/print-to-PDF)

### 1.2 Scope
This specification applies to:
- The **Viewer** (Avalonia + CEF) desktop application
- The optional **Authoring Tool** (Avalonia + CEF in author mode)
- The **Build/CLI toolchain** for cartridge construction and signing
- The **Cartridge format** (SQLite-based, read-only, signed)
- The **Key management** and **signature verification** pipeline

Online services and publishing registries remain out of scope for v1.x.

### 1.3 Audience
- Architects and senior developers designing CM subsystems
- Contributors implementing Viewer/Authoring/CLI features
- Publishers/toolsmiths integrating CM cartridge builders
- Auditors reviewing trust and liability separation

### 1.4 References
- Codexium Magnus Requirements Specification v1.2 (Draft)
- Chromium Embedded Framework (CEF) Reference
- Avalonia UI Documentation
- SQLite 3 FTS5 Documentation
- GNU GPL-3 License
- Ed25519 Signature Scheme

---

## 2. Architectural Overview

### 2.1 Architectural Style
Codexium Magnus uses a **layered, component-based architecture**:

1. **Presentation Layer** — Avalonia UI, windowing, panels.
2. **Integration Layer** — CEF host (HTML renderer), link/print bridges.
3. **Application Layer** — view models, navigation, theming, search orchestration.
4. **Domain Layer** — cartridge reader, schema adapters, signing/trust.
5. **Infrastructure Layer** — SQLite, file I/O, key store, logging.

The guiding principle is **stable core, pluggable content**: viewer code changes infrequently; cartridges supply dynamic data.

### 2.2 High-Level Component Model

- **Viewer Shell (Avalonia)**
  - windowing
  - navigation / search panes
  - preference persistence

- **Rendering Host (CEF)**
  - HTML fragment rendering
  - token-based theming injection
  - **link interception → ILinkService**
  - **print invocation → IPrintService**

- **Content Subsystem**
  - cartridge opener (SQLite)
  - manifest reader (JSON)
  - document/section provider
  - asset resolver

- **Search Subsystem**
  - SQLite FTS5 adapter
  - Boolean / wildcard / fuzzy query builder
  - result shaping

- **Security / Signing Subsystem**
  - Ed25519 verify
  - trust classification
  - user-managed key store

- **Authoring Subsystem (optional)**
  - CEF WYSIWYG
  - metadata/frontmatter editor
  - spec validator
  - previewer using same CEF pipeline

- **Build / CLI Subsystem**
  - source ingestion
  - normalization
  - SQLite cartridge writer
  - manifest/signature generator

---

## 3. Deployment View

### 3.1 Target Platforms
- **Windows 10+**: .NET 8, Avalonia, CEF (Win)
- **macOS 13+**: .NET 8, Avalonia, CEF (Mac)
- **Linux (GTK-compatible)**: .NET 8, Avalonia, CEF (Linux)

### 3.2 Deployment Units
- `codexium-magnus-viewer`
- `codexium-magnus-author` (optional or build flag)
- `codexium-magnus-cli`
- `*.ruleset` cartridges (SQLite, read-only)

### 3.3 External Dependencies
- OS default browser / URL handler (for external links)
- OS print dialog / print subsystem (for printing)

---

## 4. Component Descriptions

### 4.1 Viewer Shell
**Responsibilities**
- Launch and host Avalonia UI
- Manage panes and document view lifetime
- Maintain user preferences (theme, font size, zoom)
- Expose commands: OpenCartridge, Search, Print, OpenExternal

**Interfaces**
- Consumes: `ICartridgeService`, `ISearchService`, `IThemeService`, `ILinkService`, `IPrintService`
- Publishes: UI events → ViewModels

**Notes**
- Must remain ignorant of cartridge internal schema beyond service contracts.

---

### 4.2 Rendering Host (CEF)
**Responsibilities**
- Render normalized HTML fragments
- Inject global CSS token definitions
- Provide in-page navigation
- **Intercept all link activations** and forward to `ILinkService`
- **Handle print requests** (user-triggered or content-triggered) by forwarding to `IPrintService`

**Interfaces**
- To Viewer Shell: events `OnLinkActivated(uri)`, `OnPrintRequested(context)`
- From Viewer Shell: commands `LoadHtml(fragment, baseUri)`, `ApplyTokens(map)`

**Security**
- Default: scripts disabled
- If `data-interactive="true"`: enable sandboxed script context, still with link/print interception

---

### 4.3 Content Subsystem
**Responsibilities**
- Open `.ruleset` SQLite files
- Verify structure and minimal schema
- Read manifest (corpus, volumes, titles, signature metadata)
- Provide documents and sections to application
- Provide asset streams to CEF (via custom URI scheme)

**Data Flow**
1. Viewer calls `OpenCartridge(path)`.
2. Content Subsystem validates and returns handle.
3. Viewer asks for `GetDocument(docId)` → HTML fragment + metadata.
4. CEF loads fragment.
5. Asset requests are routed back via content URI handler.

---

### 4.4 Search Subsystem
**Responsibilities**
- Build FTS queries against cartridge DB
- Support Boolean, wildcard, fuzzy, case-sensitive
- Return ranked results with document + section info

**Integration**
- Search pane in Viewer → Search Service → Content DB
- Results used to drive document navigation

---

### 4.5 Security / Signing Subsystem
**Responsibilities**
- Ed25519 verification over manifest + DB hash
- User-importable public keys
- Trust classification:
  - Official
  - Verified
  - Unverified/Homebrew

**Integration**
- Viewer displays trust badge
- Rendering Host checks trust level before enabling interactive features
- **Link/print services MAY consult trust level** to warn/gate actions

---

### 4.6 Authoring Subsystem
**Responsibilities**
- Provide non-technical authors with a spec-driven editor
- Run the same normalization rules as CLI
- Preview via same CEF config
- NOT sign cartridges

**Integration**
- Shares rendering host, theming service, and validator library with Viewer
- Outputs source files and (optionally) invokes CLI

---

### 4.7 Build / CLI Subsystem
**Responsibilities**
- Turn source content into cartridges
- Enforce normalization rules (root wrapper, heading IDs, no `<html>`)
- Build SQLite schema
- Generate manifest JSON
- Sign with Ed25519

**Integration**
- Consumed by publishers
- Developer-friendly for CI

---

## 5. Cross-Cutting Concerns

### 5.1 External Link Handling (New in v1.1)
**Problem:** Inline content may contain external URLs which must not hijack the embedded viewer.

**Architecture Decision**
- All links clicked in CEF are **captured**.
- A dedicated **Link Service** decides:
  - **Internal**: `cdoc://...`, `#/anchor` → route back into Viewer/CEF
  - **External**: `http://`, `https://`, `mailto:`, OS-registered schemes → dispatch to **OS-level launcher**

**Flow**
1. CEF → `OnLinkActivated(URI)`
2. Viewer → `ILinkService.Handle(URI, trustContext)`
3. If external:
   - Viewer → platform launcher (`Process.Start`, `xdg-open`, `open`)
   - Viewer keeps current document loaded
4. If cartridge trust level = Unverified/Homebrew:
   - Viewer MAY show “This content wants to open: {URI}”

**Requirements Supported**
- FR-9, NFR-2

---

### 5.2 Printing (New in v1.1)
**Problem:** Large/complex HTML cannot always be printed reliably directly from the embedded CEF window.

**Architecture Decision**
- Introduce a dedicated **Print Service**.
- Support **two** print paths:
  1. **Visible-print path** — for small/medium documents
     - CEF prints current view
     - Viewer invokes OS print dialog
  2. **Off-screen/print-surface path** — for long/complex documents
     - Create hidden CEF/print surface
     - Ask CEF to render/`PrintToPDF`
     - Hand resulting artifact to OS print dialog

**Flow**
1. User selects Print OR content triggers print
2. Viewer → `IPrintService.Print(docContext)`
3. `IPrintService` inspects document size/complexity and platform
4. If small → visible-print (direct CEF → OS)
5. If large → off-screen/print-surface (CEF → PDF → OS)
6. OS print dialog displayed

**Requirements Supported**
- FR-10, NFR-1, NFR-2

---

## 6. Data View

### 6.1 Cartridge Logical Schema
- `Documents(id, slug, title, html, corpus_id, volume_id, category, updated_utc)`
- `Sections(id, doc_id, anchor, title, level, body_text)`
- `DocIndex` (FTS5, over documents/sections)
- `Assets(id, path, mime, blob)`
- `Meta(key, value)`
- `Signature(algorithm, value, public_key_hint)`

### 6.2 Application Data
- `Preferences(key, value)`
- `TrustedKeys(publisher_id, public_key, added_utc)`
- `RecentDocuments(doc_id, opened_utc)`
- Optional: `PrintHistory(doc_id, printed_utc, pages)`

---

## 7. Security Architecture

### 7.1 Threats
- T1: Malicious cartridge tries to navigate user to hostile site.
- T2: Malicious cartridge triggers large-print to crash viewer.
- T3: Modified cartridge attempts to masquerade as official.

### 7.2 Controls
- Link interception and OS mediation
- Print mediation and off-screen rendering
- Ed25519 verification
- Trust badge + user prompt on unverified external actions
- No arbitrary network access from content

---

## 8. Architectural Rationale

1. **Separate link/print services** avoid coupling CEF quirks directly to UI logic.
2. **OS mediation** for links reduces liability and keeps the viewer on-task.
3. **Off-screen print** addresses known CEF/HTML print inconsistencies.
4. **Self-managed keys** keeps you (the project) out of the SA business.

---

## 9. Change Log (Newest First)

| Version | Date | Author | Summary |
|---------|------|--------|---------|
| 1.1-draft | 2025-11-02 | System | Added explicit external-link handling (ILinkService) and dual-path printing (IPrintService). |
| 1.0-draft | 2025-11-02 | System | Initial architecture baseline for Codexium Magnus. |
