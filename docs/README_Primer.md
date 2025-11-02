# Codexium Magnus Primer

**Purpose**

Codexium Magnus is an open, cross-platform digital library framework designed to store, render, and search structured collections of textual or multimedia works — including, but not limited to, tabletop RPG rules, technical manuals, literary archives, and educational material.

This primer is the handoff bundle for starting a new conversation without losing context.

**Contents**

- `Codexium_Magnus_Requirements.md`
- `Codexium_Magnus_Architecture.md`
- `Codexium_Magnus_HLD.md`
- `Codexium_Magnus_Detailed_Design.md`
- `project_rules.md`
- `next_steps.md`

**Resumption Script (paste in new chat)**

```text
Resume from primer.

Use Codexium Magnus as the project name.
We are at Detailed Design v1.1 with typography (FR-11) and bibliography (FR-12) drafted.
We added three tables: cm_creators, cm_doc_creators, cm_doc_relations.
Viewer uses Avalonia + CEF and mediates external links and printing.
Cartridges are SQLite, read-only, signed, with FTS5.
Do not store derivable data unless there is a clear performance reason.

Version Map
	•	Requirements: v1.2 (needs v1.3 to add typography + bibliography)
	•	System Architecture: v1.1 (needs tiny update for typography tokens + IBibliographyService)
	•	HLD: v1.0 (needs flows for “Generate bibliography” and “Apply typography”)
	•	Detailed Design: v1.1 (current truth)
	•	Project Rules: 1.0
