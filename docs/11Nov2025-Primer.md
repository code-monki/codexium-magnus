# Codexium Magnus – Project Primer (Conversation Bootstrap)

## Project Identity

**Project Name:** Codexium Magnus

**Purpose:** A cross-platform archival reader using Avalonia + CEF, loading read-only SQLite “cartridges” with embedded content and metadata.

⸻

## Project Artifacts (Documents in the System)

| Document                          | Purpose                                                                                                                                                            | Current Version                             | Status                           |
| --------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ------------------------------------------- | -------------------------------- |
| Requirements Specification        | Functional Requirements FR-1 through FR-12, including new FR-11 (Typography) and FR-12 (Bibliography).                                                             | v1.3 (pending verification after re-upload) | Needs re-upload                  |
| System Architecture (SA)          | Defines high-level components, layers, and technologies. Identifies Avalonia UI shell, CEF rendering engine, cartridge model, FTS5 indexing.                       | v1.1                                        | Needs re-upload                  |
| High-Level Design (HLD)           | Shows data flows, component responsibilities, sequencing between viewer, cartridge, and services.                                                                  | v1.0                                        | Needs re-upload                  |
| Detailed Design (DD)              | Defines how features are implemented — module-level, class-level design. DD sections should not use FR numbering. Typography + Bibliography need to be integrated. | v1.1 (in-progress)                          | Needs re-upload                  |
| UI / Print Design                 | Defines screens, layout rules, print rendering requirements (A4/Letter, anti-splitting tables/images, proxy rendering via CEF).                                    | Not yet finalized                           | Will be created after DD cleanup |
| Project Rules / Development Rules | Coding, architecture, build rules. No band-aids; full passes; deterministic output. “Top-down debugging” rule.                                                     | v1.0                                        | Needs re-upload                  |
| Test Suite / Traceability Matrix  | Links requirements → HLD → DD → tests.                                                                                                                             | Not yet created                             | Pending DD completion            |

## Functional Requirements Summary (Top-Level)    

| ID    | Title                         | Description                                               |
| ----- | ----------------------------- | --------------------------------------------------------- |
| FR-1  | Browsing                      | Browse collection via creators, issues, departments, etc. |
| FR-2  | Full-text Search              | Boolean, phrase search, fuzzy, highlight terms            |
| FR-3  | In-page Navigation            | Jump to matched locations                                 |
| FR-4  | Persistent State              | Save user settings (theme, zoom, typography, etc.)        |
| FR-5  | Multi-Platform                | Windows / macOS / Linux                                   |
| FR-6  | Cartridge Loading             | SQLite cartridge, read-only, signed                       |
| FR-7  | Access Control / Verification | Warn on unsigned cartridge                                |
| FR-8  | External Links Mediation      | Viewr intercepts external URLs                            |
| FR-9  | Table of Contents and Indices | Navigable ToC + indices                                   |
| FR-10 | Print Support                 | Print via PDF; no truncation                              |
| FR-11 | Typography Support            | User-adjustable typography; WCAG compliant                |
| FR-12 | Bibliography Support          | Generate APA / CMS, on screen + print                     |

## Architecture Summary

- UI shell: Avalonia
- Rendering: Chromium (CEF)
- Cartridges: SQLite (read-only), signed
- Search: SQLite FTS5
- Viewer: mediates links, handles print rendering
- Do not store derivable data unless performance demands it

⸻

## Current Work State (at moment of pause)

**Completed**

- FR-11 and FR-12 written and formatted in Requirements doc
- Requirements numbering validated
- Primer consolidation

**In-progress**

- Detailed Design needs Typography + Bibliography added using DD numbering, not FR numbering.
- UI / Print Design doc must be started (wireframes + print layout rules).

**Blocked due to platform issues**

- Unable to access uploaded Markdown files reliably
- ZIP generation unreliable

**Next execution step — when resumed:**

- Integrate Typography + Bibliography into Detailed Design, using your numbering scheme and structure.

## Conversation Starter

We are resuming Codexium Magnus.

Please load the following documents (I will upload them next):

1) Requirements Specification (v1.3)
2) System Architecture (v1.1)
3) High-Level Design (v1.0)
4) Detailed Design (v1.1) ← needs integration of Typography + Bibliography
5) Project Rules (v1.0)

**Objectives upon resume:**

- Integrate Typography + Bibliography into Detailed Design using DD numbering.
- Begin UI / Print Design document (wireframes + print layout rules).
- Maintain strict formatting of existing docs; do not change numbering schemes.
- Deliver artifacts incrementally in chat before attempting ZIP packaging.

You are not allowed to guess.

If a document is missing, ask me to upload it.