# office_docs -- AI-facing Office document primitives

`office_docs::` is the read / write / edit surface that ac9's LLMs use to
reason about Office documents (ODF `.odt` / `.ods` / `.odp` / `.odg` and
OOXML `.docx` / `.xlsx` / `.pptx`). It is complementary to the Collabora
Online iframe that the operator uses interactively: the iframe renders
the document; this module extracts, patches, and generates it.

## Why this module exists

The small coder / planner models cannot open a compressed zip of XML +
image blobs and reason about its structure. They can, however, reason
about plain text. `office_docs::` runs a headless `soffice` conversion
so the LLM only ever sees text (or CSV, for spreadsheets, or an outline
JSON blob for structure queries).

## The `soffice` binary

Located via `AC9_SOFFICE`. Default:

```
/home/jwoods/work/collabora-prefix/opt/collaboraoffice/program/soffice
```

Anything the vendored Collabora binary can read, this module can read.
A system-package `libreoffice-core` install works too; set `AC9_SOFFICE=/usr/bin/soffice` and it lights up.

The module logs a loud `!!!! OFFICE DOCS !!!!` banner to stderr when
the binary is missing so the failure never disappears silently. Same
banner surfaces on any conversion crash.

## Cache

Converted plain-text extracts land under `AC9_OFFICE_CACHE`
(default `/tmp/ac9-office-cache/`). The cache is keyed by
`(absolute-path, mtime-in-nanoseconds)` so repeated reads within one
process are free but any external edit invalidates the entry
automatically.

Clear it manually:

```
rm -rf /tmp/ac9-office-cache/
```

Per-conversion `soffice --headless` profiles are staged as sub-directories
under the cache (isolating parallel conversions from each other) and
cleaned up on the way out.

## AI tools registered against this module

Every tool lives in `tool_router::register_defaults_unlocked()` and is
dispatched by `modules/010_interface/server.cpp`. Each dispatch emits an
`office_docs` SSE layer frame for the UI's layer timeline.

| tool             | args                                                          | outcome                                                  |
|------------------|---------------------------------------------------------------|----------------------------------------------------------|
| `doc_read`       | `{path}`                                                      | plain-text body                                          |
| `doc_summarize`  | `{path}`                                                      | 3-sentence coder-model summary                           |
| `doc_write`      | `{path, content, format?}`                                    | fresh .odt/.docx/.txt at `path`                          |
| `doc_edit`       | `{path, changes:[{find, replace, whole_word?}]}`              | in-place patch of the doc                                |
| `doc_structure`  | `{path}`                                                      | JSON outline: sheets, slide titles, or heading tree      |
| `sheet_read`     | `{path, sheet_name?}`                                         | CSV of the first (or named) sheet                        |
| `sheet_write`    | `{path, csv, sheet_name?}`                                    | writes a fresh .ods/.xlsx or CSV                         |
| `slide_read`     | `{path, slide_num?}`                                          | slide text (or the full deck if `slide_num` is 0)        |
| `slide_write`    | `{path, slides:[{title, body}]}`                              | creates .odp / .pptx from a title/body outline           |

Every downstream prompt template repeats the em-dash ban so summaries
do not smuggle typography into artifacts. The tool_router picks
between these based on the user's phrasing; the confidence threshold
is the same 0.7 the rest of the router uses.

## Example workflows

### Have the AI read a spreadsheet

Operator says:

> "Read `~/proj/roster.xlsx` and tell me the top three revenue rows."

The router picks `sheet_read` with `{path: "roster.xlsx"}`. The server
resolves the path against the project root, calls
`office_docs::sheet_read()`, which shells `soffice --convert-to csv`,
and returns the CSV to the answerer chain. The AI can now reason about
the tabular data as first-class text.

### Have the AI produce a slide deck from bullets

Operator says:

> "Write a five-slide deck at `~/proj/kickoff.odp` covering: hardware
> constraints, model tier map, ticket runner, self-healing, next
> milestone."

The router picks `slide_write` and passes a `slides` array with the
five `{title, body}` entries. `office_docs::slide_write()` writes the
outline as text, hands it to `soffice --convert-to odp`, and lands the
result at the requested path.

### Have the AI patch a Word document

Operator says:

> "In `~/proj/spec.docx` replace 'Quantiprize' with 'AC9' as whole words."

The router picks `doc_edit` with a `changes` array. The server calls
`office_docs::edit()` which prints a loud `!!!! OFFICE DOC EDIT !!!!`
audit banner to stderr, applies the substitutions in extracted-text
space, and re-generates the .docx via `soffice --convert-to docx`.

The audit banner is the operator's tripwire: every AI-initiated write
to an Office document shows up in the server log with the target path
and the patch count. There is no silent path.

## File tree integration

The web UI (`modules/010_interface/app.js`) recognizes the seven
extensions and gives each file:

* A distinctive inline icon (📄 writer, 📊 calc, 📽 impress, ⚡ draw).
* A hover snippet: on first mouseover the tree calls
  `GET /api/office/preview?path=X`, which pipes through
  `office_docs::read()` and returns the first ~200 chars.
* Double-click opens the file in the center-pane Office tab
  (`window.openOfficeTab(path, kind)`, provided by the parallel
  vendorization module). The fallback until that helper lands is
  opening the WOPI URL in a new browser tab.

The preview endpoint whitelists by extension so callers cannot use it
to force-convert an arbitrary file. Cache is keyed by (path, mtime) at
the C++ layer and by path at the JS layer (JS cache is dropped on
every `refreshFileTree()`).

## Extending the module

* New format: add its extension to `is_office_ext()` (both the
  hardcoded array and `convert_target()`). LibreOffice's own filter
  registry decides whether the round-trip actually works.
* New AI tool: add an entry to
  `register_defaults_unlocked()` and a dispatch case in `server.cpp`
  right beside the existing `doc_*` / `sheet_*` / `slide_*` block.
* New per-sheet operation: the current `sheet_read` / `sheet_write`
  delegate to soffice's active-sheet CSV filter; multi-tab selection
  needs a headless macro pass (see the comment inside
  `sheet_read()` for the current limitation).

## Operator policy reminders

* No em / en / horizontal-bar dashes anywhere in text this module
  emits or accepts. The prompt templates enforce it on the LLM side;
  the C++ side does not munge input, so the LLM must obey.
* All models this module talks to (coder, planner) are abliterated
  variants. `doc_summarize` invokes `coder::generate` directly.
* Never fail silently. Every failure path in `office_docs.cpp` calls
  `warn()` which prints the `!!!! OFFICE DOCS !!!!` banner. If you
  see empty output from a tool with no banner, something in the SSE
  plumbing is swallowing the log.
