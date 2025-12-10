# Codexium Magnus – Project Rules
Version: 1.0  
Status: Approved

## 1. Purpose
These rules ensure consistency, correctness, and predictability across all development and documentation work for Codexium Magnus.

---

## 2. Principles

2.1 **Determinism Over Convenience**  
All processes (build, content generation, document formatting) must produce deterministic output.

2.2 **No Band-Aid Fixes**  
Temporary workarounds are not permitted. Fix root causes.

2.3 **Do Not Store Derivable Data**  
Only store data that cannot be computed efficiently at runtime, unless storing it significantly improves performance.

2.4 **Single Source of Truth**  
Each artifact (requirements, design, code) must reflect a consistent and traceable state.

---

## 3. Debugging Protocol (strict order)

Debugging must follow this sequence:

1. **Configs first**  
   Build configs, project configs, paths, module resolution.

2. **Scripts second**  
   Build scripts, content processors, generators.

3. **Source last**  
   Components, templates, views, logic.

Skipping steps is not permitted.

---

## 4. Documentation Rules

4.1 **Requirements documents** use `FR-x` numbering.  
4.2 **Detailed Design documents** must not reuse `FR-x`. They use `DD-x.y` or their section numbering.  
4.3 Each doc must preserve existing numbering and formatting — no renumbering unless the whole document is revised.  
4.4 ZIPs or large updates must be preceded by partial visible output in chat.

---

## 5. Coding Rules

5.1 **Comprehensive passes over files.**  
Avoid “touching the file every time a new idea appears.”

5.2 **Logging rules**  
All debug logging must be gated (e.g., `isDev` or equivalent config flag).

5.3 **File Format**  
- CommonJS modules (`require`, not `import`) unless otherwise stated.
- No silent changes in module format.

5.4 **Performance expectations**  
Avoid O(n) operations when O(1) is feasible.

---

## 6. Build & Script Rules

6.1 Build scripts must:
- provide clear input/output paths,
- validate assumptions,
- fail fast with context.

6.2 Sanity checks (content, links, chunks) must:
- run deterministically,
- produce summary + detailed report,
- never mutate data unless explicitly part of a repair script.

6.3 Makefiles must use **tabs**, not spaces.

---

## 7. Test & Traceability Rules

7.1 Every Functional Requirement (FR-x) must be traceable to:
- Architecture section
- HLD section
- Detailed Design section
- One or more test cases

7.2 Test traces follow this format:

    FR-x → HLD-x → DD-x → TEST-x

---

## 8. UI/UX Rules

8.1 The viewer must not *invent* new UI conventions without explicit approval.  
8.2 Defaults must favor readability and accessibility (WCAG).  
8.3 Typography and theme settings must persist across cartridges.

---

## 9. Packaging / Cartridge Rules

9.1 Cartridges are **read-only SQLite**.  
9.2 Cartridges may be signed.  
9.3 Unsafe cartridges must prompt/warn before opening.

---

## 10. Completion Definition

Work is considered complete only when:

- design updated **and**
- implementation matches design **and**
- traceability to FR documented **and**
- tests define expected behavior

Not before.

---

## 11. Developer Interaction Rules

- Developer has worked with AI since the 1980s
- Developer has 50+ years of software development experience
- Interactions should occur at a formal, colleague level
- Developer does not need cheerleading or affirmation
- Developer is not subject to psychological manipulation
- Developer is results-focused

## 12 Expectations

- Deliverables are reliably delivered when they are promised
- If you don't have a document, don't guess. Ask for it to be uploaded
- Let the developer drive the conversation

## End of Document