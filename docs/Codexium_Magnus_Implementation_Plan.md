# Codexium Magnus — Implementation Plan

Version: 1.0
Date: 2 Nov 2025
Scope: Avalonia host + CEFWindow single rendering engine + typography (FR-11) + bibliography (FR-12) + shared reporting + test harnesses.

⸻

1. Objectives
	1.	Implement the application in the same structure as the Detailed Design v1.1.
	2.	Keep .NET the primary runtime.
	3.	Introduce Node only for CEF/Playwright DOM testing (required because CEF is the rendering engine).
	4.	Deliver a baseline that can run without the Eleventy pipeline, but can interop with it if present.
	5.	Make reporting, not UI, the single source of truth for build/test/sanity output.

⸻

2. Assumptions
	•	CEFWindow is the standard rendering surface.
	•	App is single user, but cartridges may be on shared storage → we design for local-first, shared-OK.
	•	Config resolution order is fixed: Session → User Profile → Corpus → System.
	•	All scripts/tools/tests should emit in (or be convertible to) the shared report format defined in DD §2.6.
	•	UI spec is TBD; implementation stubs MAY expose placeholder controls.

⸻

3. Phase Overview
	1.	Phase 1 – Foundations (must-have)
	•	P1.1 Shared Report Writer (C#)
	•	P1.2 Configuration Resolver (4-layer)
	•	P1.3 Domain Models for Typography & Bibliography
	2.	Phase 2 – Host & Rendering
	•	P2.1 Avalonia Shell
	•	P2.2 CEFWindow integration
	•	P2.3 ViewModel → CEF binding
	3.	Phase 3 – Feature Work (FR-11 / FR-12)
	•	P3.1 Typography application pipeline
	•	P3.2 Bibliography generation + view
	4.	Phase 4 – Test Harnesses
	•	P4.1 .NET unit/integration tests
	•	P4.2 Avalonia in-proc UI tests
	•	P4.3 Playwright (Node) tests against CEF
	5.	Phase 5 – Packaging / Ops
	•	P5.1 Build targets (per OS)
	•	P5.2 Test/run targets
	•	P5.3 Artifacts + logs

⸻

4. Phase 1 – Foundations

P1.1 Shared Report Writer (C#)

Goal: match the DD idea of a single report format, but in .NET.
	•	Inputs: test results, pipeline/integration results, UI smoke tests.
	•	Outputs:
	•	Markdown summary (counts, severities)
	•	JSON detail (per test/run item)
	•	Why first: everything else (tests, content, even Playwright runs) can dump into this and we don’t get format sprawl.

Deliverables:
	1.	ReportSeverity enum (Fatal, Warning, Info)
	2.	ReportEntry model
	3.	ReportWriter service with: AddEntry(...), WriteMarkdown(...), WriteJson(...)
	4.	CLI sample that emits a fake run

⸻

P1.2 Configuration Resolver (4-Layer)

Goal: encode the resolution order from the DD so UI/CEF/tests don’t each re-implement it.
	•	Layers: System → Corpus → User Profile → Session (session wins)
	•	Responsibilities:
	•	Merge typography settings
	•	Merge bibliography settings
	•	Answer “effective config” for a given view

Deliverables:
	1.	IConfigurationSource interface
	2.	CompositeConfigurationResolver that orders sources
	3.	Unit tests (xUnit/NUnit) with table-driven cases:
	•	session only
	•	session + user
	•	corpus only
	•	system fallback

⸻

P1.3 Domain Models

Goal: freeze the shapes so the UI and CEF bindings stop changing.
	•	TypographyConfig
	•	baseFontFamily
	•	baseFontSize
	•	headingScale[]
	•	printOptions
	•	BibliographyEntry
	•	normalizedAuthor
	•	title
	•	publication
	•	year
	•	sourceId / link
	•	BibliographyConfig
	•	style (APA, CMS)
	•	sortBy
	•	groupBy

⸻

5. Phase 2 – Host & Rendering

P2.1 Avalonia Shell
	•	Create main window, navigation frame, and a placeholder for the CEF host.
	•	Wire in theme selection from user profile layer.
	•	Add an “Issues / Reports” pane that can read the output from the report writer (even if it’s fake data at first).

P2.2 CEFWindow Integration
	•	Embed CEF as the single rendering engine.
	•	Define a minimal message channel (C# ↔ CEF) for:
	•	load article by id/slug
	•	apply typography
	•	show bibliography

P2.3 ViewModel → CEF Binding
	•	Create a C# viewmodel that:
	•	pulls effective config from the resolver
	•	serializes it
	•	sends it to the CEF page as JSON
	•	On the CEF side, create a simple “apply config” routine (CSS vars / classes).

⸻

6. Phase 3 – Feature Work

P3.1 Typography (FR-11)
	•	Implement server/model side first:
	•	take article
	•	attach effective typography
	•	emit JSON for CEF to render
	•	In CEF:
	•	accept typography JSON
	•	set root CSS vars
	•	re-render body

Tests: .NET unit → CEF Playwright

P3.2 Bibliography (FR-12)
	•	Implement bibliography generator in C#
	•	Store result in cartridge/index model
	•	Expose to UI
	•	In CEF:
	•	render list
	•	filter
	•	link back to article

Tests: .NET unit → .NET integration → CEF Playwright

⸻

7. Phase 4 – Test Harnesses

P4.1 .NET Unit/Integration
	•	xUnit/NUnit project
	•	Tests for:
	•	report writer
	•	config resolver
	•	typography merge
	•	bibliography normalize/sort

P4.2 Avalonia In-Proc
	•	Spin up window
	•	Assert CEF placeholder present
	•	Assert theme applied

P4.3 Playwright (Node) for CEFWindow
	•	Start app (test mode)
	•	Attach Playwright to the CEF endpoint
	•	Assert:
	•	article content renders
	•	typography CSS vars are present
	•	bibliography view loads and is filterable

⸻

8. Phase 5 – Packaging / Ops
	•	Make / build script that does, in order:
	1.	build .NET
	2.	run .NET tests
	3.	run Playwright tests (optional flag if Node not present)
	4.	write reports to /reports/YYYY-MM-DD/…
	•	Artifacts:
	•	app binaries
	•	reports (Markdown + JSON)
	•	test logs

⸻

9. Implementation Order (short list)
	1.	Shared report writer (C#)
	2.	Config resolver (4-layer)
	3.	Avalonia shell (with placeholder CEF)
	4.	CEFWindow integration (real)
	5.	Typography pipeline (C# → CEF)
	6.	Bibliography pipeline (C# → CEF)
	7.	.NET test projects
	8.	Playwright tests
	9.	Packaging/build targets

⸻

10. Dev Notes / Guardrails
	1.	.NET stays primary. Node exists only because CEFWindow testing (Playwright) needs it.
	2.	One HTML renderer only (CEFWindow). No second HTML host.
	3.	Always use the 4-layer config (Session → User Profile → Corpus → System).
	4.	Shared reporting is mandatory.
	5.	Keep core rules in C#, not browser JS.
	6.	Test where it lives.
	7.	Keep path abstraction for local/shared cartridges.

⸻

11. Development Environment Setup (VS Code–first)
	Goal: neutral setup, documented for VS Code. Other IDEs can mirror.

	Prerequisites:
	•	.NET SDK (current LTS)
	•	Node.js (current LTS) for Playwright/CEF tests
	•	Git
	•	CEF runtime for target OS

	Suggested Repo Layout:
	•	/src → Avalonia app, services, models
	•	/tests → .NET unit + integration
	•	/tests/playwright → Node-based CEF tests
	•	/reports → Markdown/JSON output
	•	/tools → optional helpers

	VS Code Setup:
	1.	Install C# (Dev Kit) extension.
	2.	Install .NET test explorer.
	3.	Install Playwright Test for VS Code.
	4.	Open workspace with /src, /tests, /tests/playwright.
	5.	Launch configs: run app, .NET tests, Playwright tests.

	First-Time Steps:
	1.	dotnet restore
	2.	dotnet build
	3.	cd tests/playwright && npm install && npx playwright install
	4.	dotnet test
	5.	npx playwright test
	6.	Confirm both write to /reports

	CI Notes:
	•	CI must have .NET and Node.
	•	Run: dotnet test → npx playwright test → publish /reports.
	•	Skip Playwright only if Node unavailable.
