# Codexium Magnus — Detailed Design

|              |                                                                                                                                    |
| ------------ | ---------------------------------------------------------------------------------------------------------------------------------- |
| **Version:** | 1.1                                                                                                                                |
| **Date:**    | 2 Nov 2025                                                                                                                         |
| **Scope:**   | Avalonia host + CEFWindow single rendering engine + typography (FR-11) + bibliography (FR-12) + shared reporting + test harnesses. |

## 1 Overview

The detailed design defines the architecture, data models, and component interactions for Codexium Magnus v1.2. It captures the implementation model that follows the system architecture and the high-level design. This version adds support for multi-cartridge libraries via a manifest subsystem.

## 2 System Components

1. **Avalonia Shell** – Host application (C#).  
2. **CEFWindow** – Embedded Chromium rendering engine (single rendering engine).  
3. **Core Library** – Configuration resolver, report writer, domain models (typography + bibliography).  
4. **Storage Layer** – SQLite3 database with local-first design, idempotent schema creation.  
5. **Test Harnesses** – xUnit (Core + Storage) and Playwright (CEF DOM) tests.  
6. **Library Manager** – Scans, indexes, and mounts cartridges from the user’s library directory.  
7. **Manifest Subsystem** – Reads per-cartridge `manifest.json`, persists library metadata, and exposes series/volume groupings to the UI.

## 3 Data Models

### 3.1 TypographyConfig

```csharp
public sealed class TypographyConfig
{
    public string? BaseFontFamily { get; set; }
    public double? BaseFontSizePt { get; set; }
    public IList<double>? HeadingScale { get; set; }
    public TypographyPrintOptions? PrintOptions { get; set; }
}

public sealed class TypographyPrintOptions
{
    public double? PageMarginMm { get; set; }
    public bool? BlackOnWhite { get; set; }
}
```

### 3.2 BibliographyConfig

```csharp
public sealed class BibliographyConfig
{
    public string? Style { get; set; }      // e.g. "APA", "CMS"
    public string? SortBy { get; set; }     // e.g. "author", "year"
    public string? GroupBy { get; set; }    // e.g. "year"
}
```

### 3.3 BibliographyEntry

```csharp
public sealed class BibliographyEntry
{
    public string Id { get; set; } = default!;
    public string? Author { get; set; }
    public string Title { get; set; } = default!;
    public string? Publication { get; set; }
    public string? Year { get; set; }
    public string? SourceId { get; set; }   // link back to cartridge or external source
}
```

###  ReportEntry

```csharp
public enum ReportSeverity
{
    Info = 0,
    Warning = 1,
    Fatal = 2
}

public sealed class ReportEntry
{
    public DateTime TimestampUtc { get; init; } = DateTime.UtcNow;
    public ReportSeverity Severity { get; init; }
    public string Title { get; init; } = string.Empty;
    public string? Details { get; init; }
    public string? Source { get; init; }
}
```

## 4 Configuration Resolver (4-Layer)

**Resolution order: System → Corpus → User Profile → Session**

but the merge is performed from lowest → highest so that the highest layer (Session) wins.

- System: built-in defaults (app, platform, theme defaults)
- Corpus: cartridge/corpus-provided settings
- User Profile: persisted user prefs (font size, theme)
- Session: temporary, per-run overrides (e.g. print preview)

**Key responsibilities:**

	1.	Merge typography settings from all participating layers.
	2.	Merge bibliography settings from all participating layers.
	3.	Expose an effective config for any view that needs it (Avalonia, CEF, tests).
	4.	Keep behavior deterministic and testable.

## 5 Report Writer

A single, shared report writer prevents format sprawl.

Capabilities

- Accumulate ReportEntry items at runtime.
- Write Markdown summary to /reports/YYYY-MM-DD/*.md.
- Write JSON detail to /reports/YYYY-MM-DD/*.json.

All tests and tools emit via this writer for consistent reporting.

## 6 Storage Layer (SQLite3)

The storage layer provides a local, idempotent database for mounted cartridges, bibliography entries, and user settings.

Schema (v1):

```sql
CREATE TABLE IF NOT EXISTS Cartridges (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    CartridgeId TEXT NOT NULL,
    Title TEXT NOT NULL,
    SeriesName TEXT,
    VolumeNumber INTEGER,
    Path TEXT NOT NULL,
    Mounted INTEGER NOT NULL DEFAULT 0,
    LastScannedUtc TEXT,
    Version TEXT
);

CREATE TABLE IF NOT EXISTS BibliographyEntries (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    CanonicalKey TEXT,
    Title TEXT,
    Author TEXT,
    Year TEXT,
    SourceId TEXT
);

CREATE TABLE IF NOT EXISTS UserSettings (
    Key TEXT PRIMARY KEY,
    Value TEXT NOT NULL,
    UpdatedUtc TEXT NOT NULL
);
```

Idempotent creation allows safe re-init each startup. Default path: ~/.codexium-magnus/app.db.

## 7 Host ↔ CEF Communication (JSON Contract)

### 7.1 Envelope

```json
{
  "type": "string",
  "requestId": "string?",
  "timestamp": "2025-11-02T19:30:00Z",
  "payload": { }
}
```

### 7.2 Config Update

```json
{
  "type": "config:update",
  "payload": {
    "typography": { "baseFontFamily": "Segoe UI", "baseFontSizePt": 12 },
    "bibliography": { "style": "APA", "sortBy": "author" }
  }
}
```

### 7.3 TOC (Data)

```json
{
  "type": "toc:data",
  "payload": {
    "entries": [
      { "id": "ct-v1-ch1", "title": "Introduction", "volume": "1", "cartridgeId": "ct-vol-1" },
      { "id": "ct-v2-ch1", "title": "Starships", "volume": "2", "cartridgeId": "ct-vol-2" }
    ]
  }
}
```

### 7.4 Entry Load / Data

```json
{ "type": "entry:load", "payload": { "id": "ct-v2-ch1", "cartridgeId": "ct-vol-2" } }

{ "type": "entry:data", "payload": {
  "id": "ct-v2-ch1",
  "title": "Starships",
  "publisher": "GDW",
  "publicationDate": "1980",
  "volume": "2",
  "html": "<h1>Starships</h1><p>…</p>"
} }
```

### 7.5 Bibliography Data

```json
{
  "type": "bibliography:data",
  "payload": {
    "entries": [
      { "id": "bib-001", "author": "Miller, Marc", "title": "Traveller", "year": "1977", "sourceId": "ct-vol-1" }
    ]
  }
}
```

## 8 Test Harnesses

- Core tests: resolver merge logic, bibliography override.
- Storage tests: idempotent DB creation.
- Playwright: CEF rendering + config application.

All emit via the shared report format.

## 9 Library & Manifest Management

### 9.1 Goals

Enable multi-cartridge and multi-volume support without breaking existing monolithic cartridges.

### 9.2 Manifest File

Each cartridge includes a manifest.json at its root:

```json
{
  "cartridgeId": "ct-vol-1",
  "title": "Classic Traveller – Volume 1",
  "seriesName": "Classic Traveller Canon",
  "volumeNumber": 1,
  "entryCount": 24,
  "bibliographyCount": 80,
  "version": "1.0.0",
  "description": "Volume 1 of the canonical Traveller materials."
}
```

### 9.3 LibraryManager Responsibilities

1.	Scan library directory for cartridges.
2.	Read each manifest and update Cartridges table.
3.	Group by seriesName + volumeNumber.
4.	Expose combined TOC and bibliography to UI.
5.	Handle version checks for updates.

### 9.4 UI Adaptations

- Library pane shows series → volumes → entries.
- Single or multi-cartridge corpora supported.
- Optional “View All Volumes” merged mode.

### 9.5 Integration Notes

- cartridgeId and volume fields are optional.
- Existing contracts and tests unaffected.
- Manifest metadata stored in SQLite for fast lookup and cross-cartridge search.

⸻

## 10 Change Log (Newest First)

| Version   | Date       | Author | Summary                                                                                                          |
| --------- | ---------- | ------ | ---------------------------------------------------------------------------------------------------------------- |
| 1.1       | 2025-11-02 | System | Added §9 Library & Manifest Management; clarified multi-cartridge design and optional cartridgeId/volume fields. |
| 1.0-draft | 2025-11-02 | System | Initial detailed design covering core architecture, models, storage, and tests.                                  |



---

## FR‑11 Typography (Detailed Design – v1.2)

### Goal
Ensure all rendered content is readable, consistent, and theme-aware in the desktop viewer and printable output.

### Scope
- Typography preset applied to article HTML at render time.
- User may adjust base font size; presets remain consistent.

### Preset (viewer: sans serif)
- Font family: `Inter, system-ui, Helvetica, Arial, sans-serif`
- Base font size: **13pt**
- Line height: **1.45**
- Heading scale:
  - H1: 2.0×
  - H2: 1.6×
  - H3: 1.3×
- WCAG AA contrast must be maintained.

### Preset (printing: serif)
- Font family: `Junicode, Georgia, serif`
- Print uses same ratios but serif type.
- Page margins standardized for readability.

### Behavior
1. User selects typography settings.
2. Viewer applies typography tokens to content container.
3. Typography never alters article source data.

---

## FR‑12 Bibliography (Detailed Design – v1.2)

### Goal
Provide generated bibliography from metadata for any article or project-wide listing.

### Ordering rules

APA:
1. By creator last name (ascending)
2. Year (descending)
3. Title (ignore “A”, “An”, “The”)

CMS:
1. Creator last name (ascending)
2. Title (alphabetical, ignore articles)
3. Year (ascending)

### Formatting rules

APA: `Lastname, Firstname. (Year). Title. Publication.`
CMS: `Lastname, Firstname. Title. Publication, Year.`

### Behavior
- Viewer requests bibliography data from cartridge.
- Bibliography formatting service returns formatted text block.
- Never stored, always generated.

---
---

## Change Log

| Version | Date       | Changes |
|---------|------------|---------|
| v1.2 | 2025-11-09 | Added Typography preset (viewer sans, print serif), added APA/CMS formatting + deterministic ordering rules. |
| v1.1 | (previous) | Initial Detailed Design draft. |

