# Codexium Magnus Detailed Design v1.0 (Draft)

## 1. Introduction

### 1.1 Purpose
This document provides the **detailed design** for the **Codexium Magnus** project. It expands the Requirements Specification (v1.2), System Architecture Specification (v1.1), and High-Level Design (v1.0) into concrete, implementable modules, schemas, and workflows.

### 1.2 Scope
This detailed design covers:
- Runtime data model and cartridge schema (read-only)
- Build/CLI pipeline (authoring sources → normalized cartridge) with **mandatory HTML entity decoding**
- Viewer implementation details (Avalonia + CEF, link and print wiring)
- Authoring tool implementation details
- Security and signing mechanics

### 1.3 References
- Codexium Magnus Requirements Specification v1.2 (Draft)
- Codexium Magnus System Architecture Specification v1.1 (Draft)
- Codexium Magnus High-Level Design v1.0 (Draft)
- Avalonia, CEF, SQLite FTS5 documentation
- GPL-3 License

---

## 2. Design Goals

1. **Determinism** — same inputs produce the same cartridge.
2. **Single normalization source** — one code path for entity decoding and HTML cleaning.
3. **Runtime simplicity** — Viewer never repairs content; it trusts the cartridge.
4. **Security by mediation** — Viewer controls all link and print operations.
5. **Cross-platform consistency** — Avalonia + CEF ensure identical rendering.
6. **Traceability** — every section maps back to requirements.

---

## 3. Terminology

| Term | Description |
|------|--------------|
| Cartridge | SQLite-based read-only bundle of normalized content and metadata. |
| Document | Individual article/chapter/adventure inside a cartridge. |
| Section | Heading-based subdivision of a document. |
| Viewer | Avalonia desktop app used to read cartridges. |
| Authoring Tool | Editor/validator for authors; does not sign cartridges. |
| CLI | Build/signing utility for publishers. |
| Decoder | Shared module that decodes HTML entities and normalizes markup. |

---

## 4. Data Model (Runtime)

### 4.1 Cartridge Schema (SQLite, Read-Only)

```sql
CREATE TABLE cm_documents (
    doc_id          TEXT PRIMARY KEY,
    slug            TEXT NOT NULL,
    title           TEXT NOT NULL,
    corpus_id       TEXT NOT NULL,
    volume_id       TEXT NOT NULL,
    category        TEXT,
    html            TEXT NOT NULL,
    has_interactive INTEGER NOT NULL DEFAULT 0,
    updated_utc     TEXT NOT NULL
);

CREATE TABLE cm_sections (
    section_id   TEXT PRIMARY KEY,
    doc_id       TEXT NOT NULL,
    anchor       TEXT NOT NULL,
    title        TEXT NOT NULL,
    level        INTEGER NOT NULL,
    body_text    TEXT,
    FOREIGN KEY (doc_id) REFERENCES cm_documents(doc_id)
);
CREATE INDEX idx_cm_sections_doc_id ON cm_sections(doc_id);

CREATE TABLE cm_assets (
    asset_id   TEXT PRIMARY KEY,
    path       TEXT NOT NULL,
    mime       TEXT NOT NULL,
    blob       BLOB
);
CREATE INDEX idx_cm_assets_path ON cm_assets(path);

CREATE TABLE cm_meta (
    key   TEXT PRIMARY KEY,
    value TEXT NOT NULL
);
```

### 4.2 Full-Text Search (FTS5)

```sql
CREATE VIRTUAL TABLE cm_search USING fts5 (
    doc_id UNINDEXED,
    section_id UNINDEXED,
    title,
    body,
    tokenize = "unicode61"
);
```

### 4.3 Local App Data (Writable)

```sql
CREATE TABLE cm_prefs (
    key   TEXT PRIMARY KEY,
    value TEXT NOT NULL
);

CREATE TABLE cm_trusted_keys (
    publisher_id   TEXT PRIMARY KEY,
    public_key     TEXT NOT NULL,
    added_utc      TEXT NOT NULL
);

CREATE TABLE cm_recent_docs (
    doc_id      TEXT PRIMARY KEY,
    opened_utc  TEXT NOT NULL
);

CREATE TABLE cm_print_history (
    doc_id      TEXT NOT NULL,
    printed_utc TEXT NOT NULL,
    pages       INTEGER,
    PRIMARY KEY (doc_id, printed_utc)
);
```

---

## 5. Cartridge Build / CLI Pipeline

### 5.1 Overview
The CLI converts source content into a normalized, fully searchable, and signed cartridge. The pipeline is **linear** and **fail-fast**.

**Stages:**
1. Discover sources
2. Parse frontmatter
3. Convert Markdown → HTML
4. Normalize HTML
5. **Decode HTML entities**
6. Extract sections
7. Build FTS rows
8. Assemble SQLite
9. Write manifest/meta
10. Sign

### 5.2 HTML Entity Decoding (Mandatory Step)
Entity decoding is performed **after normalization** but **before indexing** or section extraction.

**Implementation Notes:**
- Use a canonical HTML5 entity table.
- Entities are decoded in-place in text nodes.
- If unknown entities are encountered, fail the build with a diagnostic message.

```csharp
string normalized = HtmlNormalizer.Normalize(rawHtml);
string decoded = HtmlEntityDecoder.Decode(normalized);
var sections = SectionExtractor.Extract(decoded);
string plain = HtmlToText.Extract(decoded);
```

---

## 6. Viewer Detailed Design

### 6.1 Architecture
- **UI Framework**: Avalonia (.NET 8)
- **Renderer**: Chromium Embedded Framework (CEF)
- **Layout**: Split panes — navigation, content (CEF), and search.

### 6.2 Event Flow
- CEF raises events: `LinkClicked`, `PrintRequested`.
- Viewer routes through `ILinkService` and `IPrintService`.
- External URIs open via system browser.
- Print requests open OS print dialogs (visible or off-screen path).

### 6.3 Theming
- Viewer injects theme CSS tokens before document load.
- Theme persistence stored in `cm_prefs`.

### 6.4 Printing
- `IPrintService` decides path:
  - Small: direct CEF print.
  - Large: off-screen CEF render → PDF → OS dialog.
- User sees consistent results across OSes.

---

## 7. Authoring Tool Detailed Design

### 7.1 Components
- **AuthorShell** — window host.
- **MetadataForm** — validates frontmatter fields.
- **ContentEditor (CEF)** — WYSIWYG for HTML editing.
- **PreviewPane (CEF)** — real render view.
- **Validator** — uses same decoder and normalizer as CLI.

### 7.2 Export
1. Validate frontmatter.
2. Normalize and decode HTML.
3. Export `.html` or `.md` with YAML frontmatter.
4. Optionally call CLI `build` command.

---

## 8. Security & Signing

### 8.1 Trust Levels

```csharp
public enum TrustLevel
{ 
    Official, 
    Verified, 
    Unverified 
}
```

### 8.2 Enforcement
- Viewer badges content trust visually.
- `ILinkService` warns for unverified external links.
- `IPrintService` may watermark or alert for unverified cartridges.
- CLI handles signing with Ed25519; private keys never bundled.

---

## 9. Change Log (Newest First)

| Version | Date | Author | Summary |
|----------|------|--------|----------|
| 1.0-draft | 2025-11-02 | System | Consolidated full detailed design with data model, CLI pipeline (entity decoding), viewer, authoring, and security design. |
