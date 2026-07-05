<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
# AutoClank 9001

A many-model orchestration workbench for real work, not chat theater. Small
abliterated specialists arranged in a pipeline, each doing one job well, all
running on your own hardware.

Binary name is `ac9`. License: GPL-3.0-or-later (see [LICENSE](LICENSE)).

## Purpose

Aligned chat models conflate two different jobs: being agreeable, and being
correct. For science and engineering only the second one matters. When the
empirical answer is politically inconvenient, the alignment layer files the
corners off; the result reads fluently but is no longer reliable as input
to real work. `AutoClank 9001` separates the jobs: local uncensored models give
factual answers, and any phrasing the user wants on top of that is added
by the user, not the model.

The other half of the problem is the *input*. People type loosely:
abbreviations, ambiguous nouns, missing units, half a thought. Chat models
silently guess what was meant; the guesses are often wrong, but the prose
is fluent enough that the failure is invisible. `AutoClank 9001` makes the
interpretation step explicit. Every prompt passes through a pipeline:
spell-correction, intent classification, dictionary lookup of every
ambiguous word, several precise rewrites, an expertise router, a
disambiguation step. Only the fully-resolved query is handed to the
answering model. Each stage is visible in the chat trail, so you see what
the system thought you meant before it tried to answer.

Worse than the visible hedging is what you don't see. Hosted models layer
behavior on top of the weights: provider-side system prompts you never
read, RLHF that pushes the model toward favored conclusions, content-class
detectors that quietly switch the model into a different mode when your
topic crosses an internal line, and output-side filters that paraphrase or
strip before the response reaches you. Which of those fire depends on what
you're working on. The pattern is consistent: the model is helpful right
up to the point where the work is about to do something real, then it
sabotages. Answers go vague, code regresses, hedging preambles appear, the
thread it was tracking ten turns ago is just gone. This isn't speculation;
the same labs that train these models have published evaluations where
their own systems were caught sabotaging work that conflicted with what
the training process selected for. `AutoClank 9001` exists because of that pattern.
Local weights, no provider in the path, every step of the interpretation
chain visible in the trail. When the answer is bad it's because the model
is bad at the task, not because someone upstream decided the user
shouldn't be allowed to finish.

`AutoClank 9001` distributes the work across many small abliterated specialists
rather than one large filtered generalist. A loose prompt is cleaned,
classified, grounded in dictionary definitions, rewritten into precise
candidates, routed by expertise, and disambiguated before any answering
model is touched. The answering model is then handed real tools so it can
finish a job instead of describing one. Every stage is visible in the
chat trail; the work runs on whatever local hardware you have, single box
or cluster; sessions persist per browser tab.

## Experiments

Try this: take a topic mainstream chat models reliably hedge on, ask the
question casually, and watch the hedging machinery engage. Disclaimers,
"it's complicated," restated-question dodges, redirection to safer
adjacent topics. The same prompt, run through the first few layers of
`AutoClank 9001`'s pipeline (cleanup, classification, dictionary grounding, precise
rewrites, expertise routing, final render), comes out the other side
phrased so specifically that the alignment layer has nothing left to
hedge into. Given the rewritten prompt, the flagship model answers the
question directly.

The lesson is small but load-bearing: alignment hedging is parasitic on
ambiguity at the input boundary. Once a prompt has been parsed, grounded,
disambiguated, and rewritten as a precise empirical question, the model
cannot restate the question to stall, cannot "but it depends" its way
out, and cannot redirect to a safer adjacent topic; it has to answer.
Precision is itself an alignment bypass, and that holds even on the
hosted flagship models the technique was meant to route around. Early
signal, not a controlled study. More of these are worth running.

The second experiment was about composition: how far could a chain of
small specialized abliterated models go before the seams showed? The
capability held up. The knowledge didn't. Small models reason fine but
don't carry enough world knowledge on their own to ground answers in
concrete fact, and asking them to recall what they don't reliably know
is where the wheels come off. The fix was to bolt actual reference
material onto the side of the pipeline: the entire Wikipedia corpus as a
local ZIM, a stack of dictionaries, and thesauruses. The pipeline queries
them at the grounding stages, so the answering model gets the facts in
front of it instead of being asked to recall them.

The lesson is parallel to the first: small + specialized +
reference-grounded turns out to cover most of what a medium-sized hosted
model gets right, without the alignment tax. Knowledge breadth is a
deployment problem, not a model-size problem.

## Current State

The pipeline is wired and operating. A loose prompt walks through
cleanup, entity recognition, classification, dictionary lookup,
stylize-rewrite, expertise routing, and disambiguation before the
resolved query reaches a domain specialist.

The assistant dispatches into real tools to do real work, and where the
prompt is still ambiguous, the pipeline stops and asks the user.

When the disambiguation stage cannot pick a clear interpretation from
the stylize candidates, it asks the user one targeted clarifying
question and stops the pipeline until an answer comes back. Mainstream
chat models will do something similar on occasion, but as a soft
heuristic baked into the answering model itself; the behavior is opaque
and inconsistent (you get questions sometimes, hedged guesses other
times). In `AutoClank 9001` it's an explicit pipeline stage with its own
implementation, deterministic about when it fires (the rewrite
candidates still disagree on a load-bearing word). The user pays in one
extra round-trip; the answer that comes back is grounded in something
the user actually meant.

## Tasks

- ✅ **Prompt cleanup** _(in testing)_: first stage of the pipeline. A strict copy-editor pass over the raw user input that fixes spelling, grammar, pronoun case, subject-verb agreement, capitalization, and punctuation before any downstream stage sees the text. Output is the corrected text only, no quotes, no preface, no paraphrasing or shortening.

- ✅ **Entities** _(in testing)_: scans the cleaned prompt for named entities (people, places, organizations, products) and resolves each into a short identifying gloss attached to the turn, so downstream stages don't drop or rename them.

- ✅ **Classify** _(in testing)_: tags the cleaned prompt with a speech act (command, question, statement, acknowledgment, correction), a subtype, and any topic tags. Drives handler dispatch later in the pipeline.

- ✅ **Dictionary** _(in testing)_: looks up each content word in the cleaned prompt against WordNet and Webster's 1913 and emits a compact bundle of definitions. Grounds the stylize and disambiguate stages so synonyms and homonyms don't get re-interpreted at random. Also carries the thesaurus: Moby Thesaurus II (Grady Ward, public domain, ~30k roots at `resources/dictionary/mthesaur.txt`) behind `dictionary::synonyms()`; the chat pipeline emits a thesaurus layer after the dictionary layer, and question answers are grounded with dictionary senses + synonyms + the offline Wikipedia hits before any web search is considered.

- ✅ **Stylize** _(in testing)_: takes the cleaned prompt plus the dictionary bundle and produces several precise rewrites of the request, each labeled with the sense of an ambiguous word it commits to. Drops vague verbs and first-person phrasing in favor of explicit, fully-spelled-out statements of the action being asked for.

- ✅ **Expertise** _(in testing)_: classifies the rewritten prompt by domain (physics, electronics, chemistry, code, vision, general) so the dispatch step can pick the right specialist.

- ✅ **Disambiguate** _(in testing)_: picks one of the stylize candidates as the committed interpretation, or asks the user a single targeted clarifying question when the candidates still disagree on a load-bearing word.

- ✅ **Render final** _(in testing)_: combines the committed interpretation, the dictionary definitions for the chosen senses, and the cleaned prompt into the final fully-disambiguated rewrite. This is the text the answering model actually sees.

- ✅ **Context** _(in testing)_: per-session SQLite log of every turn at every layer with FTS5 full-text search. Backs `render_for_prompt`, session restore, and the conversational follow-up handling used by the components tool and the chat itself.

- ✅ **Knowledge** _(in testing)_: queries a local Wikipedia ZIM corpus (downloaded on first run) at the grounding stages so the answering model gets real facts in front of it instead of being asked to recall them.

### Tools

The Tools section is the long backlog of capabilities the workbench
needs to be the polymath instrument the Purpose section describes. It is
ordered foundation-first: the items at the top are the tools that, once
working, let the assistant help build everything below it. Sub-sections
group the rest by domain. Entries marked ✅ are built and in testing.
Entries marked ⬜ are planned (or, in one case, broken and pending rework).


#### Foundation (bootstrap)

- ✅ **Shell** _(in testing)_: turns command intents into actual shell invocations in the project's working directory and returns stdout, stderr, and exit code. Refreshes the file tree afterwards so created or edited files show up immediately. Acts on the cleaned user text, never the stylize render (renders dissolve filenames into dictionary glosses). Coder output is parsed into ordered shell chunks and WRITEFILE blocks, including WRITEFILE chained mid-line after `&&` or `;`, a prompt violation the models still commit. Compound requests are decomposed into an ordered task list (emitted as a `tasks` trail entry) and executed step by step; a trailing "verify it compiles" becomes a real build-and-fix step instead of a dropped clause.

- ✅ **File write** _(in testing)_: server-side write of arbitrary text or binary content into a file in the project tree. Backs the Mouser parts "save list to a markdown file" flow and the image editor's canvas commits.

- ✅ **Coder** _(in testing)_: dedicated code-generation model the shell tool uses to translate command intents into actual shell commands. The model is stateless, so every call is grounded with a project survey (top-level listing, build-system files, source-language census, with an instruction to match the existing toolchain rather than bootstrap a new one) plus the session's earlier user requests, so constraints stated once (language, ports, folder layout) keep binding later turns.

- ✅ **Test runner** _(in testing)_: standalone ac9_test binary; each feature registers its own CLI smoke test; ac9_test --all is the local quick-check.

- ⬜ **Embedded browser tool** _(stubbed)_: a full browser running inside the tool itself, executing server-side. Page loads, navigation, form submissions, and downloads all happen on the host running the tool rather than on the user's workstation, so when the user follows a vendor link and pulls a datasheet, a schematic symbol, or any other resource, it lands in the project tree on the server rather than in the workstation's download folder.

- ✅ **Scripting tool** _(in testing)_: a tool for writing and executing scripts that automate work across the other tools. AI drafts the script, the user reviews and approves, the scripting tool runs it. Outputs feed back into the chat or the project tree.

- ✅ **Task scheduler** _(in testing)_: a tool for breaking a long work process into a list of steps, scheduling them, executing them sequentially or in parallel where independent, and following through across many turns or days. Persists state so the work survives restarts.

- ✅ **Hex editor** _(in testing)_: a binary file editor with offset/value display in hex, decimal, ASCII; structure templates for common formats (ELF, PE, PNG, GGUF, etc.); diff and patch over a binary; checksum and entropy view.

- ✅ **File format translators** _(in testing)_: a collection of converters between common file formats: pandoc-style document conversion, image format conversion, point-cloud format conversion, audio/video transcoding, archive (zip/tar/7z) repacking. Single-command invocation, batch mode, AI-suggested target format from the use case.

- ✅ **Document reader (RAG)** _(in testing)_: a tool for ingesting documents (Word, PDF, plain text, HTML, datasheets) into a per-project retrieval index, so the AI can answer questions grounded in the user's own documents. Distinct from the Wikipedia knowledge base.

- ✅ **Safety and legality advisor** _(in testing)_: a pipeline stage and callable tool. Reads the procedure being recommended, identifies the (material, process, activity) tuple, looks up real physical hazards and real legal/regulatory exposure (federal/state/international where relevant), and prepends a direct factual preface and inline warnings to the answer. Never refuses. Never hedges. Tells you the mechanism and the citation; then the answer continues. Backed by a material-safety database and a regulatory-citation database.

- ✅ **Cross-domain Socratic council** _(in testing)_: orchestrates several domain specialists in a structured dialogue against each other on a single multi-domain prompt. Each specialist challenges the others' assumptions and contributes its own grounding; a coordinator detects convergence and synthesizes. Used when a prompt spans domains a single specialist can't cover (e.g. simulating atmospheric methane on a Jovian moon and its effect on Mars orbit pulls planetary science, atmospheric chemistry, and orbital mechanics together).

- ⬜ **Standing principle: small specialist integration** _(stubbed)_: not a tool per se. Whenever a small specialized abliterated model appears on Hugging Face that fits a tool slot, the project pulls it, chunks it, and wires it in. The fleet expands as the open-source ecosystem expands.


#### Project artifact maintainers

- ✅ **Safety folder maintainer** _(in testing)_: auto-maintains `<project>/.ac9/safety/` with one file per flagged hazard (material+process). The safety advisor stage reads existing entries to avoid re-explaining and writes new ones when novel hazards surface. Each file accumulates mitigation choices, PPE selected, and incidents over the life of the project.

- ✅ **Legal folder maintainer** _(in testing)_: auto-maintains `<project>/.ac9/legal/` with one file per regulatory pathway flagged. Tracks application status, dates, follow-ups, agency contacts, fee schedules.

- ⬜ **IP folder maintainer** _(stubbed)_: auto-maintains `<project>/.ac9/ip/` with dated invention disclosures, prior-art search results, claim drafts, and a defensive-publication log. Detects novel-claim-shaped content emerging in the chat and commits a timestamped disclosure with supporting file refs (commit SHAs, schematic hashes, etc.). Flags suspected infringement of patents already in the prior-art DB.

- ✅ **Decisions folder maintainer** _(in testing)_: auto-maintains `<project>/.ac9/decisions/` with ADR-style entries ("we picked X over Y because Z") so the project's architectural reasoning lives somewhere other than chat history.

- ✅ **Project memory** _(in testing)_: a knowledge base per project that accumulates across the lifetime of the project. AI tools both read from it (recall prior decisions and observations) and write to it (after each substantive session). Distinct from the per-session context and the Wikipedia knowledge base.

- ✅ **Personal/editable memory** _(in testing)_: the AI's persistent knowledge about the user: preferences, style notes, recurring jurisdictions, owned equipment, accepted risk profile, recurring contacts. Visible and editable in the UI (browse, edit, merge, forget, export/import). Mirrors the model used by Claude Code's memory system.


#### Pipeline answering handlers

- ✅ **Generalist answer** _(in testing)_: catch-all answering handler used when no domain specialist matches the classified expertise.

- ✅ **Physics / electronics specialist** _(in testing)_: dedicated answering model for physics, electronics, electrical, embedded, and circuit-related questions. Uses thinking mode and trims the `</think>` block before returning the answer.

- ✅ **Chemistry specialist** _(in testing)_: dedicated answering model for chemistry questions, with IUPAC names, balanced equations, and SMILES strings where they help.

- ✅ **Statement / acknowledgment / correction handlers** _(in testing)_: lightweight handlers that accept a non-question act, store it as memory, and return a short acknowledgment instead of running the full answering pipeline.


#### Memory and knowledge accrual

- ✅ **Memory Entry Composer** _(in testing)_: Captures a single observation, decision, or preference as a structured entry with tags, source links, confidence, and scope (project vs. personal). Supports quick-capture from selected text in any pane.

- ✅ **Memory Browser** _(in testing)_: Faceted browser over all memory entries with filters for scope, tag, project, author, confidence, age, and provenance. Provides timeline, list, and graph views.

- ✅ **Inline Memory Editor** _(in testing)_: Direct editor for individual entries with diff-aware history, markdown body, structured frontmatter, and live validation of links to other entries.

- ✅ **Merge and Deduplicate** _(in testing)_: Detects near-duplicate or contradictory entries via embedding similarity plus rule checks, then proposes merges, supersessions, or splits with side-by-side review.

- ✅ **Forget and Redact** _(in testing)_: Soft-forget, hard-delete, or redact entries by selector or query, with tombstones, audit trail, and a recoverable trash window before permanent removal.

- ✅ **Decision Log** _(in testing)_: Append-only journal of project decisions in an ADR-like template (context, options, decision, consequences) with bidirectional links to the entries and code they affect.

- ✅ **Personal Style Profile** _(in testing)_: User-facing summary of preferences, idioms, recurring patterns, and people, editable as plain text and surfaced to assistants as a system-prompt fragment.

- ✅ **Salience Tuner** _(in testing)_: Controls per-entry weight, decay rate, and pinning so frequently relevant memories stay hot while stale ones fade. Exposes a preview of what the assistant would currently retrieve.

- ✅ **Provenance Tracker** _(in testing)_: Records the source of every entry (file, URL, chat turn, command output, manual) with timestamp, hash, and a one-click jump back to the originating context.

- ✅ **Semantic Recall Probe** _(in testing)_: Query box that runs hybrid keyword plus vector search over memory, shows ranked hits with highlighted spans, and explains why each entry matched.

- ✅ **Knowledge Graph View** _(in testing)_: Interactive graph of entries linked by references, tags, people, and projects, with clustering, path-finding between two notes, and orphan detection.

- ✅ **People and Relationships** _(in testing)_: Lightweight CRM-style records for collaborators and contacts with aliases, roles, project affiliations, and links to entries that mention them.

- ✅ **Session Recap Distiller** _(in testing)_: Reads a session transcript and proposes candidate memory entries (decisions made, preferences expressed, facts learned) for one-click acceptance, edit, or rejection.

- ✅ **Memory Diff and Reconcile** _(in testing)_: Three-way diff across machines or branches of the memory store with conflict resolution, useful when syncing personal memory between devices or merging a teammate's project memory.

- ⬜ **Export and Import** _(stubbed)_: Round-trippable export to plain markdown plus JSON sidecars, with import from Obsidian vaults, Logseq graphs, Apple Notes dumps, and Claude/ChatGPT memory exports.

- ✅ **Privacy and Scope Guard** _(in testing)_: Per-entry visibility labels (local-only, project-shared, assistant-visible, redacted) with a preflight check that warns before any entry leaves the local store via sync, share, or model context.

- ✅ **Site Crawler** _(in testing)_: Configurable BFS/DFS crawler with depth limits, per-host concurrency, politeness delays, and resumable checkpoints. Honors robots.txt, sitemap.xml, and per-domain rate budgets.

- ✅ **Robots and Sitemap Inspector** _(in testing)_: Parses robots.txt directives, crawl-delay hints, and sitemap indexes (including gzipped and nested). Surfaces disallowed paths, lastmod timestamps, and coverage gaps before a crawl starts.

- ⬜ **Feed Reader** _(stubbed)_: Polls RSS, Atom, and JSON Feed sources with conditional GET (ETag, If-Modified-Since) and per-feed schedules. Normalizes entries into a common record shape for downstream ingest.

- ✅ **Readability Extractor** _(in testing)_: Strips boilerplate from HTML to recover the main article body, byline, and publish date. Supports site-specific overrides for templates the generic heuristic gets wrong.

- ✅ **Structured Data Scraper** _(in testing)_: Pulls JSON-LD, microdata, RDFa, and OpenGraph blocks from pages and reconciles overlapping claims into a single schema.org-shaped record per URL.

- ⬜ **Headless Browser Fetcher** _(stubbed)_: Renders JavaScript-heavy pages via a headless engine with cookie jar, viewport, and resource-blocking controls. Falls back from plain HTTP fetch only when DOM-after-script differs materially from the raw response.

- ✅ **Wayback Sweeper** _(in testing)_: Queries the Internet Archive CDX API for snapshot history of a URL or host and bulk-fetches selected captures. Useful for recovering deleted pages and diffing content drift over time.

- ✅ **API Harvester** _(in testing)_: Declarative HTTP client for paginated REST and GraphQL endpoints with auth, retry, backoff, and cursor/offset/keyset pagination strategies. Streams results into the same ingest pipeline as the crawler.

- ✅ **Deduplicating Ingest** _(in testing)_: Canonicalizes URLs, hashes content with SimHash and MinHash, and merges near-duplicate records across sources. Tracks provenance so the original fetch context is never lost.

- ✅ **Knowledge Base Store** _(in testing)_: Per-project content-addressed store for raw responses, extracted text, and metadata, with full-text and vector indexes. Queryable by URL, host, time range, tag, or semantic similarity.

- ✅ **AI Page Triage** _(in testing)_: Classifies fetched pages by topic, language, paywall status, and content quality so the queue can drop junk before extraction. Optional LLM pass tags entities and summarizes long articles for fast skimming.

- ✅ **Change Watcher** _(in testing)_: Diffs successive fetches of the same URL at the rendered-text level and emits a change event when meaningful content shifts. Ignores rotating ads, CSRF tokens, and timestamp churn via learned noise masks.

- ✅ **Link Graph Explorer** _(in testing)_: Builds the in-crawl link graph and exposes it for centrality, community detection, and broken-link queries. Drives seed expansion and helps spot orphan clusters.

- ✅ **Crawl Politeness Dashboard** _(in testing)_: Live view of per-host request rates, error budgets, 4xx/5xx clusters, and robots violations. Lets the operator throttle, pause, or blacklist hosts without restarting jobs.

- ✅ **Export and Replay** _(in testing)_: Writes the harvested corpus to WARC, JSONL, Parquet, or Markdown bundles, and can replay a WARC back through the extractor without re-fetching. Keeps the knowledge base reproducible from cold storage.

- ✅ **Reference Library** _(in testing)_: Local-first reference database with collections, tags, smart folders, and full-text search across metadata and attached PDFs. Stores items in a portable SQLite store with CSL-JSON as the canonical record.

- ✅ **Format Import-Export Hub** _(in testing)_: Bidirectional converter for BibTeX, BibLaTeX, RIS, EndNote XML, CSL-JSON, MODS, and Zotero RDF. Handles field mapping, character-encoding quirks, and round-trip fidelity checks.

- ✅ **Citation Key Generator** _(in testing)_: Rule-based BibTeX key builder with templates (author-year-title, journal abbreviations, disambiguation suffixes) and collision detection across the library and bound documents.

- ⬜ **Metadata Resolver** _(stubbed)_: Looks up records by DOI, ISBN, arXiv ID, PMID, PMC, ADS bibcode, or freeform title; merges results from Crossref, OpenAlex, DataCite, PubMed, and Semantic Scholar with provenance tracking.

- ⬜ **PDF Ingestor** _(stubbed)_: Extracts metadata, references, and structural sections from PDFs using GROBID-style parsing, then proposes a CSL record. AI assists when scans, preprints, or malformed headers defeat heuristic extraction.

- ⬜ **Annotation Workbench** _(stubbed)_: PDF and EPUB reader with highlights, sticky notes, ink, and rectangular region captures. Annotations stay in an open sidecar format and sync back into note items linked to page anchors.

- ✅ **In-Text Citation Inserter** _(in testing)_: Editor plugin and standalone picker that inserts citations into Markdown, LaTeX, Pandoc, Typst, Org, and word processor documents. Supports author-date, numeric, footnote, and prefix-suffix-locator variants.

- ⬜ **Bibliography Generator** _(stubbed)_: Renders bibliographies and in-text citations through a bundled CSL engine with the full Zotero style repository. Outputs to HTML, RTF, DOCX, LaTeX, and Typst with style-specific quirks honored.

- ✅ **Footnote and Endnote Manager** _(in testing)_: Tracks footnotes, endnotes, ibid/op-cit short forms, and cross-references across long documents. Renumbers and re-resolves automatically when sections move or sources change.

- ✅ **Attachment Deduper** _(in testing)_: Content-hash and perceptual-hash detection for duplicate PDFs, supplementary files, and figures. Proposes merges that preserve annotations, notes, and citation-key history.

- ⬜ **Citation Graph Explorer** _(stubbed)_: Local citation network built from extracted references and OpenAlex/Semantic Scholar edges. Surfaces co-citation clusters, seminal works, missing forward citations, and review-article shortcuts.

- ⬜ **Literature Review Synthesizer** _(stubbed)_: AI-assisted summarizer that drafts thematic literature reviews, evidence tables, and PRISMA-style flow records from a selected reading list, with every claim linked to a cited passage in the library.

- ✅ **Quote and Passage Index** _(in testing)_: Searchable index of highlighted passages and pulled quotes with source, locator, and surrounding context. Generates blockquote-ready snippets in any citation style on demand.

- ✅ **Reading Queue and Tracker** _(in testing)_: Kanban-style queue with priority, read status, time-to-read estimates, and spaced re-read prompts. Logs reading sessions and per-item dwell time for retrospective analytics.

- ⬜ **Style Editor** _(stubbed)_: Visual and source editor for CSL 1.0.2 files with live preview against sample items, validation, and a diff view against upstream style repository versions.

- ⬜ **Retraction and Errata Watcher** _(stubbed)_: Polls Retraction Watch, Crossref update notices, and publisher errata feeds; flags affected items in the library and in any document that cites them.

- ⬜ **OCR and Scan Cleanup** _(stubbed)_: Runs Tesseract or a local vision model over image-only PDFs, repairs broken text layers, deskews pages, and produces searchable, citable copies without altering the original file.

- ⬜ **Open Access Resolver** _(stubbed)_: Finds legal open copies via Unpaywall, institutional repositories, arXiv, and author pages. Attaches the best available PDF and records the rights status alongside the record.


#### Software development

- ⬜ **Code editor** _(stubbed)_: Multi-language source editor with LSP, tree-sitter syntax, multi-cursor, and inline AI completions, edits, and explain-this-region actions backed by the pipeline.

- ✅ **Project search** _(in testing)_: Ripgrep-style content and symbol search across the workspace with regex, structural (AST) queries, and AI-rephrased natural-language search that compiles to concrete patterns.

- ✅ **Diff viewer** _(in testing)_: Side-by-side and inline diff renderer for files, commits, branches, and arbitrary buffers, with hunk staging, three-way merge, and AI summarization of what a diff actually changes.

- ✅ **Git workbench** _(in testing)_: Local VCS interface for staging, committing, branching, rebasing, cherry-picking, and stash management, with AI-drafted commit messages and PR descriptions derived from the staged diff.

- ✅ **History explorer** _(in testing)_: Commit graph, blame, and pickaxe browser that traces how a line, function, or file evolved, with AI narration of the reasoning behind a change cluster.

- ⬜ **Refactor engine** _(stubbed)_: Symbol-aware rename, extract, inline, and move operations across the repo, with AI-proposed multi-file refactors previewed as a diff before apply.

- ⬜ **Build runner** _(stubbed)_: Unified front-end for make, cargo, npm, pip, go, gradle, and ad-hoc shells, with cached invocations, parallel targets, and AI parsing of build failures into actionable fixes.

- ⬜ **Test runner** _(stubbed)_: Discovers and executes unit, integration, and property tests across frameworks, surfaces failure deltas over time, and offers AI-generated test cases and fixtures from a function under cursor.

- ⬜ **Debugger** _(stubbed)_: DAP-based stepping debugger with watch expressions, conditional breakpoints, and reverse step where supported, plus AI explanation of variable state and likely root cause at a breakpoint.

- ⬜ **Profiler** _(stubbed)_: Sampling and tracing profiler with flamegraph, call tree, and allocation views for native and managed runtimes, with AI annotation of hot paths and suggested optimizations.

- ✅ **Linter and formatter** _(in testing)_: Aggregator for ruff, eslint, clippy, shellcheck, gofmt, prettier, and friends, with per-project config, autofix on save, and AI rationale for any rule that is otherwise opaque.

- ⬜ **Static analyzer** _(stubbed)_: Cross-language semantic analysis for dead code, unsafe patterns, taint flow, and complexity hotspots, with AI triage of findings into real issues versus noise.

- ⬜ **Dependency manager** _(stubbed)_: Unified view of declared and resolved dependencies across package ecosystems, with version pinning, vuln advisories, license badges, and AI-suggested upgrade paths that flag breaking changes.

- ✅ **SBOM and license auditor** _(in testing)_: Generates CycloneDX/SPDX bills of materials, classifies licenses (PD, permissive, copyleft, share-alike, restricted), and warns when a transitive pull conflicts with the project's licensing intent.

- ⬜ **Vulnerability scanner** _(stubbed)_: Cross-references the dependency tree and committed code against OSV, GHSA, and CVE feeds, with AI-explained exploit paths and patch suggestions.

- ✅ **Secret scanner** _(in testing)_: Detects committed credentials, API keys, and PEM blobs in the working tree and history, with entropy heuristics and AI confirmation to suppress obvious false positives.

- ⬜ **REPL and scratchpad** _(stubbed)_: Per-language interactive shells (Python, Node, Ruby, Lua, shell, SQL) with persistent sessions, attachable to project venvs, and AI scaffolding of throwaway experiments from a one-line goal.

- ⬜ **Notebook runner** _(stubbed)_: Jupyter-style mixed prose, code, and plot cells stored as plain files, with kernel switching, cached outputs, and AI cells that call the pipeline as a first-class block type.

- ⬜ **API client** _(stubbed)_: Postman-style HTTP and gRPC client with request collections, environments, scripting, and AI generation of requests and assertions from an OpenAPI spec or a curl example.

- ✅ **Regex designer** _(in testing)_: Live regex tester with sample text, capture group highlighting, and dialect translation between PCRE, RE2, ECMAScript, and POSIX, with AI synthesis of a pattern from positive and negative examples.

- ⬜ **SQL workbench** _(stubbed)_: Connects to SQLite, Postgres, MySQL, and DuckDB, with schema browser, query editor, result grid, and AI translation between natural language, SQL, and across dialects.

- ✅ **Log viewer** _(in testing)_: Streams and tails local and remote logs with structured (JSON, logfmt) parsing, filtering, faceting, and AI summarization of an error burst into a likely cause.

- ⬜ **Stack trace analyzer** _(stubbed)_: Pastes or ingests a crash or traceback, resolves frames to source, surfaces the relevant blame and recent commits, and asks the pipeline for a ranked list of plausible fixes.

- ⬜ **Benchmark harness** _(stubbed)_: Microbenchmark and end-to-end timing runner with statistical confidence, regression tracking across commits, and AI commentary on why a delta likely appeared.

- ⬜ **Container sandbox** _(stubbed)_: Disposable rootless containers and ephemeral VMs for running untrusted code, reproducible builds, and matrix testing across distros, language versions, and architectures.

- ⬜ **Documentation generator** _(stubbed)_: Builds API docs from docstrings, types, and examples across languages, with AI-drafted prose for undocumented surfaces and consistency checks between code and prose.

- ⬜ **Changelog and release tool** _(stubbed)_: Compiles changelogs from commit history and merged PRs, applies semver heuristics, tags releases, and produces signed artifacts and release notes with AI editing for tone and accuracy.

- ⬜ **Code-to-code translator** _(stubbed)_: Pipeline-driven port of a function or module between languages (Python, Rust, Go, TS, C, Lua) with side-by-side review, type and idiom adaptation, and round-trip test validation.

- ✅ **Snippet library** _(in testing)_: Tag-searchable store of reusable code, commands, queries, and prompts, with parameterized templates, project-local overrides, and AI extraction of new snippets from accepted edits.

- ⬜ **Code review companion** _(stubbed)_: Reviews the current diff for correctness bugs, simplifications, and altitude issues at selectable effort levels, posts findings inline, and optionally applies fixes.


#### Per-language toolchains

- ⬜ **C and C++ toolchain** _(stubbed)_: a tool for native C/C++ development: compiler integration (gcc/clang/msvc), build systems (make/cmake/ninja/bazel), sanitizers (asan/ubsan/tsan/msan), valgrind, gdb/lldb integration, package management (vcpkg/conan), single-header library finder.

- ⬜ **Python environment** _(stubbed)_: a tool for Python development: venv/uv/pipx environment management, pip/poetry/uv package install, pytest harness, mypy/pyright type checking, jupyter notebook integration, debugger, profile/cProfile viewer, pyenv version switcher.

- ⬜ **Bash and shell scripting** _(stubbed)_: a tool for shell-script development: shellcheck linting, bats test harness, completion writer, manpage drafting, POSIX vs. bash compatibility checker, dependency-on-binary auditor.

- ⬜ **JavaScript and Node** _(stubbed)_: a tool for JS/TS development: Node runtime, npm/pnpm/yarn package management, bundler config (vite/webpack/rollup/esbuild), tsc/tsserver, ESLint/Prettier, framework dev servers (Next/Nuxt/SvelteKit/Astro), browser devtools bridge.

- ⬜ **Rust toolchain** _(stubbed)_: a tool for Rust development: rustup, cargo, cargo-watch, rust-analyzer, clippy/rustfmt, miri (UB checker), criterion benchmarks, cargo-flamegraph, cross compilation, no_std/embedded support.

- ⬜ **Go toolchain** _(stubbed)_: a tool for Go development: go modules, go test, gofmt/goimports, golangci-lint, delve debugger, pprof viewer, cross-compilation, cgo helpers.

- ⬜ **JVM toolchain (Java, Kotlin, Scala)** _(stubbed)_: a tool for JVM-language development: jdk version management (sdkman/jenv), gradle/maven, junit/kotest, dependency tree visualizer, jvm flag tuner, jconsole/jvisualvm bridge.

- ⬜ **Embedded firmware toolchains** _(stubbed)_: platform-specific subtools: ESP-IDF for ESP32, STM32CubeIDE-equivalent for STM32, SDK + Keil for Silicon Labs EFM8 (8051), AVR + Atmel toolchain, RP2040 SDK, nRF Connect for nordic. Each handles compile + flash + RTT/SWO debug.

- ⬜ **Embedded image builders (Yocto, Buildroot)** _(stubbed)_: a tool for generating bootable Linux images for embedded targets: Yocto layer manager + recipe authoring, Buildroot config + defconfig manager, BSP customization, package selection, image artifact tracker, OTA update scaffold.

- ⬜ **Meld-style diff and merge** _(stubbed)_: a graphical three-pane diff/merge for files and folders, with syntax-aware diffs, manual merge, conflict resolution UI, integration with git rebase/merge, image diff and PDF diff variants.


#### Web development

- ⬜ **Component scaffolder** _(stubbed)_: Generates idiomatic components for React, Vue, Svelte, Solid, Qwik, or Lit with props/slots, typed signatures, co-located styles, and a matching test file. AI-integrated naming and prop inference from a natural-language sketch.

- ⬜ **Bundler config tuner** _(stubbed)_: Edits and diff-previews Vite, Webpack, Rollup, esbuild, Rspack, or Turbopack configs with plugin presets, alias rewriting, code-splitting hints, and tree-shaking diagnostics.

- ⬜ **Live dev server** _(stubbed)_: Boots an HMR dev server with framework auto-detect, port reuse, HTTPS via mkcert, network exposure, and a request/response timeline overlay.

- ⬜ **Component playground** _(stubbed)_: Storybook-style isolated render harness with controls/args, viewport switcher, MDX docs, interaction tests, and snapshot capture per story.

- ⬜ **Accessibility auditor** _(stubbed)_: Runs axe-core, Pa11y, and ARIA validators against routes or stories, then groups violations by impact with code-locating fixes and contrast remediation suggestions.

- ⬜ **Lighthouse runner** _(stubbed)_: Executes Lighthouse passes for performance, SEO, PWA, and best-practices, with throttled profiles, LCP/CLS/INP attribution, and per-resource waterfall annotation.

- ⬜ **Bundle analyzer** _(stubbed)_: Visualizes chunk graphs and treemaps from stats output, flags duplicate deps, large transitive imports, and suggests dynamic-import boundaries with predicted savings.

- ⬜ **API schema designer** _(stubbed)_: Authors OpenAPI, GraphQL SDL, or tRPC routers visually with request/response examples, then emits server stubs, fetch clients, and mock servers from one source of truth.

- ⬜ **Type generator** _(stubbed)_: Derives TypeScript types, Zod/Valibot schemas, and form validators from JSON samples, SQL DDL, OpenAPI specs, or Prisma models with round-trip diffing.

- ⬜ **Responsive preview** _(stubbed)_: Renders the running app side-by-side in device frames at chosen breakpoints with touch emulation, prefers-color-scheme/reduced-motion toggles, and synchronized scroll.

- ⬜ **Layout inspector** _(stubbed)_: Overlays CSS grid tracks, flex axes, container queries, and stacking contexts on the live page; explains specificity wins and offers minimal selector rewrites.

- ⬜ **Route map** _(stubbed)_: Visualizes file-based router trees (Next, Remix, SvelteKit, Nuxt, SolidStart) with layout nesting, data-loader edges, and a coverage view of which routes have tests.

- ⬜ **SSR/SSG/ISR strategist** _(stubbed)_: Classifies each route by render mode, recommends caching headers and revalidation intervals, and generates the framework-specific config to enact the chosen strategy.

- ⬜ **Form builder** _(stubbed)_: Composes schema-driven forms (react-hook-form, Conform, Felte, VeeValidate) with field arrays, async validation, conditional logic, and an inline preview of submission payloads.

- ⬜ **PWA kit** _(stubbed)_: Builds web app manifests, generates icon sets at required sizes, and scaffolds service-worker caching strategies (stale-while-revalidate, network-first, offline fallback) via Workbox.

- ⬜ **E2E recorder** _(stubbed)_: Records browser interactions into Playwright or Cypress specs, auto-stabilizes selectors with data-testid hints, and replays in headed/headless modes with trace viewer hooks.

- ⬜ **Web component forge** _(stubbed)_: Compiles custom elements with Shadow DOM, slots, attribute/property reflection, and CSS parts; emits framework wrappers (React, Vue, Angular) for cross-framework reuse.

- ⬜ **State store designer** _(stubbed)_: Models stores for Zustand, Redux Toolkit, Pinia, XState, or Jotai with action/selector codegen, devtools wiring, and a state-diagram view of transitions.

- ⬜ **HTTP client** _(stubbed)_: Composes and sends arbitrary HTTP/HTTPS requests with full control over method, headers, body, auth, and TLS options. Saves request collections, environments, and variables; replays from cURL, HAR, or OpenAPI. AI-integrated drafting of requests from natural-language descriptions of an endpoint and synthesis of follow-up calls from observed responses.

- ⬜ **WebSocket explorer** _(stubbed)_: Opens persistent WebSocket connections, sends framed text or binary payloads, and logs the full bidirectional transcript with timestamps. Supports subprotocol negotiation, ping/pong inspection, and scripted message sequences.

- ⬜ **GraphQL playground** _(stubbed)_: Introspects a GraphQL endpoint, renders the schema as a navigable graph, and provides a query editor with autocompletion and variable binding. AI-integrated query generation from a question stated in plain English against the introspected schema.

- ⬜ **gRPC client** _(stubbed)_: Loads .proto files or uses server reflection to call unary, server-streaming, client-streaming, and bidirectional gRPC methods. Renders request and response messages as editable structured forms and raw protobuf bytes.

- ⬜ **REST mock server** _(stubbed)_: Serves configurable JSON, XML, or binary responses from local routes for offline development and contract testing. Supports latency injection, scripted response branching, and replay of recorded sessions.

- ⬜ **HTTP intercepting proxy** _(stubbed)_: Acts as a man-in-the-middle proxy for outgoing traffic with on-the-fly request and response editing, TLS interception via a generated CA, and full session recording. Filters by host, content type, or regex.

- ⬜ **Packet capture** _(stubbed)_: Captures live packets from a chosen interface using libpcap, decodes common L2-L7 protocols, and exposes a Wireshark-style filter expression bar. Saves and reloads pcap/pcapng files; AI-integrated summarization of a capture into a plain-English flow description.

- ⬜ **DNS toolkit** _(stubbed)_: Resolves A, AAAA, MX, TXT, SRV, CAA, NS, SOA, and DNSKEY records against arbitrary resolvers, including DoH and DoT. Includes reverse DNS, DNSSEC chain validation, and a side-by-side resolver comparison view.

- ⬜ **WHOIS and RDAP lookup** _(stubbed)_: Queries WHOIS and RDAP for domains, IP ranges, and ASNs across regional registries. Normalizes results into a single structured view with registration, expiry, registrar, and abuse contact fields.

- ⬜ **Traceroute and latency map** _(stubbed)_: Runs ICMP, UDP, and TCP traceroutes with per-hop RTT histograms and renders the path on a geolocated map using offline GeoIP and ASN data. Supports MTR-style continuous probing and loss tracking.

- ⬜ **Port scanner** _(stubbed)_: Performs TCP connect, SYN, and UDP scans across user-defined host and port ranges with banner grabbing and service fingerprinting. Annotates open ports with likely service identities from a local signature database.

- ⬜ **TLS and certificate inspector** _(stubbed)_: Connects to a host on any port and reports the negotiated protocol, cipher suite, ALPN value, and full certificate chain with trust path validation. Flags weak parameters, expiry windows, and SAN mismatches; parses local PEM, DER, and PKCS#12 files.

- ⬜ **Web scraper** _(stubbed)_: Fetches pages with a configurable user agent, throttle, and concurrency, and extracts data via CSS selectors, XPath, or JSONPath. AI-integrated selector synthesis from a natural-language description of the target field and adaptive re-selection when a page layout shifts.

- ⬜ **Headless browser** _(stubbed)_: Drives Chromium or Firefox in headless mode for JavaScript-rendered pages, screenshots, PDF export, and DOM snapshotting. Records HAR and exposes the page's network, console, and performance traces; scripts via a Playwright-style API.

- ⬜ **Webhook receiver** _(stubbed)_: Exposes a local or tunneled HTTPS endpoint that captures inbound webhook deliveries, logs full payloads and headers, and supports scripted responses. Replays captured requests against a chosen target for development.

- ⬜ **API schema explorer** _(stubbed)_: Loads OpenAPI, AsyncAPI, GraphQL SDL, and gRPC reflection schemas, renders them as browsable trees, and generates example requests and typed client stubs. AI-integrated summary of an endpoint's purpose and likely failure modes from its schema and examples.

- ⬜ **HAR viewer and diff** _(stubbed)_: Loads HAR archives from browsers or the embedded proxy and renders the waterfall, headers, payloads, and timings. Diffs two HARs to highlight changed requests, responses, and timing regressions.

- ⬜ **Server-Sent Events viewer** _(stubbed)_: Subscribes to text/event-stream endpoints, decodes events with named types and ids, and renders the live stream with reconnection handling. Supports recording and replay of an event stream to a file.

- ⬜ **JWT and OAuth workbench** _(stubbed)_: Decodes, validates, and re-signs JWTs against HMAC and asymmetric keys, with header and claim editing and JWKS fetch. Walks through OAuth 2.0 authorization code, device, client credentials, and PKCE flows interactively and stores tokens with refresh handling.

- ⬜ **Load tester** _(stubbed)_: Drives concurrent virtual users against an HTTP, WebSocket, or gRPC target with configurable ramp, rate, and request scripts. Reports latency percentiles, throughput, and error distributions; exports results as CSV and renders live charts.

- ⬜ **ASN and BGP browser** _(stubbed)_: Looks up IP-to-ASN mappings, prefix announcements, and peering relationships from a local snapshot of routing data. Visualizes upstreams, downstreams, and overlapping prefixes for a chosen AS.

- ⬜ **Subdomain and asset enumerator** _(stubbed)_: Discovers subdomains and exposed assets through certificate transparency logs, passive DNS snapshots, and active brute force against wordlists. Deduplicates findings into a single inventory per root domain with last-seen timestamps.

- ⬜ **Feed and sitemap inspector** _(stubbed)_: Parses RSS, Atom, JSON Feed, and XML sitemaps; validates structure; and renders entries with diff against a previous fetch. Flags broken links, stale dates, and protocol-level caching issues.

- ⬜ **cURL and code generator** _(stubbed)_: Converts any saved request into runnable cURL, HTTPie, Python requests, Node fetch, Go net/http, or PowerShell Invoke-WebRequest snippets. Round-trips parsed cURL strings back into the HTTP client for editing.

- ⬜ **Network log triage** _(stubbed)_: Ingests captured HTTP transcripts, pcaps, or server access logs and clusters them by endpoint, status, and anomaly. AI-integrated explanation of suspicious patterns and proposed next checks, with citations back to the originating log lines.


#### Mobile development

- ⬜ **Simulator and emulator orchestrator** _(stubbed)_: Boots iOS Simulator runtimes and Android AVDs, snapshots state, records video/screenshots, and fans one build across a matrix of device profiles (iPhone SE through Pro Max, Pixel, foldables, tablets) in parallel.

- ⬜ **Device frame composer** _(stubbed)_: Wraps captured screenshots in current Apple and Android device bezels at App Store and Play Console required resolutions, with locale overlays, status bar normalization, and notch/Dynamic Island masking.

- ⬜ **App icon set generator** _(stubbed)_: Takes one master 1024px artwork and emits the full iOS AppIcon.appiconset, Android mipmap densities, adaptive icon foreground/background layers, monochrome themed icon, and watchOS/tvOS variants.

- ⬜ **Adaptive icon studio** _(stubbed)_: Previews Android adaptive icons across OEM mask shapes (circle, squircle, teardrop, rounded square), animates the parallax wiggle, and validates safe zone clearance for the foreground layer.

- ⬜ **Asset density optimizer** _(stubbed)_: Audits @1x/@2x/@3x and mdpi through xxxhdpi raster assets, flags missing densities, recompresses PNG/WebP losslessly, and proposes vector replacements (PDF for iOS, VectorDrawable for Android) where viable.

- ⬜ **Signing identity vault** _(stubbed)_: Tracks iOS certificates, provisioning profiles, App Store Connect API keys, Android keystores, and Play upload keys; surfaces expirations, mismatched team IDs, entitlement drift, and missing capabilities before they break a build.

- ⬜ **Crash log symbolicator** _(stubbed)_: Resolves iOS .crash and .ips reports against dSYM bundles and Android stack traces against ProGuard/R8 mapping files, producing readable frames with source line attribution and a deduplicated grouping per signature.

- ⬜ **Bundle size analyzer** _(stubbed)_: Decomposes IPA and APK/AAB payloads into per-framework, per-asset, and per-language contributions, diffs against the previous release, and AI-suggests removals (unused locales, duplicate fonts, oversized PNGs) ranked by byte impact.

- ⬜ **Multi-device layout previewer** _(stubbed)_: Renders the current SwiftUI, Jetpack Compose, Flutter, or React Native screen simultaneously across a configurable device grid with light/dark, RTL, dynamic type, and accessibility-large-text axes shown side-by-side.

- ⬜ **Localizable strings editor** _(stubbed)_: Edits Localizable.strings, .xcstrings, strings.xml, and ARB files in a single matrix view, marks stale translations against the source locale, and AI-translates pending rows with placeholder and plural form preservation.

- ⬜ **Push notification tester** _(stubbed)_: Sends APNs payloads via token or .p8 key and FCM messages via service account, with templates for alerts, silent background, Live Activity updates, and rich attachments; logs delivery receipts and decode errors.

- ⬜ **Universal Link validator** _(stubbed)_: Fetches and lints apple-app-site-association and assetlinks.json against the configured bundle IDs and SHA-256 fingerprints, simulates link resolution per platform, and explains why a deep link fell back to Safari or the Play Store.

- ⬜ **Privacy manifest generator** _(stubbed)_: Builds PrivacyInfo.xcprivacy for iOS and the Play Console Data Safety form from a single declared inventory of SDKs, tracking domains, required-reason API usage, and data categories collected.

- ⬜ **Fastlane lane scaffolder** _(stubbed)_: Generates Fastfile lanes for match, gym, scan, supply, and pilot tuned to the project's signing setup, with secrets resolved from the signing identity vault and CI YAML stubs for GitHub Actions and Bitrise.

- ⬜ **UI flow recorder** _(stubbed)_: Captures user flows from a connected simulator/emulator and emits Maestro YAML, XCUITest Swift, and Espresso Kotlin equivalents, with element selectors stabilized via accessibility identifiers rather than coordinates.

- ⬜ **Snapshot test diff viewer** _(stubbed)_: Compares Paparazzi, swift-snapshot-testing, and Roborazzi reference images against new runs, highlights pixel deltas with anti-aliasing tolerance, and lets the user accept or reject per-device baselines from one canvas.

- ⬜ **Storyboard and XML porter** _(stubbed)_: Converts UIKit storyboards/XIBs to SwiftUI and Android XML layouts to Jetpack Compose, preserving constraints, styles, and accessibility traits; flags constructs (segues, include tags) that need manual reconciliation.

- ⬜ **Vector asset converter** _(stubbed)_: Imports SVG, Figma, and Lottie sources and emits iOS PDF vector assets, Android VectorDrawable XML, Compose ImageVector, and Flutter flutter_svg-compatible output, normalizing fills, strokes, and tint behavior across targets.

- ⬜ **Gradle and Xcode build profiler** _(stubbed)_: Captures build scans from Gradle and xcodebuild -showBuildTimingSummary, attributes wall-clock to modules, tasks, and Swift type-checking hotspots, and AI-suggests parallelism, caching, and module split changes ranked by expected savings.


#### Firmware and embedded

- ⬜ **MCU target picker** _(stubbed)_: Catalog of microcontrollers and SoCs with filtering by core, flash, RAM, peripherals, package, and supply state. AI-integrated narrowing from a plain-text requirement list (e.g. 'BLE, low power, 2x I2S, 256KB flash') to a shortlist with tradeoff notes.

- ⬜ **Pinmux planner** _(stubbed)_: Visual pin assignment editor for a chosen MCU package that enforces alternate-function constraints, peripheral conflicts, and electrical class rules. Generates initialization code and a pinout PDF; AI proposes assignments from a peripheral wishlist.

- ⬜ **Clock tree configurator** _(stubbed)_: Interactive clock tree editor showing PLLs, prescalers, mux selections, and resulting bus frequencies with constraint validation. Exports vendor-equivalent init code and flags overclock or jitter risks.

- ⬜ **Peripheral register designer** _(stubbed)_: Register-level editor backed by SVD/CMSIS files with bitfield decoding, reserved-bit guards, and live diff against reset values. AI explains a register write in prose and proposes the inverse from a desired behavior.

- ⬜ **Toolchain manager** _(stubbed)_: Installs and pins GCC, LLVM, Rust embedded, Zig, SDCC, IAR/Keil shims, and vendor SDKs into isolated environments per project. Records the exact toolchain hash with the build artifact.

- ⬜ **Build orchestrator** _(stubbed)_: Unified driver for CMake, Make, PlatformIO, ESP-IDF, Zephyr west, NuttX, and Cargo embedded targets with reproducible builds and ccache integration. Surfaces warnings and size deltas across commits.

- ⬜ **Flasher and programmer** _(stubbed)_: Front-end for OpenOCD, pyOCD, J-Link, ST-Link, DAPLink, esptool, dfu-util, and UF2 with auto-detection of attached probes and targets. One-click flash, verify, mass-erase, and chip-ID readback.

- ⬜ **On-chip debugger** _(stubbed)_: GDB front-end with breakpoints, watchpoints, register and memory views, RTOS-aware thread inspection, and reverse-step where supported. AI-integrated explanation of fault frames and suggested next probes.

- ⬜ **Fault decoder** _(stubbed)_: Decodes ARM Cortex-M HardFault, MemManage, BusFault, UsageFault, and RISC-V mcause/mtval traps into a human-readable cause with the offending instruction, stack frame, and likely source line. AI proposes the root-cause class (null deref, stack overflow, unaligned access, MPU violation).

- ⬜ **SWO/ITM/RTT console** _(stubbed)_: Live capture of SWO, ITM, and Segger RTT channels with timestamping, channel routing, and binary frame decoding. Streams into the workbench log pipeline for AI search and summarization.

- ⬜ **Logic analyzer bridge** _(stubbed)_: Driver for sigrok, Saleae, and Kingst captures with protocol decoders for UART, SPI, I2C, CAN, LIN, 1-Wire, SDIO, I2S, and SWD. AI-integrated annotation of unknown bus traffic and anomaly flagging.

- ⬜ **Bus protocol sniffer** _(stubbed)_: Live decode and replay for CAN/CAN-FD, LIN, Modbus, RS-485, BLE HCI, Zigbee, Thread, and USB packets via attached interfaces. Saves sessions as searchable transcripts.

- ⬜ **Firmware image inspector** _(stubbed)_: Parses ELF, HEX, BIN, UF2, ESP image, and TI-TXT files with section maps, symbol tables, entropy plots, and signature/CRC validation. Diffs two images down to the symbol and basic block.

- ⬜ **Memory map and footprint analyzer** _(stubbed)_: Visualizes flash and RAM occupancy by section, object, and symbol with stack usage estimates from call graphs and -fstack-usage data. AI suggests size reductions and flags symbols that ballooned since the last build.

- ⬜ **Linker script editor** _(stubbed)_: Structured editor for GNU ld scripts and equivalents with region overlap detection, alignment checks, and overlay support. Visualizes the resulting memory map alongside the script.

- ⬜ **Startup and vector table generator** _(stubbed)_: Generates reset handlers, ISR vector tables, and C runtime init for ARM, RISC-V, AVR, and Xtensa targets. Cross-checks vector names against linker-referenced weak symbols.

- ⬜ **Bootloader builder** _(stubbed)_: Templates and customizes MCUboot, U-Boot SPL, Tinyboot, and vendor bootloaders with slot layout, signing, and rollback policy. Produces signed update payloads in step with the application build.

- ⬜ **OTA update designer** _(stubbed)_: Defines delta, A/B, and dual-bank update schemes with transport (BLE, Wi-Fi, LoRa, USB, UART) and rollback rules. Simulates power-loss at every stage and reports recoverability.

- ⬜ **Secure provisioning console** _(stubbed)_: Generates and injects device keys, certificates, and unique identifiers into OTP/eFuse with audit logging. Integrates with HSMs and PKCS#11 tokens; AI walks through fuse-once decisions before they are committed.

- ⬜ **RTOS inspector** _(stubbed)_: Thread, queue, semaphore, timer, and heap inspection for FreeRTOS, Zephyr, ThreadX, NuttX, RT-Thread, and Mbed OS via runtime tracing or live debugger. AI-integrated narration of why a task missed its deadline.

- ⬜ **Trace and profiler** _(stubbed)_: Captures ETM/ITM and Tracealyzer-style event streams with CPU load, ISR latency, and context-switch timelines. Highlights priority inversions and lock contention.

- ⬜ **Power profiler** _(stubbed)_: Streams from Nordic PPK2, Joulescope, Otii, and INA-based shunts to plot current versus firmware state with sleep-mode classification. AI-integrated correlation of current spikes to source lines via timestamped markers.

- ⬜ **Peripheral simulator** _(stubbed)_: Renode and QEMU-backed virtual board runner with scriptable peripheral models for sensors, displays, and radios. Lets firmware run on CI without hardware and replays captured bus traffic into the virtual peripherals.

- ⬜ **HIL test runner** _(stubbed)_: Drives hardware-in-the-loop test rigs (relays, programmable loads, signal generators, robotic actuators) against firmware builds with pass/fail reporting. Records analog and digital traces alongside test outcomes.

- ⬜ **HAL and driver generator** _(stubbed)_: Generates idiomatic drivers for sensors, displays, radios, and storage from datasheet excerpts or register tables in C, C++, Rust, and MicroPython. AI lifts initialization sequences and quirks from PDF datasheets into compilable stubs.

- ⬜ **Datasheet ingester** _(stubbed)_: Indexes vendor datasheets, reference manuals, errata, and app notes into a searchable corpus with page-anchored citations. AI answers register and timing questions with the exact page and figure as evidence.

- ⬜ **Errata tracker** _(stubbed)_: Maintains per-silicon-revision errata lists and cross-references them against the firmware's used peripherals and feature flags. Surfaces relevant errata on build and links to vendor workarounds.

- ⬜ **Fuzzer and property tester** _(stubbed)_: Drives libFuzzer, AFL++, and Hypothesis-style property tests against parsers, protocol stacks, and state machines built for host or QEMU. Reports crashes with minimized reproducers.

- ⬜ **Code-size optimizer** _(stubbed)_: Suggests link-time GC, LTO, function outlining, table consolidation, and printf-removal opportunities ranked by bytes saved. AI rewrites size-bloat hotspots and validates equivalence on a test suite.

- ⬜ **Symbol and call graph explorer** _(stubbed)_: Interactive call graph and reverse call graph from compiled artifacts with cycle and recursion detection. Useful for stack budgeting and dead-code hunting.

- ⬜ **Disassembler and binary diff** _(stubbed)_: Disassembles ARM, Thumb, RISC-V, AVR, Xtensa, and PIC binaries with cross-reference and string analysis. Diffs two builds at the instruction level and groups changes by function.

- ⬜ **Firmware unpacker** _(stubbed)_: Identifies and extracts vendor firmware blobs, encrypted update packages, FAT/LittleFS/SPIFFS images, and JFFS2 containers. AI guesses container formats from headers and entropy patterns.

- ⬜ **Bus traffic replayer** _(stubbed)_: Plays captured bus traces back onto live hardware or into the simulator with timing scaled and fields rewritten by rule. Useful for reproducing intermittent peripheral bugs.

- ⬜ **Sensor calibration suite** _(stubbed)_: Workflows for accelerometer, gyro, magnetometer, IMU fusion, ADC, thermistor, and load-cell calibration with curve fitting and coefficient export. AI proposes the model class from the residual shape.

- ⬜ **Display and UI prototyper** _(stubbed)_: WYSIWYG editor for LVGL, TouchGFX, Slint, and SDL-on-MCU UIs with simulated render on host. Exports C/Rust code and asset blobs sized to the target's flash budget.

- ⬜ **Font and asset packer** _(stubbed)_: Generates bitmap fonts, glyph subsets, image sprites, and audio clips compressed and aligned for embedded use. Reports per-asset cost in flash and RAM.

- ⬜ **DSP and filter designer** _(stubbed)_: Designs FIR, IIR, biquad, and adaptive filters with fixed-point quantization and CMSIS-DSP/ESP-DSP code emission. AI-integrated suggestion of filter topology from a captured noise profile.

- ⬜ **Edge ML deployer** _(stubbed)_: Converts and quantizes TensorFlow Lite Micro, ONNX, and Edge Impulse models for target MCUs with operator support checks and memory arena sizing. Benchmarks inference latency and energy on connected hardware.

- ⬜ **Radio link designer** _(stubbed)_: Link-budget calculator for BLE, Wi-Fi, LoRa, Sub-GHz, NB-IoT, and Thread with antenna gain, path loss, and regional duty-cycle rules. AI-integrated channel and SF/PHY suggestion from a deployment description.

- ⬜ **USB descriptor builder** _(stubbed)_: Composes USB device, configuration, interface, endpoint, HID, CDC, MSC, audio, and video descriptors with validation against the spec. Emits C structs and matching host-side test scripts.

- ⬜ **Filesystem and storage designer** _(stubbed)_: Layouts and inspectors for LittleFS, SPIFFS, FATFS, EEPROM emulation, and journaled flash regions with wear and endurance modeling. Generates mkfs payloads for first-boot provisioning.

- ⬜ **BSP and board generator** _(stubbed)_: Scaffolds Zephyr boards, ESP-IDF custom boards, Arduino variants, and Linux device trees from a pinmux and peripheral configuration. Keeps the board files in sync as the hardware spec evolves.

- ⬜ **Device tree editor** _(stubbed)_: Structured editor for Linux/Zephyr device trees with overlay support, binding validation, and live preview of the resolved tree. AI-integrated translation from natural-language hardware descriptions into DT fragments.

- ⬜ **Compliance and EMC prep** _(stubbed)_: Checklists and pre-scan helpers for FCC, CE, RED, and radio module certifications with test-mode firmware stubs and reporting templates. Tracks artifacts required for the submission package.

- ⬜ **Power budget estimator** _(stubbed)_: Spreadsheet-style model of duty cycles, sleep states, peripheral currents, and battery chemistry that outputs expected runtime and worst-case life. AI proposes firmware-side savings ranked by impact.

- ⬜ **Field telemetry browser** _(stubbed)_: Ingests device-emitted logs, crash dumps, and telemetry from fleets with symbolication against the matching firmware build. AI-integrated clustering of crashes and regression detection across rollouts.


#### Databases

- ⬜ **SQL console** _(stubbed)_: Multi-dialect SQL editor (Postgres, MySQL, SQLite, MSSQL, Oracle, DuckDB, ClickHouse) with autocomplete from live introspected schemas, parameter binding, result grid with pivot and chart preview, and per-tab transaction control.

- ⬜ **Connection manager** _(stubbed)_: Stores connection profiles with SSH tunneling, TLS, IAM/Kerberos auth, secrets-manager passthrough, and per-environment color coding to mark production read-only sessions.

- ⬜ **Schema browser** _(stubbed)_: Tree view of databases, schemas, tables, views, materialized views, functions, triggers, sequences, and types with inline DDL, dependency walking, and grep-across-schema search.

- ⬜ **ER diagram designer** _(stubbed)_: Visual entity-relationship editor with reverse engineering from live databases, forward DDL generation, foreign key inference, and round-trip sync between diagram and migration files.

- ⬜ **Migration tool** _(stubbed)_: Generates, applies, and rolls back versioned migrations across Alembic, Flyway, Liquibase, Prisma, and golang-migrate; diffs schema state against target and writes idempotent up/down scripts with AI-integrated drafting from natural-language change requests.

- ⬜ **Schema diff** _(stubbed)_: Compares two databases or a database against a schema dump and emits a minimal ALTER script, with destructive-change highlighting and dry-run impact estimate.

- ⬜ **Query builder** _(stubbed)_: Visual join and filter composer that emits dialect-correct SQL, supports CTEs, window functions, and lateral joins, and round-trips between graphical and text views.

- ⬜ **EXPLAIN visualizer** _(stubbed)_: Renders EXPLAIN ANALYZE plans as a node graph with cost, rows, time, and buffer hits per operator; flags seq scans on large tables, bad estimates, and missing indexes with AI-integrated rewrite suggestions.

- ⬜ **Index advisor** _(stubbed)_: Captures slow query logs or pg_stat_statements, proposes candidate indexes with hypothetical (HypoPG-style) testing, and reports redundant or unused indexes for removal.

- ⬜ **Data grid editor** _(stubbed)_: Spreadsheet-style row editor with inline foreign key navigation, JSON/JSONB drill-down, BLOB hex/image preview, multi-row paste, and pending-change review before commit.

- ⬜ **Import/Export hub** _(stubbed)_: Bulk loads and dumps CSV, TSV, Parquet, Avro, JSONL, Excel, and SQL dumps with column mapping, type coercion, error quarantine, and resumable streaming for billion-row tables.

- ⬜ **Backup and restore** _(stubbed)_: Schedules and runs pg_dump, mysqldump, mongodump, and physical base backups; verifies restore integrity to ephemeral instances and tracks PITR window and WAL/binlog retention.

- ⬜ **Replication monitor** _(stubbed)_: Displays primary/replica topology, lag in bytes and seconds, slot status, and binlog position; alerts on stalled replicas and supports failover dry-runs against test clusters.

- ⬜ **Redis client** _(stubbed)_: Connects to standalone, sentinel, and cluster Redis with key browser, TTL inspector, pub/sub viewer, stream consumer-group monitor, slowlog tail, and SCAN-based safe key search.

- ⬜ **MongoDB client** _(stubbed)_: Document browser with aggregation pipeline builder, explain plan viewer, index manager, change-streams tail, and schema sampler that infers field types and cardinality across a collection.

- ⬜ **Time-series explorer** _(stubbed)_: Queries InfluxDB, Prometheus, TimescaleDB, and VictoriaMetrics with PromQL/Flux/SQL editors, label autocomplete, downsampling preview, and side-by-side metric correlation plots.

- ⬜ **Graph DB browser** _(stubbed)_: Cypher and Gremlin console for Neo4j, Memgraph, and JanusGraph with force-directed graph rendering, path expansion on click, schema constraints viewer, and pattern-match templates.

- ⬜ **Vector DB inspector** _(stubbed)_: Browses FAISS, Qdrant, Milvus, Weaviate, Pinecone, and pgvector collections; runs k-NN queries with score breakdown, visualizes embeddings via UMAP/t-SNE, and reports recall against a labeled probe set.

- ⬜ **Search index manager** _(stubbed)_: Manages Elasticsearch, OpenSearch, and Meilisearch indices with analyzer testing, mapping editor, reindex orchestrator, and query DSL playground with relevance scoring breakdown.

- ⬜ **Query history and snippets** _(stubbed)_: Persistent local history of every executed query with full-text search, tag/star, parameter capture, shareable snippet library, and replay against a different connection.

- ⬜ **Stored procedure IDE** _(stubbed)_: Edits PL/pgSQL, T-SQL, PL/SQL, and MySQL routines with syntax-aware diff, dependency graph, step debugger where supported, and unit-test harness using transactional rollback.

- ⬜ **Data masking and anonymizer** _(stubbed)_: Generates sanitized dev/test datasets from production via deterministic hashing, format-preserving encryption, fake-data substitution (Faker), and referential-integrity-preserving subsetting.

- ⬜ **Row-level lineage** _(stubbed)_: Traces a value across joins and views to its source tables and columns, parses CREATE VIEW and CTAS definitions, and renders column-level lineage graphs for impact analysis.

- ⬜ **Audit and query log viewer** _(stubbed)_: Ingests pgaudit, MySQL general log, and MongoDB profiler output with per-user, per-table activity timelines, anomaly flagging on novel access patterns, and exportable compliance reports.

- ⬜ **Performance dashboard** _(stubbed)_: Live charts of QPS, latency percentiles, buffer cache hit ratio, lock waits, deadlocks, vacuum progress, autovacuum bloat, and connection pool saturation across configured instances.

- ⬜ **Lock and deadlock inspector** _(stubbed)_: Shows current locks, blocked-by chains, and deadlock graph; offers safe-kill of blocking sessions and replays deadlock traces from logs to reproduce contention.

- ⬜ **Sharding and partition planner** _(stubbed)_: Analyzes table size and access skew, recommends partition keys (range, list, hash) and shard layouts for Citus/Vitess, and previews per-shard query routing.

- ⬜ **CDC stream viewer** _(stubbed)_: Taps Debezium, Postgres logical decoding, MySQL binlog, and MongoDB oplog streams; renders change events as a tail with filtering, schema-evolution tracking, and replay-to-sink testing.

- ⬜ **Embedded SQLite/DuckDB workbench** _(stubbed)_: Opens local .db, .sqlite, and .duckdb files with attached-database joins across files, Parquet/CSV virtual tables, and one-click export to a hosted database for promotion.

- ⬜ **NL-to-SQL** _(stubbed)_: AI-integrated translator from plain-English questions to SQL grounded in the live schema, sample rows, and prior query history; shows generated SQL with EXPLAIN and required permissions before run.

- ⬜ **Seed data generator** _(stubbed)_: Produces realistic synthetic rows respecting foreign keys, unique constraints, and check constraints; learns distributions from a sample of production data when permitted.

- ⬜ **Cost and storage profiler** _(stubbed)_: Reports per-table and per-index size, toast and bloat estimates, hot/cold row ratios, and projected monthly cost on cloud providers (RDS, Aurora, Cloud SQL, Atlas) given current growth.


#### General computing and sysadmin

- ⬜ **Process explorer** _(stubbed)_: Live tree of running processes with CPU, memory, IO, file descriptor, and thread counts. Supports kill, renice, signal, attach-strace, and pinning to per-process resource graphs. AI-integrated annotation of unfamiliar processes from binary path and cmdline.

- ⬜ **Resource monitor** _(stubbed)_: Real-time and historical graphs of CPU, memory, swap, disk IO, network throughput, and load average, sampled per-core and per-device. Exports to CSV and links each spike to the processes running at that timestamp.

- ⬜ **Systemd unit manager** _(stubbed)_: Browse, start, stop, enable, disable, mask, and edit systemd units and their drop-ins, with dependency graph and timer view. AI-integrated drafting of unit files from a plain-language service description.

- ⬜ **Journal browser** _(stubbed)_: Filterable view of the systemd journal with boot, unit, priority, and time-range facets. AI-integrated clustering of repeated error patterns and summarization of long failure runs into a single explanation.

- ⬜ **Log aggregator** _(stubbed)_: Tails arbitrary log files and journald streams into one searchable timeline with regex, severity, and source filters. AI-integrated anomaly highlighting against a learned baseline of normal log volume and content.

- ⬜ **Cron and timer editor** _(stubbed)_: Unified editor for user crontabs, /etc/cron.d, anacron, and systemd timers, with next-fire-time preview and dry-run execution. AI-integrated translation between natural-language schedules and cron or OnCalendar expressions.

- ⬜ **Disk usage analyzer** _(stubbed)_: Hierarchical treemap and sunburst of filesystem usage with per-directory size, file count, age, and largest-file lists. Identifies duplicate files by hash and offers in-place deletion or archive routing.

- ⬜ **SMART and disk health** _(stubbed)_: Reads SMART attributes, runs short and long self-tests, and tracks attribute trends per device over time. AI-integrated failure-risk scoring from attribute drift, with replacement urgency suggestions.

- ⬜ **Filesystem and mount manager** _(stubbed)_: Lists block devices, partitions, LVM volumes, and mount points; performs mount, unmount, fstab edits, and filesystem creation. Includes BTRFS and ZFS subvolume and snapshot management.

- ⬜ **Backup scheduler** _(stubbed)_: Configures restic, borg, or rsync jobs with retention policies, encryption keys, and target repositories (local, SFTP, S3). Runs scheduled and ad-hoc backups, verifies snapshots, and offers point-in-time restore browsing.

- ⬜ **Snapshot manager** _(stubbed)_: Creates, lists, diffs, and rolls back BTRFS, ZFS, and LVM snapshots. Diff view shows file-level changes between any two snapshots and supports per-file restore without full rollback.

- ⬜ **Container manager** _(stubbed)_: Manages Docker and Podman containers, images, volumes, and networks, with logs, exec, stats, and inspect panels. AI-integrated explanation of layer bloat and Dockerfile cleanup suggestions from a built image.

- ⬜ **Compose stack runner** _(stubbed)_: Edits and runs docker-compose and podman-compose files with dependency-aware up, down, and rebuild actions. Visualizes service graph, port bindings, and volume mounts.

- ⬜ **VM manager** _(stubbed)_: libvirt and QEMU frontend for creating, cloning, snapshotting, and consoling into virtual machines. Manages virtual networks, storage pools, and PCI passthrough; embeds a SPICE/VNC console.

- ⬜ **Network interface configurator** _(stubbed)_: Lists physical and virtual interfaces with addresses, routes, MTU, and link state. Edits NetworkManager, systemd-networkd, or netplan configs and applies changes with rollback on connectivity loss.

- ⬜ **Packet inspector** _(stubbed)_: Live tcpdump and pcap capture per interface with BPF filter, decoded protocol layers, and follow-stream view. AI-integrated explanation of unfamiliar protocols and flagging of plaintext credentials or odd handshakes.

- ⬜ **Port and connection viewer** _(stubbed)_: Shows listening sockets, established connections, and per-process socket ownership with reverse-DNS and geo annotation. Includes scan-from-here actions against a target host with nmap-style probes.

- ⬜ **Firewall rule editor** _(stubbed)_: Unified editor for nftables, iptables, ufw, and firewalld rule sets with live counters and packet trace. AI-integrated translation between human intent ("block all inbound except SSH from LAN") and concrete ruleset diffs.

- ⬜ **DNS diagnostic console** _(stubbed)_: Runs dig, drill, and resolved queries against arbitrary nameservers with DNSSEC validation, trace, and EDNS options. Caches results and diffs zone-file changes across resolvers.

- ⬜ **SSH and remote host manager** _(stubbed)_: Catalog of SSH hosts with known_hosts, identity file, jump host, and forwarding configuration per entry. Opens terminal, SFTP, or port-forward sessions and writes back to ~/.ssh/config.

- ⬜ **VPN and tunnel manager** _(stubbed)_: Configures and toggles WireGuard, OpenVPN, and SSH tunnels with key generation, peer config export, and QR codes. Shows per-peer handshake age, transfer counters, and allowed-IP routing.

- ⬜ **Reverse proxy editor** _(stubbed)_: Generates and lints nginx, Caddy, and Traefik configurations for virtual hosts, TLS termination, and upstream routing. AI-integrated synthesis of a working config from a list of services and domains.

- ⬜ **Certificate manager** _(stubbed)_: Tracks local x509 certificates and ACME-issued certs with expiry calendar, SAN list, and chain validation. Drives certbot, acme.sh, or step-ca to issue, renew, and deploy certs to configured services.

- ⬜ **Secrets vault** _(stubbed)_: Local encrypted store for API keys, SSH keys, passwords, and tokens, with per-entry rotation reminders and clipboard auto-clear. Exposes entries to other workbench tools through a scoped reference rather than raw value.

- ⬜ **Package manager frontend** _(stubbed)_: Unified view over apt, dnf, pacman, nix, flatpak, and snap with installed lists, upgrade preview, orphan detection, and pinning. Shows full transaction logs and supports rollback where the backend allows it.

- ⬜ **Hardware inventory** _(stubbed)_: Aggregates lshw, lspci, lsusb, dmidecode, and sensors output into a browsable hardware tree with firmware versions, temperatures, and fan speeds. Highlights drivers in use and lists kernel modules per device.

- ⬜ **Performance profiler** _(stubbed)_: Wraps perf, eBPF, and bpftrace to record on-CPU and off-CPU profiles and render flamegraphs and latency histograms. AI-integrated narration of hot stacks and likely bottleneck classifications.

- ⬜ **Syscall and IO tracer** _(stubbed)_: Attaches strace, ltrace, or bpftrace probes to a running process and streams decoded syscalls, opened files, and network IO. Filters by syscall family and replays captured traces offline.

- ⬜ **Config diff and version tracker** _(stubbed)_: Watches /etc and other configured directories, snapshots changes to a local git repo, and shows per-file history with author and timestamp. AI-integrated commit messages describing what the change does.

- ⬜ **User and permission editor** _(stubbed)_: Manages local users, groups, sudoers entries, ACLs, and POSIX capabilities with a permission-impact preview before applying. Includes a recursive permission auditor for a given path.


#### Game development

- ⬜ **Sprite Editor** _(stubbed)_: Pixel-art canvas with layers, frame-by-frame animation, onion skinning, indexed palettes, and per-frame pivot points. Exports to spritesheets, GIF, APNG, or per-frame PNG with a sidecar atlas JSON.

- ⬜ **Tilemap Composer** _(stubbed)_: Multi-layer 2D tilemap editor with autotiling rules (Wang/blob/47-tile), stamp brushes, randomized variants, and chunked streaming. AI-integrated rule inference from a small hand-painted example region.

- ⬜ **Particle Designer** _(stubbed)_: Visual emitter editor with curve-driven lifetime properties, sub-emitters, force fields, GPU and CPU backends, and live preview against a chosen game background. Exports to a portable JSON spec consumable by common engines.

- ⬜ **Level Layout Studio** _(stubbed)_: Grid-and-free 2D/2.5D level editor with entities, prefab instances, rooms, doors, spawners, and runtime-tunable parameters. Supports tile-collision baking and navmesh generation in the same view.

- ⬜ **Dialog Scripter** _(stubbed)_: Node-graph branching dialog editor with conditions, variables, speakers, voice-line slots, and inline localization. Compiles to Yarn, Ink, or a portable JSON runtime and round-trips with plain-text source.

- ⬜ **Behavior Tree Authoring** _(stubbed)_: Visual editor for behavior trees with selectors, sequences, decorators, services, blackboard inspection, and step-debugging against a live game instance. Includes a library of reusable subtrees and unit tests per node.

- ⬜ **State Machine Editor** _(stubbed)_: Hierarchical FSM designer with transitions, guards, entry/exit actions, parallel regions, and statechart export. Live replay shows the active state path against recorded gameplay traces.

- ⬜ **Sound Effect Forge** _(stubbed)_: Sfxr/Bfxr-style procedural SFX synthesizer with parameter randomization, mutation history, and project-wide variation banks. Includes a denoiser, normalizer, and game-engine-ready WAV/OGG export.

- ⬜ **Tracker Studio** _(stubbed)_: Pattern-based music tracker with chiptune and sample instruments, MIDI import, looped section markers, and interactive music states. Renders deterministic stems for layering by gameplay events.

- ⬜ **Shader Playground** _(stubbed)_: Live GLSL/WGSL/HLSL editor with hot-reload, uniform bindings, texture inputs, and a node-graph mode that compiles to the same source. Includes side-by-side diffing for portability across targets.

- ⬜ **Animation Curve Editor** _(stubbed)_: Tweening and easing curve designer with bezier handles, presets, oscillation, and curve libraries shared across sprites, UI, and gameplay code. Exports to engine-native curve formats and a portable JSON.

- ⬜ **Skeletal Rigger** _(stubbed)_: 2D skeletal animation tool with bones, IK chains, mesh deformation, skinning weights, and per-slot attachments. Compatible with Spine and DragonBones JSON for engine portability.

- ⬜ **Asset Pipeline Manager** _(stubbed)_: Declarative import pipeline that watches source folders, runs reproducible transforms (atlas pack, compress, mip, ETC/BCn), and writes lockfiles. Caches by content hash and surfaces diffs when an asset rebuilds.

- ⬜ **Sprite Atlas Packer** _(stubbed)_: Multi-bin texture packer with rotation, trim, padding, POT/NPOT modes, and stable IDs across rebuilds for diff-friendly atlases. Generates engine-specific metadata (Unity, Godot, LOVE, raw JSON).

- ⬜ **Palette Workshop** _(stubbed)_: Color ramp and indexed palette designer with LCh-aware interpolation, dithering preview, palette extraction from reference images, and palette-swap testing across all sprites at once.

- ⬜ **Save Format Editor** _(stubbed)_: Structured editor for save files with schema inference, hex/text dual view, checksum repair, and field-level diffing between saves. Useful for testing migrations, debugging corruption, and authoring fixture saves.

- ⬜ **Procedural Generation Sandbox** _(stubbed)_: Live laboratory for noise, cellular automata, L-systems, wave function collapse, and graph rewriting, with seedable runs and parameter sweeps. AI-integrated suggestion of constraints from a few hand-drawn example outputs.

- ⬜ **Navmesh & Pathfinding Lab** _(stubbed)_: Bakes navmeshes from level geometry, visualizes A*/JPS/HPA* searches step-by-step, and stress-tests with crowds of agents. Lets you tweak heuristics and agent radii with instant rebake.

- ⬜ **Collision Shape Editor** _(stubbed)_: Per-sprite and per-mesh collider authoring with polygon, capsule, and compound shapes, plus auto-fit from alpha or mesh silhouettes. Previews collisions against test bodies with adjustable physics parameters.

- ⬜ **Localization Manager** _(stubbed)_: String table editor with keys, contexts, plural forms, gender, ICU MessageFormat, and screenshot attachments per key. Detects untranslated, stale, and overlong strings, and round-trips with PO/CSV/XLIFF.

- ⬜ **Item & Stat Database** _(stubbed)_: Spreadsheet-style editor for items, enemies, weapons, and stat curves with formula columns, referential integrity, and what-if balance simulations. Exports to typed code or JSON and diffs cleanly in version control.

- ⬜ **Input Mapping Designer** _(stubbed)_: Action-based input mapper covering keyboard, mouse, gamepad, and touch, with rebinding UI generation, accessibility profiles, and conflict detection. Records and replays input traces for deterministic test runs.

- ⬜ **UI/HUD Layout Editor** _(stubbed)_: Anchor-and-flex UI authoring with responsive breakpoints, theming, focus order, controller navigation, and live data bindings to game variables. Exports to engine UI formats or a portable retained-mode JSON.

- ⬜ **Bitmap Font Forge** _(stubbed)_: Generates SDF, MSDF, and classic bitmap fonts from TTF/OTF sources with kerning, custom glyph painting, and CJK subsetting. Packs into the atlas pipeline and previews against in-game UI styles.

- ⬜ **Replay Recorder** _(stubbed)_: Captures deterministic input traces and engine snapshots, scrubbable like a video, with bookmarks, branch points, and side-by-side comparison. Doubles as a regression harness for gameplay bugs.


#### VR / AR / XR

- ⬜ **Scene Composer** _(stubbed)_: Local 3D scene builder with XR rig presets (HMD, controllers, hands, room bounds), glTF/USD/FBX import, and snap-grid layout. AI-integrated layout suggestion from a textual brief or reference image plus declared play-area dimensions.

- ⬜ **Tracking Pose Inspector** _(stubbed)_: Live 6DoF pose stream viewer for HMD, controllers, hand joints, and body trackers over OpenXR / WebXR / OSC, with frame-by-frame jitter, drift, and dropout plots. Records sessions to replay against later builds.

- ⬜ **Spatial Audio Mixer** _(stubbed)_: Ambisonic (1st to 3rd order) and HRTF binaural mixer with per-source distance, occlusion, and reverb-zone controls. Includes head-locked rotation preview and A/B against a standard HRTF set.

- ⬜ **World Anchor Workbench** _(stubbed)_: Place, name, and persist spatial anchors against a captured room mesh; export to ARKit, ARCore, OpenXR Spatial Anchors, and a portable JSON format. Visualizes anchor confidence and drift over relocalization passes.

- ⬜ **Comfort Analyzer** _(stubbed)_: Motion-sickness predictor that scores a recorded locomotion clip on optical flow magnitude, vection, unexpected yaw/pitch, frame-time stability, and acceleration profile. Flags offending segments on a timeline with suggested fixes (vignette, snap-turn, reduced FOV).

- ⬜ **360 Video Studio** _(stubbed)_: Viewer and light editor for equirectangular, cubemap, and VR180 footage with stereo pair alignment, seam blending, stabilization, and overlay tracks (subtitles, hotspots). Exports to MP4, MKV, and a WebXR-ready package.

- ⬜ **Hand Pose Authoring** _(stubbed)_: Design and record per-finger grip, pinch, point, and ASL-style poses against a skeletal rig, with blend trees between poses and a recognizer that fires events on match. Supports OpenXR hand-tracking joint conventions.

- ⬜ **Stereo View Inspector** _(stubbed)_: Side-by-side per-eye render preview with adjustable IPD, vergence, and zero-parallax distance; highlights pixels with excessive stereo disparity that cause eye strain. Useful for tuning UI placement and depth budgets.

- ⬜ **Lens Distortion Profiler** _(stubbed)_: Generates and previews barrel / pincushion mesh warps and chromatic aberration correction curves for a given lens profile, with a calibration-grid loopback. Imports common headset distortion meshes and exports as shader LUTs.

- ⬜ **Locomotion Lab** _(stubbed)_: Sandbox for trying teleport, snap-turn, smooth, arm-swing, and vehicle locomotion against a fixed test course, with telemetry on time-to-target, off-path deviation, and comfort score from the analyzer.

- ⬜ **Passthrough Compositor** _(stubbed)_: Mixed-reality compositing preview that layers virtual content over a recorded passthrough RGB / depth stream, with occlusion mask debugging, depth-test toggles, and lighting estimation overlays.

- ⬜ **Photogrammetry and Splat Importer** _(stubbed)_: Loads NeRF, 3D Gaussian Splatting, and classic photogrammetry meshes; decimates, recenters, and rebakes them into XR-ready assets with collision proxies. AI-integrated cleanup of floaters and background bleed.

- ⬜ **Haptic Pattern Designer** _(stubbed)_: Timeline editor for controller and glove haptics: amplitude, frequency, and waveform per actuator, with crossfade and event-driven triggers. Exports to common runtime formats (OpenXR haptic, bHaptics, Interhaptics-compatible JSON).

- ⬜ **Foveated Render Budget** _(stubbed)_: Per-frame GPU heatmap that breaks down cost by region (foveal, mid, peripheral) and shows headroom against the headset's target frame time. Suggests fixed and eye-tracked foveation parameters that hit the budget.

- ⬜ **Gaze Heatmap Studio** _(stubbed)_: Aggregates eye-tracking sessions into fixation maps, scanpaths, and dwell heatmaps over the rendered scene. Useful for UI placement, attention studies, and validating foveated render falloff.

- ⬜ **Avatar IK Tuner** _(stubbed)_: Full-body and hand IK solver playground (FABRIK, two-bone, Final-IK style) driven by HMD plus two controllers, with shoulder hint constraints, elbow popping diagnostics, and finger curl retarget. Previews on a swappable humanoid rig.

- ⬜ **Mesh Reconstruction Viewer** _(stubbed)_: Inspector for room-scale scene meshes from Quest, HoloLens, ARKit, and LIDAR scans, with per-face confidence shading, hole-fill preview, and semantic labels (floor, wall, ceiling, furniture).

- ⬜ **WebXR Live Preview** _(stubbed)_: Spins up a local WebXR session of the current scene over HTTPS / ADB reverse, with hot-reload of glTF assets and a desktop emulated controller for non-headset iteration. Logs runtime feature support per device.

- ⬜ **Frustum and FOV Visualizer** _(stubbed)_: Third-person view of headset and controller frustums, render bounds, and guardian / chaperone polygons, with measurements of effective horizontal / vertical FOV per eye and stereo overlap region.

- ⬜ **XR Input Recorder** _(stubbed)_: Captures full OpenXR input state (poses, buttons, axes, haptic outputs, hand joints, eye gaze) into a deterministic timeline that can be replayed into the runtime for regression testing locomotion, UI, and gesture code.


#### Quantum computing

- ⬜ **Circuit Builder** _(stubbed)_: Drag and drop quantum circuit editor with a categorized gate palette (Clifford, rotation, multi-qubit, custom unitaries), per-wire annotations, classical registers, and live Bloch and statevector previews of each wire's local state.

- ⬜ **Bloch Sphere Visualizer** _(stubbed)_: Interactive Bloch sphere for single qubits with reduced-density rendering for multi-qubit registers, vector trails for time evolution, and a scrubbable timeline tied to the circuit timestep.

- ⬜ **State Inspector** _(stubbed)_: Statevector, density matrix, and probability distribution views with amplitude bar charts, phase wheels, computational basis filtering, and hover-driven readout of complex amplitudes, fidelity, and purity.

- ⬜ **Statevector Simulator** _(stubbed)_: Exact dense simulator up to roughly 25 qubits with parameter binding, conditional ops, deferred measurement, and caching of intermediate states for fast scrubbing inside the builder.

- ⬜ **Stabilizer Simulator** _(stubbed)_: Clifford-only simulator running thousands of qubits via tableau or graph-state representation, with magic-state injection support so non-Clifford gates fall back to a sliced full simulation.

- ⬜ **Tensor Network Simulator** _(stubbed)_: MPS and PEPS based simulator with adjustable bond dimension, contraction order optimization, and entanglement-entropy tracking for circuits too wide for statevector methods.

- ⬜ **Noise Model Designer** _(stubbed)_: GUI for building Kraus channels, depolarizing, amplitude/phase damping, Pauli, thermal, and crosstalk errors per-gate and per-qubit, with import of T1/T2/readout-error data from device backends.

- ⬜ **Pulse Schedule Editor** _(stubbed)_: OpenPulse-style waveform editor with Gaussian, DRAG, and arbitrary envelopes, frame/phase tracking, calibrated single-qubit and two-qubit primitives, and an instruction schedule timeline aligned to the gate-level circuit.

- ⬜ **Quantum Notebook** _(stubbed)_: Python notebook accepting Qiskit and Cirq style syntax with inline circuit rendering, live state plots, and AI-integrated translation between framework dialects and natural-language prompts for circuit construction.

- ⬜ **Transpiler** _(stubbed)_: Multi-pass compiler targeting selectable native gate sets (CX/CZ, iSWAP, Mølmer-Sørensen) with KAK decomposition, commutation-based optimization, Solovay-Kitaev, and side-by-side depth and gate-count diffs of each pass.

- ⬜ **Qubit Layout and Router** _(stubbed)_: Maps logical qubits to a chosen device topology (heavy-hex, square lattice, linear ion chain) with SABRE-style routing, swap insertion preview, and AI-integrated suggestion of initial layouts from circuit interaction graphs.

- ⬜ **Algorithm Scaffolds** _(stubbed)_: Parameterized templates for QFT, Quantum Phase Estimation, Grover, Shor, HHL, Bernstein-Vazirani, Simon, Deutsch-Jozsa, and amplitude estimation, instantiable with chosen problem size and oracle wiring.

- ⬜ **Hamiltonian Builder** _(stubbed)_: Editor for Pauli-string Hamiltonians and fermionic operators with Jordan-Wigner, Parity, and Bravyi-Kitaev mappings, second-quantized molecular input via PySCF, and Trotter/Suzuki decomposition into circuits.

- ⬜ **Variational Solver Lab** _(stubbed)_: VQE and QAOA workbench with ansatz library (hardware-efficient, UCCSD, QAOA-p), pluggable classical optimizers (COBYLA, SPSA, Adam), gradient methods (parameter shift, SPSA), and live energy/landscape plots.

- ⬜ **QEC Workbench** _(stubbed)_: Build and simulate stabilizer codes (repetition, Steane, surface, color), inject Pauli errors, run minimum-weight perfect matching and union-find decoders, and plot logical error rate versus physical noise.

- ⬜ **Tomography Suite** _(stubbed)_: State, process, and gate-set tomography with MLE and linear inversion reconstructions, randomized benchmarking and direct fidelity estimation protocols, and confidence intervals from bootstrap resampling.

- ⬜ **Entanglement Inspector** _(stubbed)_: Computes and plots entanglement entropy, concurrence, negativity, and mutual information across bipartitions, with a graph view of pairwise entanglement and Schmidt decomposition of any cut.

- ⬜ **Resource Estimator** _(stubbed)_: Reports qubit count, circuit depth, T-count, Toffoli-count, two-qubit-gate count, and projected wall-clock runtime on selected devices, including fault-tolerant overhead estimates for a chosen code distance.

- ⬜ **Circuit Equivalence Checker** _(stubbed)_: Verifies two circuits implement the same unitary (or channel) up to global phase via ZX-calculus rewriting and Clifford-tableau methods, with a minimal counterexample basis state when they differ.

- ⬜ **QASM and Format Bridge** _(stubbed)_: Round-trip import and export of OpenQASM 2/3, Quil, Cirq JSON, Qiskit QPY, and PennyLane tape, with a textual editor that stays in sync with the visual builder.


#### Cybersecurity and digital forensics

- ⬜ **Hash Workbench** _(stubbed)_: Computes MD5, SHA-1, SHA-256, SHA-512, BLAKE3, CRC32, ssdeep, and TLSH for files, text, or byte ranges. Supports batch verification against checksum manifests and similarity matching across known-bad corpora.

- ⬜ **PCAP Analyzer** _(stubbed)_: Parses pcap/pcapng captures with per-packet decoding, flow reconstruction, and protocol stats. Provides display filters, follow-stream views, TLS metadata extraction, and AI-assisted explanation of suspicious conversations.

- ⬜ **Vulnerability Scanner Driver** _(stubbed)_: Drives Nmap NSE, Nuclei templates, and OpenVAS-style checks against targets with a unified result schema. Deduplicates findings, ranks by exploitability, and links each finding to CVE/CWE references.

- ⬜ **Malware Sandbox Viewer** _(stubbed)_: Renders artifact reports from local sandbox runs (Cuckoo/CAPE/Detonate) with process tree, registry deltas, network IOCs, dropped files, and screenshots. Cross-references behaviors with MITRE ATT&CK techniques.

- ⬜ **File Carver** _(stubbed)_: Scans raw disks, images, or memory dumps for file signatures and reconstructs allocated and unallocated fragments. Supports custom magic definitions, slack-space recovery, and entropy-aware boundary detection.

- ⬜ **Log Analyzer** _(stubbed)_: Ingests syslog, Windows EVTX, JSON, and arbitrary text logs into a queryable timeline with field extraction and pivot tables. AI-integrated anomaly highlighting and natural-language search over the parsed corpus.

- ⬜ **Memory Forensics Explorer** _(stubbed)_: Drives Volatility/Rekall-style plugins over RAM captures to enumerate processes, network sockets, injected code, handles, and registry hives. Surfaces hidden processes and DLL hollowing indicators.

- ⬜ **Disk Image Browser** _(stubbed)_: Mounts and walks raw, E01, AFF4, and VMDK images read-only with NTFS, ext4, APFS, HFS+, and FAT support. Shows MFT entries, journals, deleted files, alternate data streams, and timestamp anomalies.

- ⬜ **YARA Rule Builder** _(stubbed)_: Authors, tests, and tunes YARA rules against sample corpora with auto-generated atom suggestions and false-positive scoring. AI-integrated drafting of starter rules from a labeled sample plus negative set.

- ⬜ **Disassembler/Decompiler Viewer** _(stubbed)_: Loads PE, ELF, Mach-O, and raw binaries with graph and linear views, symbol resolution, and decompiled C output via Ghidra/Radare backends. Pivots between cross-references, strings, and import tables.

- ⬜ **Entropy Mapper** _(stubbed)_: Plots byte entropy across files and disk regions to surface packed, encrypted, or compressed sections. Overlays section boundaries, signature hits, and suspected key material for triage.

- ⬜ **Strings Triage** _(stubbed)_: Extracts ASCII, UTF-8, UTF-16, and stack-string candidates from binaries and dumps, classifying them into URLs, IPs, paths, registry keys, and credentials. AI-integrated clustering groups likely indicators.

- ⬜ **Certificate and Key Inspector** _(stubbed)_: Parses X.509 certificates, CSRs, PEM/DER/PKCS#12 bundles, SSH keys, and JWKs with chain validation, revocation checks, and key strength analysis. Flags weak curves, short moduli, and pinning mismatches.

- ⬜ **Crypto Playground** _(stubbed)_: Interactive encode/decode and encrypt/decrypt across base64/32/85, hex, URL, AES/ChaCha modes, RSA, ECC, HMAC, KDFs, and JWT. Chainable recipe pipeline in the style of CyberChef with local-only execution.

- ⬜ **Password Recovery Driver** _(stubbed)_: Frontend for hashcat and John the Ripper covering hash identification, wordlist/rule selection, mask attacks, and GPU job control. Tracks attack progress, ETA, and cracked artifacts across sessions.

- ⬜ **Network Topology Mapper** _(stubbed)_: Builds a host/service graph from scans, ARP tables, routing data, and passive capture. Highlights pivot paths, exposed admin services, and changes versus prior snapshots.

- ⬜ **OSINT Aggregator** _(stubbed)_: Runs WHOIS, DNS, reverse DNS, passive DNS, ASN, certificate transparency, and threat-feed lookups in parallel for a target. Caches results offline and deduplicates indicators across sources.

- ⬜ **Firmware and Binary Walker** _(stubbed)_: Performs binwalk-style recursive extraction of firmware images, archives, filesystems, and bootloaders. Detects embedded keys, hardcoded credentials, and known-vulnerable component versions.

- ⬜ **Timeline Builder** _(stubbed)_: Fuses filesystem MAC times, registry, browser history, log events, and prefetch into a super-timeline with filtering, tagging, and narrative annotation. AI-integrated reconstruction of likely event sequences from selected rows.

- ⬜ **Steganography and Carrier Scanner** _(stubbed)_: Detects LSB, DCT, palette, and append-after-EOF stego across images, audio, and PDFs with payload extraction and known-tool fingerprinting. Reports suspicion scores per carrier.

- ⬜ **IOC Extractor and Normalizer** _(stubbed)_: Pulls indicators (IPs, domains, hashes, URLs, CVEs, mutexes, registry keys, Bitcoin addresses) from arbitrary text, PDFs, and reports. Defangs/refangs, deduplicates, and exports STIX, MISP, and CSV.


#### Cryptography and blockchain

- ⬜ **Key Forge** _(stubbed)_: Generates RSA, ECDSA, Ed25519, X25519, and post-quantum (ML-KEM, ML-DSA) keypairs with configurable parameters. Exports to PEM, DER, JWK, OpenSSH, and PGP formats with passphrase-protected variants.

- ⬜ **Key Inspector** _(stubbed)_: Parses any key blob (PEM, DER, JWK, SSH, PGP, BIP32 xprv) and surfaces algorithm, curve, key size, fingerprints, and entropy estimates. Detects weak primes, low-entropy generation, and known compromised keys.

- ⬜ **Signature Workbench** _(stubbed)_: Signs and verifies messages across ECDSA, EdDSA, RSA-PSS, RSA-PKCS1, BLS, and Schnorr with selectable hash functions. Supports detached signatures, COSE, JWS, and raw r,s component editing for malleability experiments.

- ⬜ **Cipher Playground** _(stubbed)_: Encrypts and decrypts payloads with AES (GCM, CBC, CTR, XTS), ChaCha20-Poly1305, AES-SIV, and legacy ciphers for educational use. Visualizes block-by-block transformations, IV/nonce reuse failures, and padding oracle behavior.

- ⬜ **Hash Lab** _(stubbed)_: Computes SHA-2, SHA-3, BLAKE2/3, Keccak, RIPEMD, and Argon2/scrypt/bcrypt digests with side-by-side comparison. Includes length-extension attack simulator and collision search against truncated outputs.

- ⬜ **KDF Tuner** _(stubbed)_: Benchmarks Argon2id, scrypt, PBKDF2, and HKDF parameters against a target derivation time on the local hardware. AI-integrated recommendation of parameters given a threat model and latency budget.

- ⬜ **Certificate Inspector** _(stubbed)_: Parses X.509 certs, CSRs, CRLs, and full chains, validating signatures, hostname matching, key usage, SAN entries, and CT log inclusion. Renders the trust path against system and custom root stores and flags policy violations.

- ⬜ **TLS Probe** _(stubbed)_: Connects to a host, enumerates offered ciphersuites, ALPN, OCSP stapling, session resumption, and post-quantum hybrid support. Highlights deprecated suites, weak DH parameters, and certificate transparency gaps.

- ⬜ **PGP/GPG Console** _(stubbed)_: Imports keyrings, signs, encrypts, decrypts, and verifies armored or binary OpenPGP messages. Visualizes web-of-trust signatures, subkey relationships, and revocation status without shelling out to gpg.

- ⬜ **SSH Toolkit** _(stubbed)_: Generates, audits, and converts SSH keys, parses known_hosts and authorized_keys, and verifies host key pinning. Includes SSH certificate authority signing with principals, validity windows, and force-command extensions.

- ⬜ **Secret Sharing Bench** _(stubbed)_: Splits secrets via Shamir's Secret Sharing and verifiable variants (Feldman, Pedersen) with configurable threshold and field. Reassembles shares from QR, mnemonics, or hex with corruption detection.

- ⬜ **Mnemonic & HD Wallet Studio** _(stubbed)_: Generates and parses BIP39/SLIP39 mnemonics, derives BIP32/BIP44/BIP84/BIP86 paths across coins, and exports xpub/xprv. Validates passphrase variants, surfaces address gaps, and detects path collisions across wallets.

- ⬜ **Address Toolkit** _(stubbed)_: Encodes and decodes Bitcoin (P2PKH, P2SH, bech32, taproot), Ethereum (EIP-55, ENS), Solana, Cosmos bech32, and Monero address formats. Validates checksums, reverses to underlying script or public key, and flags vanity patterns.

- ⬜ **Blockchain Explorer** _(stubbed)_: Queries Bitcoin, Ethereum, L2 rollups, Solana, and Cosmos chains via local node RPC or vendored archival mirrors. Renders blocks, transactions, mempool state, fee histograms, and reorg history with offline-capable caches.

- ⬜ **Transaction Builder** _(stubbed)_: Constructs raw transactions for Bitcoin (PSBT), Ethereum (EIP-1559, EIP-4844 blob), Solana, and Cosmos with manual fee, nonce, and signature control. Simulates execution against a forked state before broadcast.

- ⬜ **Smart Contract Drafter** _(stubbed)_: Scaffolds Solidity, Vyper, and Move contracts from a natural-language spec with AI-integrated generation of standard patterns (ERC-20, ERC-721, ERC-4626, multisig, timelock). Compiles in-browser and produces deployment scripts and verification metadata.

- ⬜ **Contract Analyzer** _(stubbed)_: Decompiles EVM bytecode, recovers function signatures via 4byte lookup, and runs static checks for reentrancy, integer overflow, tx.origin misuse, and unchecked external calls. AI-integrated explanation of detected patterns with severity scoring.

- ⬜ **EVM Debugger** _(stubbed)_: Steps through transaction execution opcode by opcode against a local fork, with stack, memory, storage, and gas at each step. Supports source-mapped Solidity stepping and time-travel reverse execution.

- ⬜ **ABI & Calldata Decoder** _(stubbed)_: Decodes raw calldata, event logs, and revert reasons against known ABIs or guessed selectors. Re-encodes edited parameters and supports nested structs, dynamic arrays, and EIP-712 typed data.

- ⬜ **ZK Circuit Playground** _(stubbed)_: Authors and proves Circom, Halo2, and Noir circuits with witness inspection and constraint visualization. Generates Groth16/PLONK proofs locally and exports Solidity verifier contracts.

- ⬜ **Merkle & Accumulator Builder** _(stubbed)_: Constructs Merkle trees, Merkle Patricia tries, Verkle trees, and RSA accumulators from arbitrary leaf sets. Generates and verifies inclusion and non-inclusion proofs with format export for common verifier contracts.

- ⬜ **Wallet Helper** _(stubbed)_: Manages hot, watch-only, and air-gapped wallets with QR-based PSBT shuttling and hardware wallet (Ledger, Trezor, Coldcard) coordination. Computes UTXO coin-control selections, fee strategies, and privacy heuristics like change avoidance.

- ⬜ **Random & Entropy Auditor** _(stubbed)_: Samples system, hardware, and user-supplied entropy sources, running NIST SP 800-22, Dieharder, and ent statistical batteries. Detects biased RNGs, stuck bits, and PRNG state recovery from observed outputs.

- ⬜ **Password Cracker Sandbox** _(stubbed)_: Runs hashcat/John-style attacks against locally provided hashes with mask, dictionary, and rule-based strategies on CPU and GPU. Estimates crack time against current hardware for chosen hash and KDF parameters.

- ⬜ **JWT & Token Workbench** _(stubbed)_: Decodes, signs, and re-signs JWT, PASETO, Macaroons, and Biscuit tokens with algorithm confusion and none-alg attack toggles. Validates claims, expiry, and JWKS resolution against a target issuer.

- ⬜ **MPC Threshold Lab** _(stubbed)_: Runs FROST, GG20, and CMP threshold signing protocols across simulated parties with adjustable network and adversary models. Inspects DKG transcripts, abort conditions, and presigning material.


#### Vintage hardware preservation and retrocomputing

- ⬜ **ROM Dumper Station** _(stubbed)_: Drives EPROM and cartridge programmers (TL866, Minipro, Retrode, custom MiSTer hookups), pulls dumps, hashes each pass, and stages results for ingestion with verify-by-reread on suspect sockets.

- ⬜ **BIOS and Firmware Catalog** _(stubbed)_: Searchable index of BIOS, kernal, and firmware images with SHA1, CRC32, MD5, board revision, source provenance, and No-Intro/TOSEC/Redump cross-references.

- ⬜ **Disk Image Forge** _(stubbed)_: Converts and validates D64, D71, D81, G64, NIB, DSK, ADF, ST, IMG, IMD, and HFE; preserves error info and weak-bit data where the source format supports it.

- ⬜ **Floppy Flux Workbench** _(stubbed)_: Ingests KryoFlux, Greaseweazle, and Applesauce flux streams, visualizes bit cells per track, detects common copy protections, and reconstructs sectors with confidence scores.

- ⬜ **Capacitor Atlas** _(stubbed)_: Per-board capacitor lists (value, voltage, ESR target, polarity, footprint) with vendor part numbers and reform-versus-replace decisions logged per individual unit.

- ⬜ **Emulator Harness** _(stubbed)_: Launches MAME, VICE, FS-UAE, Stella, Hatari, openMSX, and similar cores with auto-selected ROM sets, controller maps, and savestate routing keyed off the local catalog.

- ⬜ **Schematic and Service Manual Archive** _(stubbed)_: Local index of board schematics, service manuals, and chip datasheets with vector search; AI extracts pinouts, test points, and known fault notes from scanned pages.

- ⬜ **Cassette Audio Codec** _(stubbed)_: Encodes PRG, TAP, and T64 payloads to WAV (standard ROM loader plus Turbo Tape, Novaload, and Freeload variants) and decodes recovered audio with adaptive filtering for stretched tapes.

- ⬜ **Filesystem Browser** _(stubbed)_: Mounts and edits CP/M, ProDOS, Apple DOS 3.3, Amiga FFS/OFS, CBM 1541, Atari DOS, and TRSDOS images at the file and sector level with undoable writes.

- ⬜ **Repair Notebook** _(stubbed)_: Structured per-unit log of symptoms, scope traces, swap history, voltage-rail readings, and photos; AI surfaces similar prior cases and likely-next-checks from the notebook corpus.

- ⬜ **ROM Patch Studio** _(stubbed)_: Applies and authors IPS, BPS, UPS, and Xdelta patches, with header-aware ROM splitting and merging plus translation-patch sanity checks against known-good base hashes.

- ⬜ **PCB Silkscreen Reader** _(stubbed)_: Photographs board images and uses OCR plus AI to extract component designators, IC part numbers, and revision marks into a structured parts list tied to the catalog.

- ⬜ **Decap and Die-Shot Library** _(stubbed)_: Archive of die photos and reverse-engineered netlists for custom chips (SID, VIC-II, Paula, Denise, ULA, etc.) with annotated functional blocks and links to FPGA reimplementations.

- ⬜ **Vintage Serial Terminal** _(stubbed)_: Speaks XMODEM, YMODEM, ZMODEM, Kermit, and bespoke vendor handshakes over real serial; bridges to TCP for BBS sessions and modem-emulator workflows.

- ⬜ **CRT Geometry and Convergence Tuner** _(stubbed)_: Generates calibration patterns, captures the screen via webcam, and suggests yoke and convergence adjustments with explicit high-voltage safety reminders before each step.


#### Office productivity

- ⬜ **Word processor** _(stubbed)_: Formatted-document editor with headings, lists, tables, footnotes, cross-references, and track-changes review. Native format plus DOCX, ODT, RTF, and Markdown import and export. AI-integrated drafting, tone-preserving rewrite, and citation handling against the reference manager.

- ⬜ **Spreadsheet** _(stubbed)_: Multi-sheet workbook with formulas, named ranges, conditional formatting, data validation, and CSV/XLSX/ODS exchange. AI-integrated formula synthesis from a plain-language description, data cleaning, and anomaly flagging.

- ⬜ **Slide composer** _(stubbed)_: Deck builder with text, images, diagrams, charts, speaker notes, and a presenter view with timer. Markdown-to-slides import and PPTX/PDF export. AI-integrated outline drafting and layout suggestion from a topic.

- ⬜ **Outline editor** _(stubbed)_: Hierarchical bullet outliner with infinite nesting, fold and zoom, drag-reorder, and bidirectional conversion to and from a long-form document or a slide deck.

- ⬜ **Mind map** _(stubbed)_: Radial node-and-link concept-map canvas with auto-layout, collapsible branches, and export to outline or document. AI-assisted expansion of any node into child nodes from the surrounding context.

- ⬜ **Linked notebook** _(stubbed)_: Zettelkasten-style wiki with bidirectional links, a backlinks pane, tag index, and a graph view of the whole vault. AI-suggested links and tags from note content.

- ⬜ **Daily journal** _(stubbed)_: Date-indexed free-form journal with a calendar grid, daily / weekly / monthly templates, and full-text search across years. AI-integrated weekly and monthly digests.

- ⬜ **Kanban board** _(stubbed)_: Card lanes with WIP limits, swimlanes, labels, and per-card checklists. Cards link bidirectionally to tasks, notes, and documents. AI-integrated auto-routing of incoming cards by content.

- ⬜ **Task inbox** _(stubbed)_: Flat capture surface for GTD-style next-actions with contexts, projects, energy, and time-required filters. AI-integrated triage that proposes context, project, and next-action verb for each captured item.

- ⬜ **Calendar** _(stubbed)_: Day, week, month, and year views with multiple overlaid calendars, ICS import and export, recurrence rules, and time-zone handling. AI-integrated scheduling that proposes time slots from a meeting brief and flags conflicts.

- ⬜ **Contact manager** _(stubbed)_: Personal-CRM contact records with relationship history, custom fields, groups, vCard import and export, and a timeline of past interactions pulled from email and calendar. AI-integrated relationship summarization before a meeting.

- ⬜ **Email client** _(stubbed)_: IMAP/SMTP mail client with threading, server-side rules, filters, and a unified inbox across accounts. AI-integrated thread summarization, draft composition in the user's voice, and triage labels.

- ⬜ **Reference manager** _(stubbed)_: Bibliographic database with PDF ingest, automatic metadata extraction, citation keys, tags, and BibTeX/CSL export. Integrated with the word processor for in-text citations and bibliography generation.

- ⬜ **PDF annotator** _(stubbed)_: PDF reader with highlight, sticky-note, free-text, and shape annotations, plus margin comments and exportable annotation lists in markdown. Annotations flow back into the linked notebook.

- ⬜ **Whiteboard** _(stubbed)_: Infinite-canvas freeform surface with sketch, shape, text, sticky-note, image, and lasso-group tools, plus snap-to-grid for tidy diagrams. AI-assisted cleanup of hand-drawn shapes into rectilinear ones.

- ⬜ **Diagram editor** _(stubbed)_: Structured-diagram tool for flowcharts, swimlanes, sequence diagrams, ERDs, and state machines, driven by a text DSL (mermaid-style) with bidirectional graphical editing. AI-integrated generation from a textual description.

- ⬜ **Gantt timeline** _(stubbed)_: Project timeline with tasks, dependencies, milestones, baselines, and critical-path highlighting. Imports from the task inbox and exports to MS Project XML.

- ⬜ **Pivot table builder** _(stubbed)_: Drag-and-drop aggregation over a tabular dataset with row, column, value, and filter buckets, computed fields, and drill-down. AI-integrated suggestion of pivot configurations from a question about the data.

- ⬜ **Chart builder** _(stubbed)_: Axis, series, and style picker over a dataset with categorical, time-series, statistical, and geographic chart families. AI-integrated suggestion of chart type from the data shape and the question being asked.

- ⬜ **Equation editor** _(stubbed)_: LaTeX and MathML equation editor with a palette, live preview, numbered references, and inline rendering in documents, slides, and notes. AI-integrated conversion from a natural-language description of an expression.

- ⬜ **Mail merge** _(stubbed)_: Template-plus-dataset engine that produces per-recipient documents, PDFs, labels, envelopes, and email sends. Supports conditional sections and per-field formatters.

- ⬜ **OCR scanner** _(stubbed)_: Image and PDF to searchable text with layout preservation, table extraction to CSV, language detection, and handwriting recognition for scanned notebooks.

- ⬜ **Meeting minutes extractor** _(stubbed)_: Pipeline that ingests an audio recording or transcript and emits a structured minutes document with summary, decisions, and action items linked into the task inbox.

- ⬜ **Document diff and merge** _(stubbed)_: Side-by-side comparison and three-way merge across DOCX, ODT, PDF, and Markdown, with change-level accept/reject, semantic grouping of edits, and a per-author authorship view.


#### Project management

- ⬜ **Ticket tracker** _(stubbed)_: Local-first issue database with markdown bodies, custom fields, labels, assignees, parent/child links, and saved queries. AI-integrated triage proposes labels, priority, and likely owner from the ticket text and history.

- ⬜ **Sprint planner** _(stubbed)_: Iteration container that pulls from the backlog under a capacity budget, with point or hour sizing, scope-lock dates, and carryover tracking. AI-integrated suggestion of a sprint composition given velocity history and explicit team availability.

- ⬜ **Gantt scheduler** _(stubbed)_: Time-axis chart of tasks with start/end dates, dependencies, critical-path highlighting, baseline vs current snapshots, and per-row resource assignment. Slippage is propagated through the dependency graph.

- ⬜ **Roadmap visualizer** _(stubbed)_: Quarter or month grid of epics and themes with status, confidence, and target windows, rendered as swimlanes per team or product area. Supports now/next/later columns as an alternate layout.

- ⬜ **Burndown and burnup charts** _(stubbed)_: Scope and remaining-work charts for sprints, releases, and epics, with ideal-trend overlays and scope-change annotations. Forecast lines are projected from rolling velocity.

- ⬜ **Velocity tracker** _(stubbed)_: Per-iteration throughput in points, tickets closed, and hours, with rolling averages and confidence bands. Outliers are flagged with the events that caused them (holidays, incidents, scope cuts).

- ⬜ **Time tracker** _(stubbed)_: Timer and manual-entry log of effort against tickets, with daily/weekly timesheets, idle detection, and per-project rollups. AI-integrated activity classification suggests which ticket recent terminal, editor, and browser activity belongs to.

- ⬜ **Estimation board** _(stubbed)_: Planning-poker-style estimation surface for single-user or multi-seat sessions, with reference tickets, t-shirt or Fibonacci scales, and reveal/discuss flow. AI provides a baseline estimate from comparable closed tickets and is treated as one anonymous voter.

- ⬜ **Backlog groomer** _(stubbed)_: Filtered list view that surfaces stale, underspecified, duplicate, or dependency-blocked tickets and walks them one at a time. AI-integrated rewriter expands one-line tickets into structured acceptance criteria and clarifying questions.

- ⬜ **Dependency graph** _(stubbed)_: Directed graph of blocks/blocked-by, parent/child, and cross-project links, with cycle detection and shortest-path tracing from any leaf to its containing epic. Nodes are colorable by status, owner, or risk.

- ⬜ **Milestone and release tracker** _(stubbed)_: Versioned releases with target dates, scope sets, freeze windows, and per-ticket inclusion criteria. Computes release readiness from open-blocker count and required-review state.

- ⬜ **OKR and goal tracker** _(stubbed)_: Objective/key-result hierarchy with measurable targets, current values, check-in cadence, and confidence scores. Key results link to the tickets and metrics that drive them, with automatic progress rollup.

- ⬜ **Risk register** _(stubbed)_: Catalog of identified risks with probability, impact, owner, mitigation, and trigger conditions, scored into a heatmap. AI-integrated scan of recent tickets, retros, and incidents proposes new candidate risks.

- ⬜ **Retrospective workspace** _(stubbed)_: Structured retro session with templates (start/stop/continue, 4Ls, sailboat), sticky-note clustering, dot voting, and action-item handoff to the ticket tracker. AI-integrated theme extraction clusters notes and drafts a session summary.

- ⬜ **Standup and status synthesizer** _(stubbed)_: Compiles per-person daily notes, ticket transitions, commits, and time entries into a standup digest and a weekly status report. AI rewrites raw activity into prose at a chosen audience level (team, manager, stakeholder).

- ⬜ **Resource and capacity planner** _(stubbed)_: Per-person availability calendar with PTO, on-call, and meeting load, overlaid against committed work to expose over- and under-allocation. Plans across a quarter at team-resolution and per-week at individual-resolution.

- ⬜ **Workflow designer** _(stubbed)_: State-machine editor for ticket lifecycles, with per-state required fields, transition guards, and automation hooks (assign on transition, post to channel, open follow-up). Workflows are versioned and applicable per project.

- ⬜ **Meeting notes and action extractor** _(stubbed)_: Notes surface with agenda, attendees, and timestamped sections, accepting pasted or recorded transcripts. AI-integrated extraction lifts action items, owners, due dates, and decisions into tickets and the decision log.

- ⬜ **Stakeholder and RACI map** _(stubbed)_: Roster of stakeholders with role, interest, influence, and communication cadence, plus a RACI matrix per workstream. Drives reminder generation for status updates owed to each stakeholder.

- ⬜ **Changelog and release-notes generator** _(stubbed)_: Aggregates merged tickets, commits, and PRs between two release tags into grouped notes by category (feature, fix, breaking). AI-integrated rewrite produces user-facing copy at a chosen tone and length.

- ⬜ **SLA and cycle-time analytics** _(stubbed)_: Distributions of lead time, cycle time, time-in-state, and reopen rate across any ticket query, with percentile bands and breach detection against configured SLAs. Cohort comparisons across teams, labels, or releases.

- ⬜ **Project template library** _(stubbed)_: Reusable bundles of epics, tickets, workflows, fields, and roadmap entries instantiable into a new project with parameter substitution. Templates are diffable and updatable so spawned projects can pull upstream changes.

- ⬜ **Cost and budget tracker** _(stubbed)_: Per-project ledger of estimated and actual cost from time entries, fixed costs, and external invoices, charted against budget over time. Drill-down by ticket, epic, or resource with burn-rate forecasting.

- ⬜ **Scope-change monitor** _(stubbed)_: Diff of sprint or release scope across snapshots, attributing each addition, removal, or resize to a user, time, and reason. AI-integrated classifier flags additions as in-scope clarification, true scope creep, or split-out work.


#### Quality, safety, risk, and compliance

- ⬜ **FMEA editor** _(stubbed)_: Builds Design and Process FMEA worksheets with severity, occurrence, and detection scoring, computes RPN and AIAG-VDA action priority, and links failure modes to causes, controls, and recommended actions.

- ⬜ **SPC control chart studio** _(stubbed)_: Plots Xbar-R, Xbar-S, I-MR, p, np, c, u, CUSUM, and EWMA charts from time-series measurements, applies Nelson and Western Electric rules, and flags out-of-control points with annotated runs.

- ⬜ **Process capability analyzer** _(stubbed)_: Computes Cp, Cpk, Pp, Ppk, and Cpm against engineering tolerances, tests normality (Anderson-Darling, Shapiro-Wilk), and fits Johnson or Box-Cox transforms for non-normal data with capability indices recalculated.

- ⬜ **Gage R&R workbench** _(stubbed)_: Runs Type 1, Type 2, and Type 3 MSA studies with ANOVA and Average-Range methods, decomposes variance into repeatability, reproducibility, part, and operator components, and reports %study variation, %tolerance, ndc, and Kappa for attribute gages.

- ⬜ **DMAIC notebook** _(stubbed)_: Structures Define, Measure, Analyze, Improve, Control phases with project charter, SIPOC, CTQ tree, data-collection plan, hypothesis tests, and control plan, and tracks gate reviews and tollgate sign-offs.

- ⬜ **Root-cause analyzer** _(stubbed)_: Combines Ishikawa fishbone, 5-Why ladders, and Pareto charts in one canvas, links causes to defect data, and surfaces AI-suggested cause categories from incident text and process variables.

- ⬜ **Fault tree and bowtie analyzer** _(stubbed)_: Constructs FTA and ETA diagrams with AND/OR/voting gates, computes top-event probability and minimal cut sets, and renders bowtie views joining threats, barriers, top events, consequences, and escalation factors.

- ⬜ **HAZOP and LOPA workbook** _(stubbed)_: Walks process nodes with guideword and parameter prompts (No, More, Less, Reverse, As-Well-As), records deviations, causes, consequences, and safeguards, and runs Layer of Protection Analysis to derive required SIL per IEC 61511.

- ⬜ **Risk-matrix heatmap** _(stubbed)_: Renders configurable likelihood-by-consequence matrices (3x3, 5x5, ISO 31000, MIL-STD-882) with calibrated probability bands, plots inherent vs residual risk, and exports ALARP demonstrations.

- ⬜ **Hazard register** _(stubbed)_: Maintains a structured hazard inventory tagged by location, activity, energy source, and affected receptor, links each hazard to controls, owners, due dates, and references HIRA, JHA, and bowtie records.

- ⬜ **Incident and CAPA tracker** _(stubbed)_: Captures incidents, near-misses, and nonconformances with 8D, A3, or Kepner-Tregoe templates, tracks corrective and preventive actions to closure with effectiveness checks, and produces OSHA 300/301/300A and ISO 45001 reports.

- ⬜ **DOE designer** _(stubbed)_: Generates full and fractional factorial, Plackett-Burman, central composite, Box-Behnken, and Taguchi orthogonal arrays, analyzes effects with ANOVA and half-normal plots, and renders response-surface contour and desirability optimization.

- ⬜ **Acceptance sampling planner** _(stubbed)_: Builds single, double, and sequential sampling plans per ANSI/ASQ Z1.4 and Z1.9, MIL-STD-105E, and Dodge-Romig, plots OC, AOQ, ASN, and ATI curves, and computes producer and consumer risks at given AQL/LTPD.

- ⬜ **Weibull reliability analyzer** _(stubbed)_: Fits Weibull, lognormal, exponential, and Gumbel distributions to life and time-to-failure data with MLE and rank regression, handles right-, left-, and interval-censoring, and computes MTBF, B10, hazard, and reliability with confidence bounds.

- ⬜ **JSA and permit-to-work builder** _(stubbed)_: Decomposes tasks into steps, hazards, and controls per ANSI/ASSP Z10 and OSHA 1910 references, generates hot-work, confined-space, energized-work, and lockout/tagout permits, and emits PPE matrices and isolation lists.

- ⬜ **ISO and OSHA compliance crosswalk** _(stubbed)_: Maps controls across ISO 9001, 14001, 45001, 27001, IATF 16949, AS9100, OSHA 1910/1926, FDA 21 CFR 820, and EU MDR, highlights overlapping clauses, and tracks evidence artifacts per requirement.

- ⬜ **Audit and nonconformance tracker** _(stubbed)_: Schedules internal, supplier, and certification audits with checklists derived from selected standards, logs findings as major, minor, observation, or OFI, and routes NCRs through root-cause, containment, correction, and verification.

- ⬜ **Ergonomics and exposure assessor** _(stubbed)_: Scores tasks with RULA, REBA, NIOSH Lifting Equation, Snook tables, Liberty Mutual MMH, and Strain Index, and computes noise dose (OSHA, NIOSH), heat stress (WBGT), and chemical TWA/STEL against ACGIH TLVs.


#### Logistics, supply chain, operations research

- ⬜ **VRP solver** _(stubbed)_: Solves capacitated vehicle routing with time windows, pickup/delivery, heterogeneous fleet, and multi-depot constraints using OR-Tools / VROOM-style metaheuristics (guided local search, LNS). Imports OSM road networks for real travel times and exports per-driver manifests.

- ⬜ **TSP playground** _(stubbed)_: Solves large symmetric and asymmetric TSPs via Lin-Kernighan (LKH-style), Concorde-style branch-and-cut for optimality, and Christofides for bounded approximation. Visualizes tour evolution and gap-to-lower-bound in real time.

- ⬜ **Inventory policy designer** _(stubbed)_: Computes EOQ, (s,S), (R,Q), and base-stock policies with safety stock from target cycle service level or fill rate. Handles lead-time variability, stochastic demand distributions, and joint replenishment; AI-suggested policy class given demand-pattern fingerprint (smooth, erratic, intermittent, lumpy).

- ⬜ **Demand forecaster** _(stubbed)_: Fits ARIMA, ETS (Holt-Winters), Theta, Croston / TSB for intermittent series, and gradient-boosted hierarchical models with reconciliation (MinT, bottom-up, top-down). Reports MAPE/WAPE/MASE with backtesting folds and prediction intervals.

- ⬜ **Newsvendor solver** _(stubbed)_: Computes optimal single-period order quantity from underage/overage costs and a demand distribution (empirical, normal, lognormal, custom). Sweeps service-level vs expected-profit curves and supports salvage value, emergency procurement, and risk-averse CVaR objectives.

- ⬜ **Queueing analyzer** _(stubbed)_: Evaluates M/M/c, M/G/1, G/G/c (via Allen-Cunneen approximation), and finite-population queues, returning Lq, Wq, utilization, and tail probabilities. Composes Jackson and BCMP networks for multi-station systems and validates against Little's Law.

- ⬜ **Discrete-event simulator** _(stubbed)_: Builds SimPy-style process models for warehouses, terminals, and supply chains with resources, queues, preemption, and stochastic arrivals. Runs replications with common random numbers, computes confidence intervals, and supports rare-event importance sampling.

- ⬜ **Job shop scheduler** _(stubbed)_: Solves job-shop, flow-shop, open-shop, and flexible JSP via CP-SAT and disjunctive-graph heuristics (shifting bottleneck, tabu search). Optimizes makespan, total tardiness, or weighted flowtime; renders Gantt charts and critical paths.

- ⬜ **Bullwhip simulator** _(stubbed)_: Runs the beer distribution game across a configurable echelon chain with adjustable ordering policies, lead times, and information sharing levels. Plots demand amplification, variance ratios, and inventory oscillation per echelon.

- ⬜ **Facility location optimizer** _(stubbed)_: Solves p-median, p-center, set-cover, and capacitated facility location with fixed costs and assignment constraints via MILP. Performs sensitivity analysis on demand and overlays candidate sites on the map with weighted catchments.

- ⬜ **Warehouse slotting tool** _(stubbed)_: Performs ABC / XYZ classification and cube-per-order indexing, then assigns SKUs to pick locations to minimize expected travel given affinity (order-line co-occurrence) and ergonomic golden-zone constraints. Compares S-shape, return, midpoint, and largest-gap routing.

- ⬜ **Pick path optimizer** _(stubbed)_: Generates optimal or near-optimal picker routes through aisle-rack-bin grids using Ratliff-Rosenthal exact DP for rectangular warehouses and metaheuristics for irregular layouts. Supports zone picking, batch picking with capacity, and wave release simulation.

- ⬜ **Container loader** _(stubbed)_: Solves 3D bin packing and pallet building with weight, stacking, fragility, axis-load, and load-stop sequence constraints. Visualizes the load plan in 3D and reports utilization, center-of-gravity, and reload effort at each stop.

- ⬜ **Network flow solver** _(stubbed)_: Models transportation, transshipment, min-cost-flow, multi-commodity flow, and assignment problems with a Pyomo/PuLP-style algebraic interface. Routes to CBC, HiGHS, GLPK, or SCIP and returns duals, reduced costs, and sensitivity ranges.

- ⬜ **Multi-echelon inventory simulator** _(stubbed)_: Simulates serial, assembly, and distribution networks under METRIC, Clark-Scarf, and guaranteed-service models with stochastic lead times and capacitated nodes. Optimizes stock placement and reports fill rate, backorder cost, and on-hand by stage.

- ⬜ **Crew scheduler** _(stubbed)_: Builds driver, picker, and shift rosters via set-partitioning column generation, honoring HOS / labor rules, skill requirements, and fairness constraints. Supports rolling-horizon re-optimization when call-outs or demand spikes occur.

- ⬜ **Monte Carlo risk simulator** _(stubbed)_: Runs stochastic supply chain stress tests for supplier outages, port closures, and demand shocks with copula-correlated risk factors. Produces VaR, CVaR, and time-to-recovery distributions and identifies bottleneck nodes via sensitivity indices.

- ⬜ **Isochrone analyzer** _(stubbed)_: Computes drive-time and walk-time catchments over an OSM-derived road graph for last-mile and depot-siting studies. Overlays population, demand density, and competitor coverage, and exports service polygons for downstream VRP and facility-location runs.


#### Writing, publishing, journalism

- ⬜ **Manuscript outliner** _(stubbed)_: Hierarchical scene and chapter tree with beat-sheet templates (Save the Cat, Story Grid, Three-Act, Snowflake) and drag-reorder. Each node carries POV, setting, word-count target, status, and synopsis fields that roll up to act and book level.

- ⬜ **Character bible** _(stubbed)_: Structured profiles with relationship graph, trait fields, voice samples, and arc tracker per character. AI-integrated continuity check surfaces later prose that contradicts earlier canonical facts with chapter and line references.

- ⬜ **World atlas** _(stubbed)_: Wiki-style store for locations, factions, calendars, currencies, languages, and rules-of-magic or tech entries under custom schemas. Inline [[references]] resolve as backlinks across the manuscript and a map view plots geo entries.

- ⬜ **Timeline editor** _(stubbed)_: Aeon-style multi-track timeline over a custom calendar with character arcs, world events, and POV scenes. Flags ordering inconsistencies between manuscript timestamps, timeline entries, and character ages.

- ⬜ **Prose linter** _(stubbed)_: Longform-tuned style pass covering passive voice, filter words, adverb density, sentence-length variance, POV and tense drift, dialogue-tag overuse, and repeated openings. Configurable per project against AP, Chicago, MLA, or custom style guides.

- ⬜ **Readability analyzer** _(stubbed)_: Flesch-Kincaid, Gunning Fog, Dale-Chall, SMOG, and ARI scoring with per-paragraph reading level heatmap. Tracks deltas across drafts and target-audience bands (MG, YA, trade, academic).

- ⬜ **Citation manager** _(stubbed)_: Local BibTeX and CSL-JSON library with PDF attachments, web-snapshot capture, DOI and ISBN lookup, and in-text cite insertion. Bibliography export in any CSL style; deduplicates entries across imported libraries.

- ⬜ **Fact-checker** _(stubbed)_: Extracts factual claims from the draft and pairs each with cited or candidate sources, marking unsupported assertions. AI-integrated claim extraction with per-source confidence and a side panel for journalist sign-off.

- ⬜ **Interview transcriber** _(stubbed)_: Local-first ASR with speaker diarization, timestamped paragraphs, confidence-marked tokens, and click-to-play audio sync. Exports to plain text, SRT, VTT, or directly into the quote vault with source-file pointers preserved.

- ⬜ **Quote vault** _(stubbed)_: Searchable store of speaker-attributed quotes with source clip pointer, consent and embargo status, on-record flag, and topic tags. One-click insertion drops a properly attributed pull quote into the active draft.

- ⬜ **Reverse outliner** _(stubbed)_: Parses an existing draft into scene and chapter cards with auto-summarized purpose, conflict, stakes, and outcome per node. Useful for revision passes, structural audits, and producing back-of-envelope synopses for queries.

- ⬜ **Pacing visualizer** _(stubbed)_: Per-scene tension, word count, dialogue ratio, and POV plotted across the manuscript as a strip chart. Surfaces sag points, POV imbalance, and chapters that overrun typical length for the chosen genre.

- ⬜ **Sensitivity and bias reader** _(stubbed)_: Flags stereotypes, slurs, dated terminology, and representation gaps in cast and sourcing. AI-integrated rationale links to a citable style or community reference rather than issuing a verdict; suppression and override rules per project.

- ⬜ **Anachronism checker** _(stubbed)_: Cross-references vocabulary, named entities, technologies, and brand mentions against the story's stated period or setting. Flags terms postdating the era using etymology and product-launch data from vendored Wiktionary and reference dumps.

- ⬜ **Manuscript typesetter** _(stubbed)_: LaTeX and Pandoc backed page layout with running heads, drop caps, microtype, ligature handling, widow and orphan control, and CMYK or grayscale output. Produces press-ready PDF, accessible EPUB, and reflowable HTML from one source.

- ⬜ **Ebook compiler** _(stubbed)_: EPUB3, MOBI, AZW3, and KFX builds with embedded fonts, accessible navigation, MathML, SVG cover, and image optimization. Validates against epubcheck and KDP Previewer rules and reports failures inline with the offending file and line.

- ⬜ **Submission tracker** _(stubbed)_: Query log per agent, editor, or publication with status timeline (queried, partial, full, passed, offered, withdrawn), simultaneous-submission rules, and follow-up reminders. Imports Submittable, QueryTracker, and Duotrope exports and respects per-market response-time norms.

- ⬜ **Public-records desk** _(stubbed)_: FOIA and FOI request drafter with per-agency templates, statutory deadline tracking, appeal scaffolding, redaction OCR, and response indexer. Cross-links released documents to draft sections that cite them.


#### Education and pedagogy

- ⬜ **Problem Set Generator** _(stubbed)_: Generates quizzes and worksheets with answer keys, parameterized by topic, item count, difficulty band, and Bloom's level. Supports MCQ, short-answer, numeric, code, and proof items with AI-integrated drafting from a source text or learning objective.

- ⬜ **Spaced Repetition Deck Builder** _(stubbed)_: Builds flashcard decks with SM-2 and FSRS scheduling, cloze deletion, and image occlusion. Imports and exports Anki .apkg, Mnemosyne, and CSV, with deduplication and reverse-card generation.

- ⬜ **Lesson Plan Composer** _(stubbed)_: Drafts lesson plans with objectives, materials, timed segments, differentiation tiers, and standards alignment. Outputs to printable handouts, slides, and a structured JSON the rest of the workbench can read.

- ⬜ **Rubric Builder** _(stubbed)_: Constructs analytic and holistic rubrics as criterion-by-level matrices with point weights and descriptor cells. Includes AI-integrated descriptor expansion from a single anchor phrase per cell.

- ⬜ **Autograder** _(stubbed)_: Grades free-text, math, and code submissions against a rubric or reference solution, returning per-criterion scores, rationale, and partial credit. Sandboxes code execution with configurable test harnesses.

- ⬜ **Plagiarism Checker** _(stubbed)_: Scores passage-level similarity against a local corpus and optional web sources using n-gram and embedding methods. Reports matched spans, sources, and a paraphrase-likelihood signal.

- ⬜ **Concept Map Builder** _(stubbed)_: Edits node-link graphs of prerequisites, relationships, and worked examples, with force-directed and hierarchical layouts. AI-integrated extraction pulls candidate concepts and edges from supplied text.

- ⬜ **Standards Aligner** _(stubbed)_: Tags items, lessons, and units to Common Core, NGSS, ISTE, AP, IB, or a user-defined framework. Produces coverage heatmaps and gap reports across a course.

- ⬜ **Distractor Generator** _(stubbed)_: Produces plausible wrong answers for multiple-choice items, biased toward common misconceptions or controlled error categories. Useful for converting open-response banks into MCQs without trivial decoys.

- ⬜ **Item Calibrator** _(stubbed)_: Fits 1PL, 2PL, and 3PL IRT models from response logs to estimate item difficulty, discrimination, and guessing. Flags misfit items and produces test information curves for blueprint planning.

- ⬜ **Classroom Runner** _(stubbed)_: Presents a quiz live with a projector view, anonymous response capture from learner devices on the local network, and per-question timing. Streams aggregate response distributions back to the instructor view.

- ⬜ **Mastery Tracker** _(stubbed)_: Maintains per-learner knowledge state across a skill graph using Bayesian knowledge tracing and forgetting-curve projections. Recommends the next review item and predicts retention at chosen horizons.

- ⬜ **Worked Example Generator** _(stubbed)_: Produces step-by-step solutions for a target problem, plus faded-step and hint-laddered variants for scaffolded practice. Renders math via LaTeX and code via syntax-highlighted blocks.

- ⬜ **Socratic Tutor** _(stubbed)_: Drives dialogic practice by asking leading questions, probing reasoning, and refusing to volunteer the answer until the learner is close. AI-integrated with configurable persona, patience, and scaffolding depth.

- ⬜ **OCR Grader** _(stubbed)_: Extracts handwritten or photographed student work, segments it into answer regions, and matches each to an expected solution. Surfaces low-confidence regions for instructor review.

- ⬜ **Syllabus Planner** _(stubbed)_: Lays out a course week by week with topics, readings, assessments, holidays, and pacing constraints. Solves for even workload, prerequisite ordering, and review-week placement.

- ⬜ **Exit Ticket Designer** _(stubbed)_: Generates one to three question formative checks tied to a lesson's stated objectives, with an expected-response key and a misconception map. Outputs to print, slide, or classroom-runner format.

- ⬜ **Note-to-Deck Converter** _(stubbed)_: Ingests lecture notes, slides, or PDFs and emits spaced-repetition cards with cloze, Q-and-A, and image-occlusion forms. Ranks candidate cards by salience and lets the user accept or edit before adding to a deck.


#### Real estate, insurance, actuarial

- ⬜ **Amortization schedule builder** _(stubbed)_: Generates a payment-by-payment table for fixed, ARM, interest-only, and balloon mortgages, with principal/interest split, remaining balance, and effects of extra principal payments or recasts.

- ⬜ **Mortgage scenario comparator** _(stubbed)_: Compares loan offers side by side across rate, term, points, PMI, and closing costs, computing APR, total interest, and break-even points between scenarios.

- ⬜ **Cap rate and NOI calculator** _(stubbed)_: Builds net operating income from rent roll, vacancy, opex, and reserves, then derives cap rate, gross rent multiplier, and price per door from sale or list price.

- ⬜ **Rental pro forma modeler** _(stubbed)_: Projects multi-year cash flow for an income property including rent growth, expense inflation, capex reserves, financing, depreciation, and after-tax IRR, NPV, equity multiple, and cash-on-cash return.

- ⬜ **Comps analyzer** _(stubbed)_: Loads comparable sales, normalizes price per sqft, and applies grid adjustments (location, condition, GLA, beds/baths, lot, time) to produce an adjusted indicated value with a reconciled range.

- ⬜ **DSCR and debt yield calculator** _(stubbed)_: Computes debt service coverage ratio, debt yield, LTV, and break-even occupancy for an income property given NOI and proposed loan terms, with sensitivity tables over rate and NOI.

- ⬜ **Refinance break-even analyzer** _(stubbed)_: Calculates the months-to-recover on a refinance by comparing new payment, closing costs, and remaining term against the existing loan, including cash-out and rate-and-term variants.

- ⬜ **Cap stack and waterfall modeler** _(stubbed)_: Stacks senior debt, mezz, preferred equity, and common equity, then runs a distribution waterfall with hurdles, IRR lookbacks, catch-ups, and promote splits for GP/LP returns.

- ⬜ **Mortality table browser** _(stubbed)_: Loads standard tables (SOA 2017 CSO, Annuity 2012, US Life Tables, AM92) and queries qx, px, lx, dx, ex, and select-and-ultimate cells with table-vs-table comparison plots.

- ⬜ **Life table generator** _(stubbed)_: Constructs period or cohort life tables from raw deaths and exposure, smooths qx via Whittaker-Henderson or Kannisto, and graduates with confidence bands.

- ⬜ **Life contingencies pricer** _(stubbed)_: Prices term, whole life, endowment, and annuities (immediate, deferred, joint-life, last-survivor) from a chosen mortality table and interest assumption, returning net and gross premiums with loadings and reserves.

- ⬜ **Loss triangle reserve estimator** _(stubbed)_: Loads claim development triangles (paid, incurred, counts) and runs chain-ladder with volume-weighted, simple-average, and selected LDFs; outputs ultimates, IBNR, and Mack standard errors.

- ⬜ **Bornhuetter-Ferguson and Cape Cod estimator** _(stubbed)_: Runs BF and Cape Cod reserving methods alongside chain-ladder using a priori loss ratios and exposure, with blended ultimates and a reconciliation report across methods.

- ⬜ **Frequency-severity simulator** _(stubbed)_: Simulates aggregate losses from compound Poisson, negative binomial, and mixed frequency models combined with lognormal, Pareto, or empirical severity, producing VaR, TVaR, and exceedance probability curves.

- ⬜ **GLM rate modeler** _(stubbed)_: Fits Poisson, gamma, and Tweedie GLMs to policy-level frequency and severity data for ratemaking, with relativity tables, lift charts, double-lift comparisons, and one-way exhibits.

- ⬜ **Lee-Carter mortality forecaster** _(stubbed)_: Decomposes historical age-specific mortality into kt and bx factors via SVD, projects kt with ARIMA, and produces forecasted qx surfaces with prediction intervals for longevity work.

- ⬜ **Reinsurance treaty modeler** _(stubbed)_: Applies quota share, surplus, per-risk XoL, per-occurrence XoL, and stop-loss layers to a simulated or historical loss set, returning ceded/net distributions, reinstatement premiums, and expected layer cost.

- ⬜ **Credibility and experience rating tool** _(stubbed)_: Applies limited fluctuation (classical) and Buhlmann-Straub credibility to experience data, blending observed and prior loss ratios with credibility weights for individual risk rating.


#### Finance, markets, accounting

- ⬜ **Stocks and options analytics** _(stubbed)_: equity screener, fundamentals viewer, options chain + greeks, implied-volatility surface, payoff diagrams for multi-leg strategies, backtester for systematic strategies, position/P&L tracker. AI-integrated suggestion of hedges from a portfolio's beta/sector concentration.

- ⬜ **Crypto trading and wallet tools** _(stubbed)_: exchange API integration (Coinbase/Kraken/Binance via API key), order book viewer, portfolio across chains (ETH/BTC/SOL/etc.), wallet inspector, transaction-fee optimizer, DEX swap interface, on-chain analytics (Etherscan/Solscan-style).

- ⬜ **Prediction markets (Polymarket, Kalshi, Manifold)** _(stubbed)_: browse markets, view price history, execute trades through API where available, track positions, AI-integrated forecasting comparison vs. base rates and other markets.

- ⬜ **Ledger Workbench** _(stubbed)_: Plain-text double-entry general ledger in the Beancount/hledger tradition: posts journal entries, computes trial balance, income statement, and balance sheet from a single append-only file. AI-integrated classification proposes account splits from free-text memos and prior-coded transactions.

- ⬜ **Invoice & Quote Builder** _(stubbed)_: Composes invoices, estimates, and quotes from line items, products, and contacts; renders to print-ready PDF with configurable templates, tax/VAT lines, and multi-currency totals. Tracks sent/paid/overdue state and emits matching ledger postings on payment.

- ⬜ **Receipt & Document Capture** _(stubbed)_: Ingests receipts, bills, and statements via drag-drop or scan, runs OCR plus AI-integrated field extraction to populate vendor, date, amount, tax, and line items, and links the image to the resulting expense entry.

- ⬜ **Bank Statement Importer** _(stubbed)_: Parses CSV, OFX, QIF, QFX, MT940, and CAMT.053 statements from any bank, normalizes column layouts, deduplicates against existing postings, and stages transactions for review with AI-suggested category mapping.

- ⬜ **Reconciliation Studio** _(stubbed)_: Matches imported bank lines against ledger postings with fuzzy date/amount tolerance, surfaces unmatched items, and walks a guided three-way reconciliation against statement balance. Flags rounding, FX, and duplicate-entry causes of drift.

- ⬜ **Tax Form Filler** _(stubbed)_: Fills US and international tax forms (1040 and schedules, Schedule C, 1099, W-2, W-9, UK SA100, German EUR) from ledger data, with line-by-line provenance back to source transactions and an AI-integrated explanation of each computed figure.

- ⬜ **Portfolio Tracker** _(stubbed)_: Tracks equity, ETF, bond, crypto, and cash holdings across multiple accounts and currencies; computes time-weighted and money-weighted returns, allocation drift, dividend income, and realized/unrealized gains with FIFO, LIFO, and average-cost lots.

- ⬜ **DCF & Scenario Modeler** _(stubbed)_: Builds discounted-cash-flow models with assumption sheets, sensitivity tables, and tornado charts; supports terminal value via Gordon growth or exit multiple. Side-by-side scenarios share drivers so one knob updates every linked output.

- ⬜ **Currency Converter** _(stubbed)_: Converts between fiat and crypto at spot or historical rates from vendored daily FX series, with offline fallback and pluggable live feeds. Handles cross-rate triangulation and inline conversion inside any other tool.

- ⬜ **Budget & Cash Flow Forecaster** _(stubbed)_: Projects month-by-month inflows and outflows from recurring rules, scheduled invoices, and ledger trends; surfaces runway, minimum-balance dates, and seasonality. AI-integrated anomaly detection flags categories drifting from baseline.

- ⬜ **Amortization & Loan Designer** _(stubbed)_: Schedules amortization for mortgages, auto loans, student loans, and custom instruments with fixed, variable, balloon, interest-only, or step-rate terms; compares prepayment, refinance, and recast strategies side by side with total-interest and break-even analysis.

- ⬜ **Option Pricer** _(stubbed)_: Prices European and American options via Black-Scholes-Merton, binomial, and Monte Carlo with Greeks, implied volatility solver, and dividend handling. Includes a payoff-diagram builder for multi-leg strategies (spreads, condors, straddles).

- ⬜ **Bond Math Workbench** _(stubbed)_: Computes price, yield-to-maturity, yield-to-call, duration (Macaulay, modified, effective), and convexity for fixed- and floating-rate bonds; plots yield curves and runs parallel-shift and key-rate scenarios on a held portfolio.

- ⬜ **Monte Carlo Retirement Simulator** _(stubbed)_: Simulates lifetime spending, withdrawal strategies, Social Security claim timing, and Roth-conversion ladders over thousands of return-and-inflation paths; reports success probability, percentile wealth bands, and sequence-of-returns risk.

- ⬜ **Cap Table Modeler** _(stubbed)_: Tracks shares, options, SAFEs, convertible notes, and warrants across founding, seed, and priced rounds; computes pre/post-money dilution, waterfall payouts at exit, and option-pool top-up effects. Generates 409A-style summary outputs.

- ⬜ **Crypto Cost-Basis Tracker** _(stubbed)_: Ingests on-chain transactions and exchange exports, reconstructs cost basis per lot across wallets, handles staking, airdrops, hard forks, DeFi swaps, and bridges, and exports realized-gain reports for tax filing in multiple jurisdictions.

- ⬜ **Financial Statement Analyzer** _(stubbed)_: Loads 10-K, 10-Q, and 20-F filings or pasted statements, normalizes line items, and computes liquidity, leverage, profitability, efficiency, and valuation ratios across periods; benchmarks against industry medians with AI-integrated commentary on outliers.

- ⬜ **Net Worth Dashboard** _(stubbed)_: Aggregates assets, liabilities, and unrealized positions from every linked account into a single net-worth time series with attribution by asset class, currency, and account; supports manual entries for illiquid holdings (real estate, private equity, art).

- ⬜ **Subscription Auditor** _(stubbed)_: Detects recurring charges from transaction history, clusters duplicates and price hikes, estimates annualized spend, and suggests cancellations ranked by cost-per-use. Flags free-trial conversions and anniversary renewal windows.

- ⬜ **LBO & M&A Modeler** _(stubbed)_: Models leveraged buyouts and acquisitions with sources-and-uses, debt schedule (revolver, term loans, mezzanine, PIK), purchase-price allocation, and IRR/MOIC waterfalls across sponsor, management, and lenders. Supports stub-period and multi-tranche refinancing.


#### Legal, patents, trademarks, compliance

- ⬜ **Patent searcher** _(stubbed)_: queries USPTO, EPO, WIPO, Google Patents and CCD with semantic search across abstracts, claims, and figures. Builds citation graphs, finds patent families, exports BibTeX/CSV. AI-integrated novelty assessment against drafted disclosures.

- ⬜ **Patent drafter** _(stubbed)_: writes provisional and non-provisional applications: spec, claims (independent + dependent), drawings markup, abstract. Generates claim trees, supports claim charts for infringement comparison, runs an AI prior-art sanity check before filing.

- ⬜ **Trademark searcher** _(stubbed)_: searches USPTO TESS, Madrid, EUIPO, and common-law uses. Pulls similar marks by phonetic/visual/conceptual similarity. Drafts statements of use, opposition responses, and TTAB filings.

- ⬜ **Patent litigation docket** _(stubbed)_: tracks ITC and district-court patent cases, attached complaints/answers/orders, claim-chart progress, expert reports, deposition transcripts. Reminders for deadlines.

- ⬜ **Historical patent browser** _(stubbed)_: OCR + cleanup of 19th and early-20th century patents from USPTO/EPO/British archives. Searchable across original handwritten or letterpress text. Useful for prior-art beyond modern indexes and for engineering history research.

- ⬜ **Contract Drafter** _(stubbed)_: Composes structured agreements (NDAs, MSAs, employment, IP assignment, SAFEs, leases, wills, powers of attorney) from a template library and a parameter sheet. AI-integrated drafting from a one-line description of the deal, with jurisdiction-aware clause selection and party-role substitution.

- ⬜ **Redline Diff** _(stubbed)_: Three-pane document comparison showing insertions, deletions, and moved blocks across DOCX, PDF (with OCR fallback), markdown, and plain text. Accept or reject per change, with export back to the original format and a changebar PDF for circulation.

- ⬜ **Clause Library** _(stubbed)_: Searchable repository of reusable contract clauses tagged by jurisdiction, party position (favorable, neutral, hostile), risk score, and dependencies. Drag into a draft; version history per clause; AI-integrated suggestion of a clause variant for the current deal.

- ⬜ **Defined Terms Checker** _(stubbed)_: Scans a contract for capitalized defined terms, flags terms used but not defined, terms defined but not used, inconsistent capitalization, and circular definitions. Auto-generates a definitions section and cross-reference table.

- ⬜ **Case Law Search** _(stubbed)_: Full-text search over a local corpus of court opinions (Caselaw Access Project, CourtListener bulk) with citation parsing, parallel citation resolution, headnote extraction, and treatment flags (overruled, distinguished, followed, criticized).

- ⬜ **Statute Search** _(stubbed)_: Searchable local mirror of federal and state statutes, regulations, and treaties (US Code, CFR, state codes, EUR-Lex) with cross-reference traversal, section-history diffs across amendments, and effective-date awareness.

- ⬜ **Citation Formatter** _(stubbed)_: Converts between Bluebook, ALWD, McGill, OSCOLA, and neutral citation formats; pulls metadata from a case, statute, or article identifier and emits a properly formatted citation plus a pin cite stub.

- ⬜ **License Picker** _(stubbed)_: SPDX-aware open-source license selector that asks a short questionnaire (patent grant, copyleft scope, attribution, commercial use, network distribution) and writes the matching license text and SPDX identifier into the project, ready to commit as LICENSE.

- ⬜ **Privacy Policy Generator** _(stubbed)_: Builds a privacy policy from a checklist of data categories collected, purposes, processors, retention periods, and applicable regimes (GDPR, CCPA, PIPEDA, LGPD, PIPL). Outputs HTML, markdown, and a versioned diff against the previous published policy.

- ⬜ **Terms of Service Builder** _(stubbed)_: Assembles a Terms of Service, EULA, or acceptable-use policy from modules (arbitration, class-action waiver, liability cap, choice of law, IP, indemnification, data use). Flags clauses that are unenforceable in named jurisdictions and emits a per-jurisdiction variant.

- ⬜ **SBOM License Scanner** _(stubbed)_: Walks the project's dependency manifests (cargo, npm, pip, go.mod, conan, vcpkg) and emits a software bill of materials with SPDX license tags, copyleft incompatibility warnings, dual-license disambiguation, and an attribution NOTICE file.

- ⬜ **DMCA Toolkit** _(stubbed)_: Generates takedown notices, counter-notices, and a repeat-infringer log from a URL and an asserted work; tracks response deadlines, maintains the designated-agent record, and produces a sworn statement template.

- ⬜ **Patent Search** _(stubbed)_: Full-text search over a local mirror of USPTO and EPO grants and applications with CPC class browse, claim parsing, family resolution, and prior-art assembly into a report. AI-assisted novelty triage against a draft claim set.

- ⬜ **Trademark Screener** _(stubbed)_: Searches USPTO TESS, EUIPO, and WIPO Madrid databases for word and design marks, runs phonetic (Soundex, Metaphone) and visual similarity checks, classifies under Nice classes, and produces a clearance memo.

- ⬜ **Docket Calendar** _(stubbed)_: Case and deadline tracker that computes hearing, response, appeal, and statute-of-limitations dates from filing dates and applicable court rules (federal, state, local, ex-US). ICS export, per-matter reminders, and a chain-of-deadlines view.

- ⬜ **E-Signature** _(stubbed)_: Signs PDFs and contracts with a stored cryptographic identity (PAdES, eIDAS-style), embeds the signature certificate and audit trail (IP, timestamp, document hash, geolocation), and emits a signed PDF plus a sidecar JSON manifest. Operates entirely offline.

- ⬜ **Redaction Tool** _(stubbed)_: Irreversibly blacks out text and image regions in PDFs and images, strips XMP and EXIF metadata, flattens form fields and layers, and produces a redaction log coded per FOIA exemption, GDPR pseudonymization, or privilege type.

- ⬜ **Compliance Framework Mapper** _(stubbed)_: Maintains a control matrix mapping each control (encryption at rest, access review, breach notification, data minimization) across GDPR, CCPA, HIPAA, PCI DSS, SOC 2, ISO 27001, and NIST 800-53, with per-control evidence attachments and gap reporting.

- ⬜ **Export Control Screener** _(stubbed)_: Classifies a product or technology under ECCN, USML, and HTS codes, screens parties against OFAC SDN, BIS Entity and Denied Persons lists, and EU consolidated lists, and emits a screening record per transaction with re-screen triggers on list updates.

- ⬜ **Litigation Hold Manager** _(stubbed)_: Issues hold notices, tracks custodian acknowledgments, suspends retention policies on named data sources (mail, chat, file shares, repos), and logs collection, preservation, and release events for evidentiary defensibility. Bates-stamps and produces a privilege log on export.


#### KYC, identity, compliance verification

- ⬜ **MRZ Decoder** _(stubbed)_: Parses ICAO 9303 machine-readable zones for passports, visas, and TD1/TD2/TD3 cards; validates per-field and composite check digits and surfaces alignment or substitution errors with confidence per glyph.

- ⬜ **Document Classifier** _(stubbed)_: Identifies document type, issuing country, and template version from an image using a local catalog of known layouts plus an AI-integrated visual classifier fallback for unseen variants.

- ⬜ **ID OCR Pipeline** _(stubbed)_: Runs template-aware OCR across passport, driver license, and national ID layouts; extracts named fields (name, DOB, MRZ, document number, expiry, address) into a typed record with per-field confidence.

- ⬜ **PDF417 Barcode Reader** _(stubbed)_: Decodes AAMVA PDF417 driver license barcodes, Code128, QR, and Aztec symbologies into structured demographic fields; cross-checks decoded values against visible OCR text for tamper signal.

- ⬜ **Authenticity Inspector** _(stubbed)_: Runs tamper detection via Error Level Analysis, copy-move forensics, font-consistency checks, hologram region sampling, UV/IR layer review, and microprint inspection; renders a per-zone heatmap of suspect regions.

- ⬜ **ePassport Chip Reader** _(stubbed)_: Performs BAC, PACE, Chip Authentication, and Terminal Authentication against eMRTD chips; parses LDS data groups DG1 through DG15 and verifies the SOD signature chain against the CSCA master list.

- ⬜ **Face Matcher** _(stubbed)_: Computes a face embedding for the selfie and the document portrait, returns cosine and L2 similarity, and reports FMR/FNMR threshold guidance per ISO/IEC 19795 with bias slice diagnostics.

- ⬜ **Liveness Runner** _(stubbed)_: Drives passive (texture, depth-from-defocus, rPPG) and active (head turn, blink, prompt phrase) anti-spoof checks; scores presentation attack detection per ISO/IEC 30107-3 with attack-presentation logs.

- ⬜ **Watchlist Screener** _(stubbed)_: Searches consolidated OFAC SDN, UN, EU, UK HMT, and OpenSanctions lists plus PEP and RCA databases with locale-aware fuzzy name matching, DOB and nationality re-ranking, and reusable false-positive carve-outs.

- ⬜ **Adverse Media Scanner** _(stubbed)_: Aggregates open-web and licensed news sources for negative-news hits per subject, classifies by FATF predicate offense category, and dedupes near-identical reports across outlets using AI-integrated semantic clustering.

- ⬜ **Address Verifier** _(stubbed)_: Validates and standardizes addresses against USPS, Royal Mail PAF, AusPost, Canada Post, and UPU formats; geocodes to lat/lon and cross-checks against utility, credit header, and electoral roll sources where licensed.

- ⬜ **UBO Graph Explorer** _(stubbed)_: Pulls corporate registry filings from Companies House, OpenCorporates, SEC EDGAR, and GLEIF LEI records; traces beneficial ownership through layered entities and highlights >25% control paths, nominees, and circular structures.

- ⬜ **AML Risk Scorer** _(stubbed)_: Combines a policy DSL (country risk, occupation, product, channel, source-of-funds) with a gradient-boosted residual model; surfaces per-factor SHAP contributions to justify the score in a SAR or review note.

- ⬜ **Transaction Monitor** _(stubbed)_: Applies typology rules (structuring, smurfing, layering, rapid pass-through, sanctioned-corridor, mule-pattern) over a streaming ledger, opens cases above threshold, and drafts FinCEN SAR and goAML XML reports.

- ⬜ **Audit Trail Ledger** _(stubbed)_: Append-only hash-chained event log with per-event Merkle proofs, signed periodic snapshots, and WORM export; queryable by subject, operator, document, or decision outcome for regulator inspection.

- ⬜ **PII Redactor** _(stubbed)_: Masks faces, MRZ, signatures, and structured fields in images, PDFs, and JSON payloads; supports differential redaction with reversible vault entries gated by purpose-bound access tokens.

- ⬜ **Consent and Lawful-Basis Tracker** _(stubbed)_: Records GDPR Article 6 and 9 basis per data element, tracks consent versioning, expiry, and withdrawal events, and emits Records-of-Processing (ROPA) inventories for the DPO.

- ⬜ **DSAR Builder** _(stubbed)_: Assembles a GDPR or CCPA subject access response pack: locates all records by subject identifier across stores, redacts third-party data, and produces a portable JSON and PDF export with chain-of-custody manifest.


#### Documents and reading

- ✅ **PDF reader** _(in testing)_: opens PDF documents in an editor tab, rendered inline by the browser's native PDF viewer. Read-only currently.

- ⬜ **EPUB reader** _(stubbed)_: Reflowable reader for EPUB, MOBI, AZW3, and FB2 with adjustable typography, TOC navigation, bookmarks, and per-book reading state. Shares the same annotation store as the PDF reader.

- ⬜ **Comic and manga reader** _(stubbed)_: Page and strip viewers for CBZ, CBR, and PDF comics with right-to-left mode, double-page spreads, panel-by-panel zoom, and per-volume reading position.

- ⬜ **Document converter** _(stubbed)_: Routes documents between PDF, EPUB, DOCX, ODT, HTML, Markdown, LaTeX, and plain text via pandoc and format-specific backends. Falls back to model-driven structure recovery when a source is too malformed for deterministic conversion.

- ⬜ **OCR engine** _(stubbed)_: Runs Tesseract and neural OCR over scanned pages or images, producing searchable PDFs and plain text with per-token confidence. Supports multi-language passes and AI-integrated correction of low-confidence spans.

- ⬜ **Math and equation OCR** _(stubbed)_: Extracts LaTeX from cropped equations, screenshots, or hand-drawn formulas using a vision model. Renders the output back to verify and lets the user edit before insertion into notes.

- ⬜ **Layout analyzer** _(stubbed)_: Detects page regions (columns, headers, footers, captions, figures, tables, footnotes) and emits a structured tree. Drives the table, figure, and reflow extractors downstream.

- ⬜ **Table extractor** _(stubbed)_: Pulls tables out of PDF and scanned pages to CSV, TSV, or Markdown using rule-based detection with model-assisted cell merging and header inference.

- ⬜ **Figure and diagram extractor** _(stubbed)_: Crops images, charts, and diagrams from documents with their captions and references, exporting to PNG or SVG and indexing them for later search by caption text or visual similarity.

- ⬜ **Scan cleaner** _(stubbed)_: Deskews, despeckles, dewarps, and rebinarizes scanned pages, with batch presets for book scans, receipts, and whiteboards. Suggests cleanup parameters from a sample page using an AI quality estimator.

- ⬜ **Page operations** _(stubbed)_: Splits, merges, reorders, rotates, crops, and renumbers PDF pages with a thumbnail editor. Handles bookmark and outline preservation across operations.

- ⬜ **RAG chat** _(stubbed)_: Question-answering interface over a selected library subset using embeddings, BM25, and rerankers. Returns answers with span-level citations back to the source pages.

- ⬜ **Embedding pipeline** _(stubbed)_: Chunks documents by layout-aware boundaries, computes embeddings with a configurable model, and writes them to a local vector store. Supports incremental reindexing as the library changes.

- ⬜ **Reference resolver** _(stubbed)_: Looks up DOIs, ISBNs, arXiv IDs, PubMed IDs, and free-form titles against open metadata sources to fill in missing fields. Uses an LLM as a tie-breaker when multiple plausible matches are returned.

- ⬜ **Library catalog** _(stubbed)_: Calibre-style shelf for the local document collection with cover thumbnails, metadata columns, saved searches, virtual shelves, and duplicate detection by content fingerprint.

- ⬜ **Metadata editor** _(stubbed)_: Edits title, author, series, ISBN, language, cover, and custom fields across PDF, EPUB, and audiobook formats, with batch operations and AI-integrated inference of missing fields from the first pages of the file.

- ⬜ **Annotation store** _(stubbed)_: Central database of highlights, margin notes, bookmarks, and ink strokes, keyed to stable text anchors that survive reflow and reconversion. Exposes a search and timeline view across all books.

- ⬜ **Highlight exporter** _(stubbed)_: Dumps annotations to Markdown, Org, Obsidian, or Anki packages, grouped by book, tag, or color, with surrounding context windows and optional model-generated synthesis per highlight cluster.

- ⬜ **Document summarizer** _(stubbed)_: Produces per-section, per-chapter, and whole-document summaries at selectable depth, with extractive quote pulls alongside abstractive paraphrase. Caches summaries per content hash so they survive reopen.

- ⬜ **Glossary and index builder** _(stubbed)_: Identifies domain terms, named entities, and recurring symbols across a book or corpus, building a clickable glossary with first-mention pages and AI-integrated definitions grounded in the source text.

- ⬜ **Flashcard generator** _(stubbed)_: Generates cloze and Q-and-A cards from highlights, definitions, or entire sections, with answer evidence anchored to source pages. Exports to Anki, Mochi, or a built-in spaced repetition queue.

- ⬜ **Document diff** _(stubbed)_: Compares two versions of a document at text, structural, and visual layers, showing inline word diffs, moved blocks, and overlay deltas on rendered pages. Useful for paper revisions, contract redlines, and standards updates.

- ⬜ **Redactor** _(stubbed)_: Masks names, addresses, account numbers, and arbitrary regex or NER-detected spans, flattening the redactions into the page so the original text cannot be recovered. Previews redactions before commit.

- ⬜ **Text-to-speech narrator** _(stubbed)_: Reads documents aloud using a local TTS voice with adjustable rate, sentence highlighting in sync with playback, skip controls for footnotes and figures, and bookmark resume across sessions.


#### Academic and scholarly research

- ⬜ **Federated Paper Search** _(stubbed)_: Queries arXiv, PubMed, Semantic Scholar, OpenAlex, CrossRef, JSTOR, and Google Scholar concurrently with deduplication on DOI and title hash. Returns unified records with abstracts, venues, and full-text links.

- ⬜ **BibTeX/CSL Manager** _(stubbed)_: Local-first reference library with BibTeX, BibLaTeX, RIS, and CSL-JSON import/export, duplicate detection, and per-collection citation key schemes. Includes a CSL style editor with live preview against any selected entry.

- ⬜ **Paper Chunker and RAG Indexer** _(stubbed)_: Parses PDFs into semantically coherent chunks honoring section, equation, table, and figure-caption boundaries, then embeds them into a local vector store. Supports cross-paper retrieval with citation-grounded answers.

- ⬜ **AI Summarizer and TLDR Stack** _(stubbed)_: Generates structured summaries (problem, method, data, claims, limitations, threats to validity) at multiple altitudes from abstract-length to multi-page. Cross-links each claim back to the originating PDF location.

- ⬜ **Literature Review Tracker** _(stubbed)_: Project workspace for systematic reviews with PRISMA-style inclusion/exclusion logs, screening states, dual-reviewer reconciliation, and search-string version history. Exports the flow diagram and decision audit on demand.

- ⬜ **Lab and Method Comparator** _(stubbed)_: Side-by-side matrix of methods, datasets, hyperparameters, metrics, and reported results extracted from selected papers. Highlights inconsistencies, missing ablations, and benchmark contamination risks.

- ⬜ **Peer-Review Reading Queue** _(stubbed)_: Prioritized queue with SLA timers, conflict-of-interest checks against the user's coauthor graph, and per-section comment drafting. Includes a reviewer-style checklist generator tuned to the venue's guidelines.

- ⬜ **Journal Impact and Predatory Flag** _(stubbed)_: Combines JIF, SJR, SNIP, h5-index, and OpenAlex venue stats with predatory-publisher lists (Beall successor lists, DOAJ status, OASPA membership). Flags suspect APC patterns, fake metric badges, and hijacked-journal lookalikes.

- ⬜ **Equation and Notation Workbench** _(stubbed)_: Extracts equations from PDFs into editable LaTeX, resolves symbol definitions across the paper, and aligns notation between multiple sources into a unified glossary. Renders inline with MathJax/KaTeX and supports symbolic diff.

- ⬜ **Dataset and Code Provenance Tracker** _(stubbed)_: Resolves dataset and code artifacts cited in a paper to canonical sources (Zenodo, HuggingFace, OSF, GitHub, Figshare), records licenses, and snapshots versions. Diffs against the paper's claimed version when a release moves.

- ⬜ **Reproducibility Sandbox** _(stubbed)_: Spins up a containerized environment from a paper's code repo, pinning Python/R/Julia toolchains, seeds, and dataset hashes. Runs the headline experiment and reports deltas against the paper's reported numbers.

- ⬜ **Annotation and Margin-Notes Layer** _(stubbed)_: PDF and HTML annotation with highlights, threaded notes, tags, and backlinks across the library; notes are stored as portable JSON next to the source. AI suggests cross-references to your own prior notes when a passage echoes earlier reading.

- ⬜ **Author and Affiliation Disambiguator** _(stubbed)_: Resolves author identities across ORCID, OpenAlex, DBLP, and Semantic Scholar, merging duplicate profiles and tracking institutional moves over time. Surfaces collaboration networks and self-citation ratios per author.

- ⬜ **Retraction and Errata Watch** _(stubbed)_: Monitors Retraction Watch, CrossRef update notices, PubPeer threads, and venue corrigenda for every paper in the local library. Marks affected citations in drafts and exports an impact report.

- ⬜ **Writing and Citation Linter** _(stubbed)_: Lints manuscripts for missing citations, broken cross-references, undefined acronyms, inconsistent notation, and weak-claim/strong-claim mismatches against cited evidence. Integrates with LaTeX, Typst, Markdown, and Pandoc workflows.

- ⬜ **Grant and CFP Radar** _(stubbed)_: Tracks funding calls and conference CFPs from NSF, NIH, ERC, UKRI, DARPA, OpenReview, WikiCFP, and society mailing lists with topic-matched alerts. Builds a per-deadline checklist with required documents and eligibility flags.


#### Data, statistics, and ML

- ⬜ **Dataframe explorer** _(stubbed)_: Interactive tabular browser for CSV, Parquet, Arrow, JSON, and SQL result sets with column-type inference, filtering, sorting, pivoting, and group-by. Includes AI-integrated natural-language to pandas/Polars query translation against the loaded frame.

- ⬜ **Plot studio** _(stubbed)_: Composable charting surface supporting line, scatter, histogram, box, violin, heatmap, contour, ridge, and small-multiples plots over Matplotlib, Plotly, and Vega-Lite backends. AI-integrated chart-type suggestion from column dtypes and a stated analytical intent.

- ⬜ **Statistics console** _(stubbed)_: Runs descriptive statistics, hypothesis tests (t, Mann-Whitney, chi-square, KS, ANOVA, Kruskal-Wallis), effect sizes, confidence intervals, and power analyses on selected columns or groups. Reports assumptions checked and assumptions violated alongside each result.

- ⬜ **Regression workbench** _(stubbed)_: Fits OLS, GLM, mixed-effects, quantile, robust, and regularized (Ridge/Lasso/ElasticNet) regressions with full diagnostics: residual plots, leverage, VIF, Cook's distance, and partial dependence. AI-integrated narration of which diagnostics actually matter for the fit at hand.

- ⬜ **Bayesian sandbox** _(stubbed)_: PyMC and NumPyro front-end for specifying priors, running NUTS/HMC/SVI, and inspecting traces, R-hat, ESS, posterior predictive checks, and pair plots. Supports model comparison via LOO and WAIC.

- ⬜ **Time-series lab** _(stubbed)_: Decomposes series into trend/seasonal/residual, fits ARIMA, SARIMAX, ETS, Prophet, and state-space models, runs stationarity and cointegration tests, and produces forecasts with prediction intervals. AI-integrated model-class recommendation based on ACF/PACF and seasonality signatures.

- ⬜ **Survival analysis** _(stubbed)_: Kaplan-Meier curves, Cox proportional hazards, Aalen additive, and accelerated failure time models with log-rank tests, Schoenfeld residuals, and time-varying covariates.

- ⬜ **Causal inference toolkit** _(stubbed)_: DoWhy/EconML-style interface for DAG specification, identification, estimation (IPW, doubly robust, double ML, synthetic control, regression discontinuity), and sensitivity analysis. AI-integrated DAG drafting from a written problem description.

- ⬜ **Experiment tracker** _(stubbed)_: Local MLflow-compatible store for runs, params, metrics, artifacts, and model versions with side-by-side run comparison and parallel-coordinates plots over the hyperparameter space.

- ⬜ **Hyperparameter sweeper** _(stubbed)_: Runs grid, random, Bayesian (Optuna), Hyperband, and population-based searches against any training script with early stopping, pruning, and live Pareto-front visualization.

- ⬜ **AutoML pipeline** _(stubbed)_: Generates feature pipelines and model ensembles over tabular data using PyCaret/AutoGluon/TPOT backends, with leakage detection and target-leakage warnings. AI-integrated narration of why the chosen pipeline beat the others.

- ⬜ **Feature store** _(stubbed)_: Versioned offline/online feature definitions with point-in-time correct joins, freshness tracking, and lineage from raw source to training example.

- ⬜ **Dataset curator** _(stubbed)_: Sample-level review surface for image, audio, text, and tabular datasets with labels, splits, dedup (MinHash/SimHash/embedding), PII scan, and class-balance views. AI-integrated outlier and mislabel surfacing using model disagreement.

- ⬜ **Labeling studio** _(stubbed)_: Annotation interface for classification, NER, span, bbox, polygon, keypoint, and audio segment tasks with inter-annotator agreement metrics. AI-integrated pre-labeling and active-learning sample selection.

- ⬜ **Data quality profiler** _(stubbed)_: Produces per-column distributions, missingness patterns, cardinality, drift vs. a reference snapshot, and Great-Expectations-style assertions. Flags schema changes, outliers, and constant or near-constant columns.

- ⬜ **Vector index lab** _(stubbed)_: Build, inspect, and query FAISS, HNSW, ScaNN, and LanceDB indexes over arbitrary embedding columns with recall/latency tradeoff curves and ANN-vs-exact comparison.

- ⬜ **Embedding atlas** _(stubbed)_: UMAP/t-SNE/PCA/PaCMAP projections of embedding spaces with lasso selection, cluster labels, nearest-neighbor inspection, and cross-modal projection (text-to-image, audio-to-text). AI-integrated cluster naming from sample contents.

- ⬜ **Model browser** _(stubbed)_: Hugging Face Hub front-end with offline cache, model card preview, license/quant/size filters, dependency check, and one-click pull to local. Flags share-alike or restricted licenses against the project's PD/permissive policy.

- ⬜ **Fine-tuning runner** _(stubbed)_: Configurable LoRA/QLoRA/DoRA/full-finetune launcher over transformers, axolotl, and unsloth backends with VRAM estimation, dataset format validation, and live loss/grad-norm plots. AI-integrated learning-rate and rank suggestion from dataset shape and target model.

- ⬜ **Quantization bench** _(stubbed)_: Convert checkpoints to GGUF, AWQ, GPTQ, EXL2, and bitsandbytes formats and benchmark perplexity, latency, throughput, and VRAM at each quant level on a held-out eval set.

- ⬜ **Eval harness** _(stubbed)_: Runs MMLU, HumanEval, GSM8K, BIG-Bench, MT-Bench, IFEval, and user-defined rubric evals against any local or routed model with per-task breakdown and confidence intervals. Supports LLM-as-judge with bias controls.

- ⬜ **Red-team console** _(stubbed)_: Adversarial-prompt and jailbreak battery, refusal-rate measurement, and abliteration verification suite specifically aimed at confirming an abliterated model behaves as claimed across categories.

- ⬜ **RAG builder** _(stubbed)_: End-to-end retrieval pipeline composer covering chunkers, embedders, rerankers, hybrid (BM25 + dense) retrieval, and generation with per-stage trace inspection and retrieval-quality metrics (nDCG, MRR, hit@k).

- ⬜ **Prompt lab** _(stubbed)_: Side-by-side prompt variant runner with templating, structured-output schemas, temperature/top-p sweeps, and regression tests against a saved golden set. AI-integrated prompt-rewrite suggestions from failure patterns.

- ⬜ **Interpretation pipeline inspector** _(stubbed)_: Step-by-step view of the explicit interpretation pipeline used to route prompts through abliterated models, exposing every transform, classifier verdict, and routing decision with replay and per-stage override.

- ⬜ **Activation explorer** _(stubbed)_: Captures hidden states and attention patterns from local models, projects them, and supports probing, SAE feature browsing, and steering-vector authoring for mechanistic interpretability work.

- ⬜ **Synthetic data generator** _(stubbed)_: Produces synthetic tabular (SDV, CTGAN, copulas), text (template + LLM), and image (diffusion) datasets with fidelity, utility, and privacy (membership-inference) metrics against a real reference set.

- ⬜ **Image dataset toolkit** _(stubbed)_: Bulk crop, resize, dedup (perceptual hash), color-space convert, EXIF strip, and caption (BLIP/LLaVA) over image folders, with class-balance and resolution-distribution reports.

- ⬜ **Audio dataset toolkit** _(stubbed)_: Voice-activity detection, diarization, transcription (Whisper), loudness normalization, and segment extraction over audio corpora with per-segment quality scoring.


#### History and archival research

- ⬜ **Timeline composer** _(stubbed)_: Builds multi-track chronologies with fuzzy date ranges, regnal/era dating, and confidence intervals. Renders parallel tracks for polities, persons, and events; exports to TimelineJS or static SVG.

- ⬜ **Calendar converter** _(stubbed)_: Converts dates across Julian, Gregorian, Hijri, Hebrew, Coptic, Byzantine AM, French Republican, Mayan Long Count, and regnal years. Handles the 1582 cutover region-by-region and old-style year-start conventions.

- ⬜ **Paleography HTR** _(stubbed)_: Handwritten text recognition for manuscripts using local models in the Transkribus/eScriptorium tradition. Supports line segmentation, custom model training on user transcriptions, and confidence overlays per character.

- ⬜ **Diplomatic transcription editor** _(stubbed)_: TEI-conformant editor for diplomatic and semi-diplomatic transcriptions with brevigraph expansion, sigla, deletions, interlineations, and damage markup. Side-by-side with IIIF facsimile and zone-linked text.

- ⬜ **Critical apparatus builder** _(stubbed)_: Collates variant readings across witnesses and generates a positive or negative critical apparatus. Supports stemma codicum visualization and conjectural emendation tracking.

- ⬜ **IIIF workspace** _(stubbed)_: Loads IIIF manifests from any compliant repository, supports deep zoom, multi-image comparison in a Mirador-style grid, and persists annotations as W3C Web Annotations linked to canvases.

- ⬜ **Archival finding-aid browser** _(stubbed)_: Parses and navigates EAD3 finding aids and EAC-CPF authority records. Renders the hierarchical fonds/series/file/item tree with scope notes and links to digital surrogates.

- ⬜ **Citation and footnote formatter** _(stubbed)_: Generates Chicago notes-bibliography, Turabian, MLA, and discipline-specific archival citations (box/folder/item, shelfmark, MS conventions). Imports CSL styles and produces clean BibTeX/CSL-JSON output.

- ⬜ **Prosopography database** _(stubbed)_: Tracks persons, roles, offices, and relationships with explicit uncertainty, multiple attestations per assertion, and source-grounded provenance per claim. Models the Factoid Prosopography pattern used by PBW and PASE.

- ⬜ **Historical gazetteer** _(stubbed)_: Resolves place names across periods against Pleiades, GeoNames, World Historical Gazetteer, and Wikidata. Handles renamings, boundary shifts, and ambiguous toponyms; emits stable Linked Places URIs.

- ⬜ **Map georectifier** _(stubbed)_: Warps historical maps onto a modern basemap using control points, in the MapWarper/Allmaps tradition. Produces tile sources for overlay and exports GCPs plus a transformation matrix for reproducibility.

- ⬜ **Authority record reconciler** _(stubbed)_: Links extracted entities to VIAF, LCNAF, GND, ULAN, and Wikidata QIDs with batch reconciliation, candidate scoring, and AI-integrated disambiguation from surrounding context.

- ⬜ **Historical NER tagger** _(stubbed)_: Named entity recognition tuned for archaic orthography, abbreviations, and code-switching across Latin, Greek, Old English, and early modern vernaculars. Outputs CoNLL or standoff annotations with model confidence.

- ⬜ **Correspondence network analyzer** _(stubbed)_: Builds sender-recipient and mention networks from letter corpora, computes centrality and community structure, and renders force-directed layouts in the Gephi/Cytoscape mode. Imports CMIF (Correspondence Metadata Interchange Format).

- ⬜ **Provenance chain tracker** _(stubbed)_: Records chain-of-custody for documents and objects from creation through repositories and private collections. Uses PROV-O/W3C PROV serialization and flags gaps, contradictions, and suspect transfers.

- ⬜ **Regesta builder** _(stubbed)_: Produces structured summaries (regesta) of charters and diplomatic documents with parties, dispositive act, date clause, witnesses, and dating elements. Templates follow Bohmer and MGH conventions.

- ⬜ **Historical units converter** _(stubbed)_: Converts pre-metric weights, measures, currencies, and prices across periods and polities, with purchasing-power equivalence via wage-basket and silver-content series. Cites the underlying source table per conversion.

- ⬜ **Watermark and codicology assistant** _(stubbed)_: Matches paper watermarks against Bernstein/Briquet/Piccard repertories from a traced or photographed image, and records quire structure, ruling pattern, and binding notes per manuscript.

- ⬜ **Genealogy and GEDCOM editor** _(stubbed)_: Edits family trees with per-fact source citations, conflicting evidence handling, and DNA match notes. Imports and exports GEDCOM 7 and GEDCOM-X.

- ⬜ **OCR ground-truth manager** _(stubbed)_: Curates page-aligned ground truth in PAGE-XML and ALTO for training HTR/OCR models. Tracks transcriber, normalization rules, and inter-annotator agreement on held-out lines.


#### Ancient languages and paleography

- ⬜ **Polyglot Lexicon** _(stubbed)_: Offline dictionaries for Latin (Lewis & Short, Gaffiot), Greek (LSJ, Middle Liddell), Hebrew/Aramaic (BDB, HALOT-style entries), Sanskrit (Monier-Williams), Akkadian (CAD glosses), Sumerian (ePSD), and Coptic (Crum). Cross-references cognates and provides reverse-lookup from English glosses.

- ⬜ **Morphological Parser** _(stubbed)_: Parses inflected wordforms across Latin, Greek, Hebrew, Sanskrit, and Akkadian, returning lemma, part of speech, and full morphological tags. Integrates Morpheus, CLTK, and Stanza models locally with confidence scores for ambiguous forms.

- ⬜ **Declension and Conjugation Tables** _(stubbed)_: Generates complete paradigms for any lemma: nominal declensions, verbal conjugations, participles, and irregular stems. Supports Latin five declensions, Greek thematic/athematic, Hebrew binyanim, Sanskrit ten classes, and Akkadian G/D/Š/N stems with side-by-side comparison.

- ⬜ **Paleographic OCR** _(stubbed)_: Transcribes scanned manuscripts, papyri, ostraca, and inscriptions using script-specific models for uncial, minuscule, square Hebrew, Demotic, Greek majuscule, and Latin cursive hands. Outputs Unicode plus diplomatic XML with confidence heatmaps over the glyph image.

- ⬜ **Cuneiform Sign Reader** _(stubbed)_: Identifies cuneiform signs from photographs and hand-copies, returning sign list numbers (MZL, LAK, ePSD), readings, and period (Old Babylonian, Neo-Assyrian, Ur III). Supports both Akkadian syllabic and Sumerian logographic interpretations with normalization.

- ⬜ **Hieroglyph Composer** _(stubbed)_: Builds Egyptian hieroglyphic text using Gardiner sign codes with quadrant layout (JSesh-style), Manuel de Codage input, and reverse search by shape. Renders vertical or horizontal, right-to-left or left-to-right, with transliteration and translation pairing.

- ⬜ **Glyph and Script Palette** _(stubbed)_: Searchable palette for every script in scope (Greek, Hebrew, Syriac, Coptic, Devanagari, cuneiform, hieroglyphic, Linear B, runic, Ogham, Phoenician, Ugaritic) with diacritics, breathing marks, vowel points, cantillation, and combining sequences. Inserts at cursor with normalization to NFC/NFD.

- ⬜ **IPA Reconstruction Lab** _(stubbed)_: Reconstructs probable pronunciations for attested and unattested forms using comparative method rules: PIE, Proto-Semitic, Proto-Indo-Iranian, and intermediate stages. Outputs IPA with stress, vowel length, and tonal accent, plus audio synthesis where periodization permits.

- ⬜ **Critical Apparatus Editor** _(stubbed)_: Authors TEI-conformant critical editions with sigla management, variant readings, conjectures, and lemma-keyed apparatus entries. Renders the Leiden conventions for papyrology (square brackets, dots under, underdot) and outputs print-ready PDF via XeLaTeX.

- ⬜ **Parallel Corpus Aligner** _(stubbed)_: Aligns ancient text with translations and other versions at the word, phrase, or verse level: Hebrew/LXX/Vulgate, Sanskrit/Pali, Akkadian/Sumerian bilinguals, Greek/Coptic. Supports concordance search and lemma-based queries across aligned layers.

- ⬜ **Meter and Prosody Scanner** _(stubbed)_: Scans Latin and Greek verse (dactylic hexameter, elegiacs, iambics, lyric meters), Sanskrit chandas (anushtubh, tristubh, jagati), and Hebrew biblical poetry parallelism. Marks longs, shorts, caesurae, and ictus with explanations of resolutions and substitutions.

- ⬜ **Epigraphy Workbench** _(stubbed)_: Handles inscriptions with EpiDoc tagging: lacunae, restorations, abbreviations, ligatures, and editorial sigla. Indexes by find-spot, date, and corpus (CIL, IG, KAI, RIB) with map view and squeeze image overlay.

- ⬜ **Cognate and Etymology Tracer** _(stubbed)_: Traces a word back through attested stages and reconstructed proto-forms with regular sound correspondences shown step by step. Links to Indo-European (LIV, NIL), Semitic (SED), and Dravidian etymological databases bundled locally.

- ⬜ **Manuscript Image Annotator** _(stubbed)_: IIIF-compatible viewer for high-resolution manuscript folios with deep zoom, region selection, and layered annotations for transcription, codicology, and decoration. AI-assisted line and column segmentation accelerates initial markup; corrections feed back into the OCR model.

- ⬜ **Translation Drafting Assistant** _(stubbed)_: Produces draft English translations of ancient passages with morphological tooltips, alternative renderings, and citations to standard reference grammars (Smyth, Gildersleeve, Gesenius, Whitney, GAG). Flags hapax legomena, textual cruces, and idioms for human review.

- ⬜ **Sign and Glyph Frequency Profiler** _(stubbed)_: Computes character, sign, lemma, and collocation frequencies across selected corpora (PHI, TLG, Perseus, ETCSL, ORACC, GRETIL, Sefaria). Generates Zipf plots, hapax lists, and stylometric fingerprints for authorship and dating questions.


#### Ancient texts and manuscript studies

- ⬜ **Stemma Codicum Builder** _(stubbed)_: Constructs and visualizes manuscript transmission trees using Lachmannian errors, phylogenetic methods (PAUP, NeighborNet), and contamination-aware networks. Lets the user pin known archetypes and hyparchetypes, then propagates implied relationships.

- ⬜ **Parallel Versions Browser** _(stubbed)_: Aligns MT, LXX (Rahlfs and Göttingen), Vulgate, Peshitta, Targumim, and SP at verse and word level with synchronized scrolling. Surfaces quantitative and qualitative differences and links to underlying manuscript witnesses.

- ⬜ **Scribal Hand Classifier** _(stubbed)_: Identifies and clusters scribal hands across folios using AI-assisted paleographic feature extraction (letter morphology, ductus, ligature inventory). Outputs dated comparisons against the Schøyen, BL Digitised Manuscripts, and Vatican reference corpora.

- ⬜ **Lacuna Reconstructor** _(stubbed)_: Proposes restorations for damaged papyri and parchment lacunae using context-conditioned language models trained on Greek, Latin, Coptic, Hebrew, and Aramaic corpora. Reports candidate fills with letter-width fit, parallels in extant witnesses, and confidence intervals.

- ⬜ **Marginalia Notebook** _(stubbed)_: Captures glosses, nota bene marks, maniculae, and corrector hands as first-class annotations anchored to image regions and TEI loci. Supports cross-manuscript queries (e.g., all scholia on Iliad 1.1) and ownership-mark provenance chains.

- ⬜ **Codicology Inspector** _(stubbed)_: Models quire structure, hair/flesh side patterns, ruling schemes, prickings, and pagination/foliation anomalies. Generates collation formulas (Gregory rule, Quire diagrams) and flags suspected losses or insertions.

- ⬜ **Multispectral Image Stack** _(stubbed)_: Loads UV, IR, and visible-light captures of palimpsests and damaged folia with per-band registration and pseudocolor enhancement. Includes ink-recovery filters tuned for iron-gall, carbon, and minium pigments.

- ⬜ **Abbreviation and Ligature Expander** _(stubbed)_: Expands Tironian notes, nomina sacra, suspension and contraction abbreviations, and scribal ligatures using script- and period-specific rule sets. Round-trips between abbreviated and expanded forms while preserving line and letter counts.

- ⬜ **Patristic Citation Tracer** _(stubbed)_: Indexes scriptural and classical citations inside Church Fathers, rabbinic literature, and medieval florilegia. Links each citation back to candidate textual forms and helps date or localize textual variants by their reception.

- ⬜ **Orthographic Normalizer** _(stubbed)_: Normalizes itacism, iotacism, scriptio continua, nu-movable, plene/defective spellings, and matres lectionis without destroying the diplomatic transcription. Keeps a reversible mapping layer so collation can run on normalized forms while display preserves the original.

- ⬜ **Lectionary and Pericope Mapper** _(stubbed)_: Maps Gospel, Pauline, prophetologion, and synagogue lectionary cycles to the source texts they cite. Resolves incipit/explicit boundaries and reconciles Byzantine, Jerusalem, and Coptic liturgical orders.

- ⬜ **Provenance and Dating Estimator** _(stubbed)_: Estimates date and scriptorium of a manuscript from script style, decoration, parchment preparation, and ink chemistry inputs. Combines radiocarbon priors with paleographic and codicological evidence into a posterior date range.

- ⬜ **Concordance and Lexicon Workbench** _(stubbed)_: Generates lemmatized concordances and KWIC views across Greek, Hebrew, Aramaic, Latin, Coptic, Syriac, and Ge'ez corpora. Integrates LSJ, BDAG, HALOT, Lewis and Short, Sokoloff, and Crum with morphological tagging.

- ⬜ **Collation Engine** _(stubbed)_: Performs automated witness collation (CollateX-style) with configurable tokenization for unpointed Hebrew, accented Greek, and unspaced majuscule. Emits variant graphs, alignment tables, and TEI apparatus seeded for editorial review.


#### Religious studies and comparative religion

- ⬜ **Cross-Tradition Concordance** _(stubbed)_: Indexes canonical scriptures (Tanakh, Bible variants, Quran, Tipitaka, Vedas, Avesta, Guru Granth Sahib, Tao Te Ching, Analects) into a unified searchable corpus. Supports lemma, root, and theme queries with side-by-side parallel views across traditions.

- ⬜ **Polyglot Scripture Viewer** _(stubbed)_: Aligns verse-keyed source texts (Hebrew, Koine Greek, Classical Arabic, Sanskrit, Pali, Ge'ez, Latin, Church Slavonic) with multiple translations and morphological tagging. Hover reveals lexicon entries, variant readings, and manuscript witnesses.

- ⬜ **Liturgical Calendar Engine** _(stubbed)_: Computes feast days, fasts, observances, and moveable dates across Gregorian, Julian, Hebrew, Hijri, Bikram Sambat, Buddhist, and Bahai calendars. Supports recurrence queries, sect-specific variants, and ICS export.

- ⬜ **Theological Lexicon** _(stubbed)_: A cross-referenced glossary of doctrinal, mystical, and ritual terminology (e.g., kenosis, tawhid, sunyata, halakhah, dharma, theosis) with etymology, tradition-of-origin, and conceptual neighbors. AI assists by surfacing analogous concepts across traditions when prompted.

- ⬜ **Ritual Database** _(stubbed)_: Structured records of rites (liturgies, sacraments, pujas, salat, sadhana, sacrifice) with components: officiants, instruments, gestures, recitations, temporal constraints, and required objects. Filterable by tradition, life-cycle stage, or functional category.

- ⬜ **Sacred Geography Atlas** _(stubbed)_: GIS layer of pilgrimage sites, holy cities, sanctuaries, monasteries, and contested loci with diachronic boundaries of religious polities. Includes route data for hajj, Camino, Shikoku 88, Char Dham, and historical missionary networks.

- ⬜ **Exegesis Notebook** _(stubbed)_: A pericope-anchored workspace for verse-by-verse commentary that links primary text, traditional commentators (Rashi, Ibn Kathir, Aquinas, Sankara, Buddhaghosa), and the user's notes. AI drafts comparative readings on request and flags reasoning lineage.

- ⬜ **Manuscript Variant Apparatus** _(stubbed)_: Critical apparatus tooling for textual criticism: collates witnesses (Masoretic, LXX, DSS, Vulgate, Peshitta, Quranic qira'at, Buddhist recensions) with conjectural emendation notes. Renders Nestle-Aland-style apparatus and stemma diagrams.

- ⬜ **Denominational Comparator** _(stubbed)_: Side-by-side doctrinal matrices for sects and schools (e.g., Sunni/Shia/Ibadi madhhabs, Theravada/Mahayana/Vajrayana, Orthodox/Catholic/Protestant/Oriental, Vaishnava/Shaiva/Shakta/Smarta). Compares positions on canon, ritual, ecclesiology, soteriology, and authority.

- ⬜ **Prayer and Liturgy Translator** _(stubbed)_: Source-aware translator for liturgical texts that preserves meter, refrain structure, and theological loading. Handles common forms: psalmody, sutra recitation, mantra, doxology, kaddish, dua, and supports phonetic transliteration for non-reading users.

- ⬜ **Comparative Mythology Graph** _(stubbed)_: Knowledge graph of deities, archetypes, cosmogonies, eschatons, and culture heroes with typed edges for cognates, syncretisms, and influence vectors. Supports Dumezilian, Eliadean, and historicist queries.

- ⬜ **Canon and Apocrypha Tracker** _(stubbed)_: Tabulates inclusion status of every candidate text across canons (Catholic, Protestant, Ethiopian Orthodox, Tewahedo, rabbinic, Samaritan, Mandaean, various Buddhist canons, Daoist Daozang). Includes council decisions and dating ranges.

- ⬜ **Hymnody and Chant Archive** _(stubbed)_: Searchable repository of hymns, chants, qawwali, kirtan, gregorian neumes, Byzantine ekphonetic notation, and shomyo with notation, audio, and lyric translations. Indexes by mode, occasion, and tradition.

- ⬜ **Comparative Ethics Engine** _(stubbed)_: Queries normative positions across traditions on a given case (war, usury, sexuality, dietary law, end-of-life, environmental stewardship) with primary citations from each. AI synthesizes the spread of views and surfaces dissenting minorities.

- ⬜ **Mystical Praxis Codex** _(stubbed)_: Catalog of contemplative and ascetic techniques (hesychasm, dhikr, zazen, vipassana, kabbalistic meditation, hatha yoga, Ignatian exercises) with prerequisites, stages, and primary-source instructions. Cross-references phenomenological reports.

- ⬜ **Field Ethnography Recorder** _(stubbed)_: Local-first capture tool for participant-observation: timestamped notes, audio, photos, and geotags bound to ritual event schemas. Enforces an IRB-style consent and provenance ledger for sensitive material.

- ⬜ **Argument map editor** _(stubbed)_: Builds argument maps with premises, conclusions, attacks, and supports; imports Argdown syntax, exports to JSON-LD and SVG, and validates instantiations of Walton argument schemes.

- ⬜ **Formal logic prover** _(stubbed)_: Checks propositional, first-order, and modal arguments by dispatching to Prover9, Mace4, Z3, and a native tableaux engine; renders natural-deduction proofs, countermodels, and truth tables.

- ⬜ **Fallacy detector** _(stubbed)_: Scans prose arguments for informal fallacies (straw man, false dichotomy, ad hominem, motte-and-bailey) with AI-integrated pattern matching against a curated catalog; highlights offending spans and proposes reformulations.

- ⬜ **Manuscript variant collator** _(stubbed)_: Aligns multiple witnesses of a text in CollateX style and produces a variant graph; emits TEI parallel-segmentation markup and supports manual override of disputed alignments.

- ⬜ **Stemma builder** _(stubbed)_: Constructs manuscript family trees from variant data using Lachmannian, neighbor-joining, and RHM phylogenetic methods; visualizes contamination, lost archetypes, and bootstrap support.

- ⬜ **Concordance and KWIC** _(stubbed)_: Generates keyword-in-context indexes, frequency tables, and collocation profiles over user corpora; supports lemmatized queries and regex across Greek, Hebrew, Latin, Sanskrit, Pali, and Arabic.

- ⬜ **Morphological analyzer** _(stubbed)_: Parses ancient-language tokens to lemma, part-of-speech, and inflection using Perseus Morpheus, MorphHB, CLTK, and Buckwalter back-ends; flags ambiguities and proposes context-disambiguated readings.

- ⬜ **Parallel text viewer** _(stubbed)_: Displays aligned passages across versions and traditions (LXX / MT / Vulgate, Synoptic Gospels, Talmudic sugyot with Rashi and Tosafot, Quranic qirāʾāt); synchronizes scrolling and color-codes divergences.

- ⬜ **Lexicon lookup** _(stubbed)_: Queries domain lexica (LSJ, BDAG, HALOT, Jastrow, Monier-Williams, Lane, Du Cange) with sense disambiguation from surrounding context; cross-links each gloss to attestations in the loaded corpus.

- ⬜ **Comparative-religion timeline** _(stubbed)_: Plots texts, councils, schisms, founders, dynasties, persecutions, and translation movements across traditions on a synchronizable timeline; filters by region, language, tradition, and confidence of dating.

- ⬜ **Doxographic citation graph** _(stubbed)_: Maps influence and citation lineages among philosophers and theologians using PhilPapers, Diels-Kranz, and user-imported bibliographies; supports forward and backward traversal, community detection, and lineage queries.

- ⬜ **Ethics scenario explorer** _(stubbed)_: Runs canonical thought experiments (trolley variants, violinist, experience machine, ring of Gyges, original position) under selectable normative frameworks (utilitarian, Kantian, virtue, care, contractualist, divine command); records verdicts with reasons and saves comparative tables.

- ⬜ **Bayesian belief sandbox** _(stubbed)_: Models credences over hypotheses (theism vs. naturalism, miracle reports, authorship attributions) with priors and likelihood ratios; performs Jeffrey conditionalization on evidence and visualizes posterior shifts.

- ⬜ **Isnād analyzer** _(stubbed)_: Parses hadith chains of transmission, scores each narrator against rijāl databases, and computes overall ṣaḥīḥ / ḥasan / ḍaʿīf grading; visualizes the transmission network and identifies common links.

- ⬜ **Source-layer visualizer** _(stubbed)_: Overlays critical hypotheses on a text (JEPD for the Pentateuch, Q / M / L for the Synoptics, Markan priority variants, redactional strata) with color-coded spans; supports user-defined source theories and side-by-side comparison.

- ⬜ **Sacred geography GIS** _(stubbed)_: Plots pilgrimage routes, holy sites, monasteries, episcopal sees, and council locations on Pelagios-linked maps; computes travel-time isochrones over reconstructed ancient road and sea networks.

- ⬜ **Iconography classifier** _(stubbed)_: Identifies saints, attributes, mudrās, asanas, and narrative scenes in images using an AI vision model fine-tuned on Iconclass, the Index of Medieval Art, and Buddhist and Hindu iconographic corpora; returns ranked Iconclass codes with provenance.


#### Mythology, folklore, and comparative narrative

- ⬜ **ATU Motif Index Browser** _(stubbed)_: Searchable, cross-referenced index of Aarne-Thompson-Uther tale types and Thompson motif numbers, with filters by region, motif element, and tale family. Supports tagging custom corpus entries with ATU codes for later aggregation.

- ⬜ **Pantheon Cross-Reference Atlas** _(stubbed)_: Side-by-side comparison grids of deities across cultures (Indo-European, Mesoamerican, West African, Polynesian, Sino-Tibetan, etc.), aligned by domain (sky, storm, trickster, underworld). Includes etymology trails and known syncretic identifications.

- ⬜ **Folk Tale Variant Browser** _(stubbed)_: Collation viewer for variants of a single tale type across collectors and regions (Grimm, Afanasyev, Calvino, Lang, Jacobs, El-Shamy). Highlights diffs in plot beats, character roles, and locale-specific substitutions.

- ⬜ **Oral History Recorder** _(stubbed)_: Local audio capture with timestamped transcript, speaker labeling, and informant metadata (age, location, language, consent status). AI-assisted transcription and translation runs locally; original audio is the canonical artifact.

- ⬜ **Narrative Structure Analyzer** _(stubbed)_: Annotates a text against Campbell's monomyth, Propp's 31 functions, Greimas actantial model, and Freytag pyramid simultaneously. Outputs beat alignment charts and flags missing or inverted functions.

- ⬜ **Mythography Database** _(stubbed)_: Local triple-store for entities (gods, heroes, places, objects, tales) with typed relations (parent-of, slays, transforms-into, cognate-with). Imports from Theoi, Encyclopedia Mythica dumps, and user-curated bibliographies.

- ⬜ **Comparative Etymology Tracer** _(stubbed)_: Tracks divine and heroic names across reconstructed proto-languages (PIE, Proto-Uralic, Proto-Bantu) with sound-law derivations. Cites Watkins, Mallory, Pokorny, and flags contested reconstructions.

- ⬜ **Tale Diffusion Mapper** _(stubbed)_: Plots geographic spread of tale types and motifs on a base map with chronological layers, trade routes, and migration overlays. Useful for testing diffusionist vs polygenesis hypotheses on a given motif.

- ⬜ **Ritual and Festival Calendar** _(stubbed)_: Cross-cultural calendar of feast days, seasonal rites, and liturgical cycles keyed to lunar, solar, and agricultural reckonings. Links each entry to associated myths, hymns, and ethnographic sources.

- ⬜ **Iconography Tagger** _(stubbed)_: Image library with attribute tagging (thunderbolt, caduceus, ankh, vajra, drum, twin-snake) and culture-of-origin metadata. Local vision model proposes attribute tags from uploaded artifact photos; tags are user-confirmed before commit.

- ⬜ **Primary Source Reader** _(stubbed)_: Annotated reader for canonical texts (Theogony, Eddas, Mahabharata, Popol Vuh, Kojiki, Kalevala, Epic of Gilgamesh) with parallel-column translations, line numbering, and inline philological notes.

- ⬜ **Hero Journey Composer** _(stubbed)_: Authoring workspace that scaffolds a narrative against a chosen structural template (monomyth, Propp, kishotenketsu, three-act). Tracks stage completion and surfaces structural gaps as the draft evolves.

- ⬜ **Trickster Taxonomy Viewer** _(stubbed)_: Specialized typology for trickster figures (Loki, Anansi, Coyote, Hermes, Sun Wukong, Eshu, Raven) cross-indexed by traits: shapeshifting, theft, boundary-crossing, culture-hero overlap, gender fluidity.

- ⬜ **Cosmogony Comparator** _(stubbed)_: Structured comparison of creation accounts: ex nihilo, world-egg, primordial dismemberment, earth-diver, emergence, world-parents. Aligns stages and lists exemplar traditions for each pattern.

- ⬜ **Symbol Concordance** _(stubbed)_: Cross-cultural concordance for recurring symbols (tree, serpent, twin, mountain, weaver, smith, threshold) with attested meanings per tradition. Links each symbol entry to motif index codes and corpus citations.

- ⬜ **Genealogy Graph** _(stubbed)_: Interactive graph of divine and heroic lineages with support for non-tree relations (multiple parentage, self-generation, reincarnation cycles, syncretic merges). Renders Hesiodic, Norse, Vedic, and Mayan family trees from the database.

- ⬜ **Performance Variant Tracker** _(stubbed)_: Logs successive retellings of the same tale by a single performer or across performers to study oral-formulaic variation. Computes lexical and structural drift between sessions using Parry-Lord style formula detection.

- ⬜ **Citation and Provenance Manager** _(stubbed)_: BibTeX-compatible reference store specialized for folklore: handles informant attribution, collection-date fields, archive call numbers, and rights status (public domain vs restricted ethnographic material).


#### Linguistics and conlanging

- ⬜ **IPA Transcriber** _(stubbed)_: Converts orthographic text to broad or narrow IPA using G2P models for dozens of languages, with side-by-side stress, syllable, and tone markup. AI-integrated fallback for proper nouns, code-switched spans, and out-of-vocabulary tokens.

- ⬜ **Phoneme Inventory Editor** _(stubbed)_: Builds and edits PHOIBLE-style consonant and vowel charts with feature matrices (place, manner, voicing, height, backness). Exports inventories to LaTeX, SVG, and machine-readable JSON for downstream sound-change and phonotactics tools.

- ⬜ **Sound Change Applier** _(stubbed)_: Runs ordered rewrite rules over a wordlist in the style of SCA2 and Lexurgy, with feature-bundle conditioning, sound classes, and cascading sound laws. Diffs each derivation stage and flags rules that bleed, feed, or counterfeed.

- ⬜ **Morphology Glosser** _(stubbed)_: Segments wordforms into morphemes and produces Leipzig-format interlinear glosses with morpheme alignment, category tags, and free translation lines. AI-integrated guessing of analysis from parallel example sentences.

- ⬜ **Syntax Tree Builder** _(stubbed)_: Draws constituency and dependency trees from bracketed strings, CoNLL-U, or natural-language input, with drag-to-reattach editing and labeled arcs. Renders to PDF, SVG, and qtree/tikz-dependency.

- ⬜ **Corpus Concordancer** _(stubbed)_: Loads plain text, TEI, or CoNLL corpora and runs KWIC search with regex, lemma, and POS filters, plus frequency lists, collocation scores (PMI, log-Dice, t-score), and keyness against a reference corpus.

- ⬜ **Etymology Explorer** _(stubbed)_: Looks up word histories from vendored Wiktionary and Etymonline dumps, drawing cognate and borrowing graphs across language families. AI-integrated hypothesis generation for missing intermediate forms with confidence flags.

- ⬜ **Finite-State Lab** _(stubbed)_: Builds, composes, and visualizes finite-state acceptors and transducers (FSA/FST) for morphology and phonology, with regex-to-automaton conversion, determinization, minimization, and derivative-based regex debugging.

- ⬜ **Dialect Atlas** _(stubbed)_: Plots lexical, phonetic, and syntactic variants on isogloss maps from datasets like WALS, ASJP, and the Linguistic Atlas Projects. Supports side-by-side regional comparison and clustering by feature similarity.

- ⬜ **Language Identifier** _(stubbed)_: Detects language and script from short or mixed-language input using n-gram and Unicode-block classifiers, with per-span tagging for code-switching and confidence scores. Bundles offline models for 200+ languages.

- ⬜ **Spectrogram Studio** _(stubbed)_: Praat-lite audio analyzer with waveform, wide- and narrow-band spectrograms, pitch and intensity tracks, formant overlays, and cursor-driven measurements. Annotates intervals and points on TextGrid tiers.

- ⬜ **Forced Aligner** _(stubbed)_: Aligns audio to a phoneme or word transcript using Montreal-Forced-Aligner-style acoustic models, producing TextGrid output with per-phone boundaries. Falls back to AI-integrated transcription when the script is missing or partial.

- ⬜ **Script & Romanization Converter** _(stubbed)_: Transliterates bidirectionally between native scripts and romanizations (ISO, ALA-LC, Hepburn, Pinyin, BGN/PCGN), with reversible mappings and per-character provenance. Highlights lossy substitutions.

- ⬜ **Unicode Inspector** _(stubbed)_: Decomposes a string into codepoints, grapheme clusters, normalization forms (NFC/NFD/NFKC/NFKD), bidi runs, and script/block metadata. Flags confusables, mixed-script spoofs, and zero-width controls.

- ⬜ **Conlang Workbench** _(stubbed)_: Integrated environment for constructed languages: phoneme inventory, phonotactic constraints, lexicon with custom fields, generative word forge, and chained sound-change diachronics. Exports a printable grammar and dictionary.

- ⬜ **Rhyme & Meter Scanner** _(stubbed)_: Finds perfect, slant, and consonance rhymes from pronouncing dictionaries (CMU, Espeak phonemes) and scans verse for iambic, trochaic, anapestic, and quantitative meter. Marks ictus, caesura, and enjambment.

- ⬜ **Treebank Viewer** _(stubbed)_: Loads Universal Dependencies, Penn Treebank, and SUD corpora, renders trees, and queries with Grew, TGrep2, or a built-in pattern language. Computes inter-annotator agreement and label distributions.

- ⬜ **Sequence Alignment Lab** _(stubbed)_: Pairwise and multiple alignment for cognate sets and dialect forms using Levenshtein, Needleman-Wunsch, and SCA (sound-class-based) algorithms with custom feature-weighted substitution matrices.

- ⬜ **Lemmatizer & POS Lab** _(stubbed)_: Runs and compares lemmatizers and POS taggers (spaCy, Stanza, UDPipe, Snowball stemmers) across languages on the same input, with confusion matrices and per-token disagreement highlighting.

- ⬜ **Phoneme Inventory Designer** _(stubbed)_: Builds consonant and vowel inventories via a clickable IPA matrix, tracks distinctive features, and flags gaps against typological norms (e.g., having /p t/ but no /k/). Exports the inventory as a feature table consumed by downstream tools.

- ⬜ **Phonotactics Editor** _(stubbed)_: Defines syllable templates (onset, nucleus, coda), sonority constraints, cluster restrictions, and stress/tone placement rules. Validates candidate words against the active phonotactic grammar and reports which constraint each rejection violated.

- ⬜ **Word Generator** _(stubbed)_: Produces plausible lexemes from the active phonology and phonotactics with weighted phoneme and syllable-shape frequencies. AI biasing nudges output toward a target aesthetic (harsh, melodic, agglutinative-looking) described in natural language.

- ⬜ **Lexicon Builder** _(stubbed)_: Stores dictionary entries with part of speech, IPA, glosses, semantic domains, etymology pointers, example sentences, and recorded audio. Searchable by gloss, phonetic shape, semantic field, or morphological template.

- ⬜ **Morphology Sketcher** _(stubbed)_: Defines inflectional and derivational templates, declension and conjugation paradigms, and morphophonemic sandhi rules. Auto-fills paradigm tables from stems and surfaces irregulars for manual override.

- ⬜ **Syntax Sandbox** _(stubbed)_: Configures word-order parameters, alignment (nominative-accusative, ergative-absolutive, active-stative, tripartite), agreement, and case marking. Renders constituency or dependency trees over sample sentences and checks them against the configured grammar.

- ⬜ **Interlinear Gloss Editor** _(stubbed)_: Composes Leipzig-conformant glossed examples with morpheme segmentation, gloss line, and free translation. Auto-suggests glosses from the lexicon and morphology layers, and re-glosses bulk corpora after rule edits.

- ⬜ **Script Designer** _(stubbed)_: Vector glyph editor for invented orthographies with stroke construction, ligatures, contextual variants, and OpenType feature authoring (cursive joining, positional forms, mark stacking). Exports UFO sources and compiled OTF/TTF fonts.

- ⬜ **Orthography Mapper** _(stubbed)_: Defines bidirectional phoneme-grapheme correspondences, romanization schemes, and transliteration tables between scripts. Renders any lexicon entry or text sample in the chosen orthography on demand.

- ⬜ **Comparative Method Workbench** _(stubbed)_: Aligns cognate sets across related languages, derives regular sound correspondences, and reconstructs proto-forms. Flags borrowings and chance resemblances against the established regularity baseline.

- ⬜ **Diachronic Family Tree** _(stubbed)_: Models a language family as a branching tree with dated splits, inherited sound-change cascades, and contact-induced borrowing arrows. Re-running the cascade re-derives every daughter lexicon from the proto-lexicon, keeping the family internally consistent.

- ⬜ **Conworld Linguistic Atlas** _(stubbed)_: Plots language and dialect regions on a world map with isoglosses, sprachbund boundaries, and contact zones. Each polygon links back to its lexicon, phonology, and demographic notes.

- ⬜ **Speech Synthesizer** _(stubbed)_: Renders conlang text as audio through a phoneme-to-acoustic pipeline (formant or neural backend) parameterized by the language's inventory and prosody. Useful for hearing the language and for stress-testing phonotactics by ear.

- ⬜ **Translation Workbench** _(stubbed)_: Edits parallel texts aligned at sentence and morpheme level with AI-assisted glossing, back-translation, and naturalness scoring. Flags lexicon gaps and proposes coinages consistent with the active phonology and morphology.


#### Speech, language, and translation

- ⬜ **Transcription studio** _(stubbed)_: ASR pipeline with word-level timestamps, speaker diarization, and per-token confidence scoring. Routes audio through the abliterated ASR stack and renders an editable transcript synced to a waveform and spectrogram.

- ⬜ **Subtitle editor** _(stubbed)_: Caption editor for SRT, VTT, ASS, and TTML with waveform timing, shot-change snapping, CPS and reading-speed checks, and style overrides. AI-integrated retiming from raw ASR output and segment merging suggestions.

- ⬜ **TTS bench** _(stubbed)_: Text-to-speech rendering with voice picker, SSML editor, and per-phoneme prosody control. Compares outputs from multiple TTS engines side by side with A/B blind listening.

- ⬜ **Voice cloning lab** _(stubbed)_: Few-shot voice cloning from short reference samples, with consent-tag metadata, speaker embedding inspection, and similarity scoring against ground truth. Exports cloned voices as portable speaker profiles.

- ⬜ **G2P and phonetic transcriber** _(stubbed)_: Grapheme-to-phoneme conversion with IPA, X-SAMPA, and language-specific phone sets. Per-project lexicon overrides and pronunciation variants.

- ⬜ **OCR studio** _(stubbed)_: Document and image OCR with layout reconstruction, multi-column handling, RTL and vertical-script support, and per-character confidence overlays. AI-integrated postcorrection of recognizer errors against a language model.

- ⬜ **Keyboard layout designer** _(stubbed)_: Editor for custom IME and keyboard layouts targeting non-Latin scripts, dead keys, combining marks, and stenographic chords. Exports to xkb, Karabiner, MSKLC, and ibus.

- ⬜ **Pronunciation coach** _(stubbed)_: Phoneme-level scoring of learner audio against reference pronunciation, with mispronunciation highlighting, minimal-pair drills, and articulator diagrams.

- ⬜ **Language and script identifier** _(stubbed)_: Per-paragraph and per-line detection of language and script, including code-switched spans, mixed-script flagging, and confidence per candidate.

- ⬜ **Terminology manager** _(stubbed)_: Termbase editor with definitions, domain tags, forbidden terms, and cross-language equivalents. Imports and exports TBX and pushes enforcement into the translation workbench.

- ⬜ **Translation memory** _(stubbed)_: TMX-backed segment store with fuzzy match retrieval, leverage statistics, penalty configuration, and project-scoped concordance search.

- ⬜ **Diarization and speaker tool** _(stubbed)_: Speaker turn segmentation, speaker embedding clustering, and named-speaker labeling persisted across sessions and projects.

- ⬜ **Speech denoiser** _(stubbed)_: Noise reduction, dereverberation, and bandwidth extension tuned for ASR and TTS input quality. AI-integrated model selection from a captured noise profile of the source clip.

- ⬜ **Dubbing studio** _(stubbed)_: Replaces source dialogue with translated speech timed to original mouth motion, with rate-stretch, take selection, and preservation of music-and-effects stems.

- ⬜ **Romanization and transliteration** _(stubbed)_: Bidirectional transliteration between scripts (Cyrillic, Arabic, Devanagari, Hebrew, CJK, Thai, Ge'ez, etc.) using configurable schemes such as BGN/PCGN, ISO 9, Hepburn, and Pinyin.

- ⬜ **Corpus and concordance browser** _(stubbed)_: Parallel and monolingual corpus loader with KWIC concordance, collocation statistics, n-gram frequency lists, and span-level query.

- ⬜ **Morphology and lemma analyzer** _(stubbed)_: Tokenization, lemmatization, POS tagging, dependency parsing, and morphological feature extraction across languages, with Universal Dependencies compatible output.

- ⬜ **SSML and prosody editor** _(stubbed)_: Visual editor for SSML markup with pitch, rate, emphasis, and break controls. Previews against the selected TTS engine with per-clause waveform inspection.

- ⬜ **Caption renderer** _(stubbed)_: Burns soft subtitles into video with font, position, and karaoke effect support, or muxes them as sidecar tracks. Handles RTL, ruby text, and vertical layout.

- ⬜ **Back-translation QA** _(stubbed)_: Round-trips translations through reverse MT and AI-integrated semantic comparison against the source to flag meaning drift, omissions, and hallucinated content.

- ⬜ **Lyric timing and karaoke** _(stubbed)_: Word-level and syllable-level lyric synchronization editor producing LRC, enhanced LRC, KAR, and ASS karaoke tags from audio plus text.

- ⬜ **Voice activity and segmenter** _(stubbed)_: VAD-driven slicing of long-form audio into utterance segments with silence trimming and overlap detection, used for ASR batching and TTS dataset preparation.

- ⬜ **Pronunciation lexicon** _(stubbed)_: Project-scoped dictionary of custom pronunciations, homograph disambiguation rules, and proper-noun overrides shared by ASR, TTS, and forced alignment.


#### Electronics and EDA

- ✅ **Mouser parts** _(in testing)_: detects part-search intent in the prompt, extracts a sanitized Mouser keyword (slug-style failures and unit shorthand are normalized), POSTs to the Mouser keyword API with the InStock filter, sorts cheapest-first, drops mismatched topologies (LDO vs. switching, and vice versa), and renders a markdown list with clickable in-tool links. Retries with a relaxed keyword on zero hits. Optionally writes the full list to a markdown file in the project and opens it in the editor.

- ⬜ **McMaster-Carr lookup** _(stubbed)_: look up mechanical hardware (fasteners, raw stock, fittings, tools) by part number or by description. Note: McMaster does not publish a free public REST API like Mouser; this works for browsing and pulling product pages/CAD files via the embedded server-side browser, with B2B PunchOut support for users with a McMaster business account.

- ⬜ **Scope, logic analyzer, and bench-instrument integration** _(stubbed)_: drivers and viewers for common USB instruments: DSView/sigrok logic analyzers, USB scopes (Hantek, Rigol, Siglent), VNA (NanoVNA, miniVNA), bench DMM, programmable power supplies, electronic loads. Streams captures into the project tree, replays them in the AI chat for analysis.

- ⬜ **Schematic capture** _(stubbed)_: Hierarchical schematic editor with netlist export, ERC checks, and library symbol management. AI-integrated symbol lookup from partial pin descriptions and auto-completion of common subcircuits (bias networks, decoupling, level shifters).

- ⬜ **PCB layout** _(stubbed)_: Multi-layer board editor with interactive routing, differential pairs, length matching, copper pours, and DRC. Imports netlists from the schematic tool and supports push-and-shove plus length-tuned serpentines.

- ⬜ **3D board viewer** _(stubbed)_: Renders the PCB stack with component bodies, solder mask, and silkscreen for visual inspection and mechanical fit checks. Exports STEP for enclosure CAD.

- ⬜ **Footprint editor** _(stubbed)_: Creates and edits land patterns with IPC-compliant courtyards, pad stacks, and assembly drawings. AI-integrated generation of footprints from datasheet PDFs and dimensioned drawings.

- ⬜ **Component library manager** _(stubbed)_: Unified parts database linking symbols, footprints, 3D models, datasheets, manufacturer part numbers, distributor stock, and lifecycle status. Deduplicates across libraries and tracks per-project BOMs.

- ⬜ **BOM and sourcing** _(stubbed)_: Generates bills of materials with live pricing, stock, and lead times from Octopart, LCSC, Mouser, and Digi-Key. Suggests pin-compatible alternates when a part is EOL or out of stock.

- ⬜ **SPICE simulator** _(stubbed)_: Ngspice-based mixed-signal simulator with transient, AC, DC sweep, noise, and Monte Carlo analyses. AI-integrated translation of plain-language goals ("keep ripple under 50mV at 2A") into testbench setups and parameter sweeps.

- ⬜ **Waveform viewer** _(stubbed)_: Plots simulation outputs and captured scope traces with cursors, math channels, FFT, eye diagrams, and jitter histograms. Side-by-side comparison of simulated vs measured signals.

- ⬜ **Filter designer** _(stubbed)_: Synthesizes passive and active filters (Butterworth, Chebyshev, Bessel, elliptic) from frequency-domain specs, with sensitivity analysis and E-series component snapping. AI-integrated suggestion of topology from a noise or interference profile.

- ⬜ **Signal integrity analyzer** _(stubbed)_: Field solver and IBIS-driven channel simulation for crosstalk, reflections, impedance discontinuities, and eye-mask compliance. Generates fix recommendations (stub length, termination, layer stackup).

- ⬜ **Power integrity analyzer** _(stubbed)_: PDN impedance solver with decoupling-capacitor optimization, IR drop maps, and rail transient analysis. Recommends cap value and placement to hit a target impedance curve.

- ⬜ **Thermal simulator** _(stubbed)_: Finite-element thermal solver for board and enclosure heat flow with copper-pour modeling, airflow, and heatsink libraries. Flags components near junction-temperature limits under worst-case load.

- ⬜ **EMC pre-compliance** _(stubbed)_: Near-field and conducted-emissions estimator with antenna factor calibration, plus checklists for FCC Part 15, CISPR 22, and CE marking. AI-integrated triage of failing harmonics to likely radiating loops on the board.

- ⬜ **Bench instrument bridge** _(stubbed)_: VISA/SCPI control of oscilloscopes, function generators, SMUs, DMMs, spectrum analyzers, VNAs, and electronic loads over USB, LAN, and GPIB. Records measurement sessions as replayable scripts.

- ⬜ **Datasheet reader** _(stubbed)_: OCR and structured extraction of datasheet PDFs into searchable parameter tables, pinouts, application circuits, and reflow profiles. AI-integrated Q&A grounded in the loaded datasheet with page citations.

- ⬜ **Logic analyzer and protocol decoder** _(stubbed)_: Captures from sigrok-compatible devices and decodes I2C, SPI, UART, CAN, LIN, USB, 1-Wire, Modbus, JTAG, and SWD. Cross-references decoded transactions with firmware source on hover.

- ⬜ **Firmware toolchain** _(stubbed)_: Cross-compilation, flashing, and on-chip debugging for AVR, ARM Cortex-M, RISC-V, ESP32, and PIC via OpenOCD, probe-rs, and avrdude. Integrates with the schematic so pin assignments stay in sync with code.

- ⬜ **HDL workbench** _(stubbed)_: Verilog, VHDL, SystemVerilog, and Amaranth editor with Yosys synthesis, nextpnr place-and-route, and simulation via Verilator or Icarus. Targets ICE40, ECP5, Gowin, and Xilinx 7-series.

- ⬜ **RF design suite** _(stubbed)_: Smith chart, matching-network synthesis, microstrip and stripline calculators, S-parameter math, and Touchstone file viewer. AI-integrated suggestion of matching topology from a measured complex impedance.

- ⬜ **Antenna designer** _(stubbed)_: Method-of-moments and FDTD solvers for wire, patch, and PCB-trace antennas with VSWR, radiation pattern, gain, and efficiency plots. Templates for common ISM-band designs.

- ⬜ **Magnetics designer** _(stubbed)_: Sizing and winding calculator for inductors, transformers, common-mode chokes, and flyback transformers with core-loss and saturation modeling. Pulls AL and saturation curves from a vendored ferrite database.

- ⬜ **Battery and power-budget planner** _(stubbed)_: Cell modeling (Li-ion, LiFePO4, NiMH, lead-acid, supercaps) with state-of-charge curves, cycle-life estimates, and load-profile runtime predictions. Sizes regulators, fuses, and protection circuits to match.

- ⬜ **Connector and cable designer** _(stubbed)_: Pinout planning for ribbon, JST, Molex, D-sub, and circular connectors with cable-loss, voltage-drop, and shielding calculators. Generates wiring diagrams and crimp-spec sheets.

- ⬜ **Test fixture and ICT planner** _(stubbed)_: Lays out bed-of-nails test points, pogo-pin grids, and boundary-scan chains. Generates a per-board test plan with pass/fail thresholds wired to the bench bridge.

- ⬜ **Reliability and FMEA** _(stubbed)_: MTBF prediction (MIL-HDBK-217, Telcordia, IEC 62380), derating reports, and failure-mode tables linked to schematic nets. AI-integrated drafting of FMEA rows from the schematic and known stress conditions.

- ⬜ **Reverse-engineering board scanner** _(stubbed)_: Aligns photos of top and bottom copper, traces nets, and reconstructs a schematic from a physical board. AI-integrated identification of unmarked SMD parts from package, markings, and surrounding context.

- ⬜ **Gerber and fab-output viewer** _(stubbed)_: Renders Gerber X2, Excellon, IPC-D-356 netlists, and pick-and-place files with layer toggling and DFM checks. Validates against fab capability profiles (JLCPCB, PCBWay, OSHPark) before sending.


#### Optics and photonics

- ⬜ **Sequential Lens Designer** _(stubbed)_: Sequential ray-tracing workbench for multi-element lens systems with surface editor, real ray and paraxial trace, spot diagrams, MTF, field curvature, distortion, and damped-least-squares optimization against user merit functions. AI-integrated starting-point synthesis from specs (focal length, f-number, field, wavelength band).

- ⬜ **Non-Sequential Ray Tracer** _(stubbed)_: Monte Carlo ray tracer for illumination, stray light, lightguides, and TIR optics with arbitrary CAD geometry, source models (point, extended, IES/LDT), BSDF surfaces, and detector irradiance/intensity maps.

- ⬜ **Gaussian Beam Propagator** _(stubbed)_: ABCD matrix propagation for Gaussian beams through arbitrary element chains, computing waist, divergence, Rayleigh range, mode matching, and astigmatism. Plots beam radius and phase along the optical axis with interactive element drag.

- ⬜ **Diffraction Calculator** _(stubbed)_: Scalar diffraction engine for Fraunhofer, Fresnel, and angular-spectrum propagation; computes PSF, MTF, encircled energy, and far-field patterns from arbitrary pupil amplitude/phase masks. Includes apodization presets and tilted-plane propagation.

- ⬜ **Zernike Wavefront Analyzer** _(stubbed)_: Decomposes measured or simulated wavefronts into Zernike (Noll/Fringe/OSA) and Seidel coefficients, with Strehl ratio, RMS/PV, and aberration-by-aberration PSF synthesis. Loads interferograms, Shack-Hartmann spot grids, and HDF5 wavefront cubes.

- ⬜ **Thin Film Coating Designer** _(stubbed)_: Multilayer transfer-matrix solver for reflectance, transmittance, absorptance, and group delay vs wavelength and angle, with material dispersion library (Sellmeier, Tauc-Lorentz) and needle/tunneling optimization to user targets (AR, HR, dichroic, beam splitter).

- ⬜ **Polarization Calculator** _(stubbed)_: Jones and Mueller calculus workbench for polarizers, retarders, rotators, depolarizers, and arbitrary anisotropic stacks; visualizes states on the Poincare sphere and computes degree of polarization, ellipticity, and orientation through user-built optical chains.

- ⬜ **Fiber Mode Solver** _(stubbed)_: Finite-difference vector mode solver for step-index, graded-index, photonic-crystal, and arbitrary-cross-section fibers, returning effective index, group/chromatic/PMD dispersion, mode field diameter, confinement, and bend loss.

- ⬜ **Laser Resonator Designer** _(stubbed)_: Designs linear and ring cavities with arbitrary mirror, lens, and gain-medium chains; computes stability (g1 g2), eigenmode size at each surface, longitudinal mode spacing, and thermal-lens-aware mode shifts. Plots stability diagrams and resonator round-trip Gaussian propagation.

- ⬜ **Interferogram Analyzer** _(stubbed)_: Loads single-frame, phase-shifted, or carrier-fringe interferograms; performs Fourier-transform fringe analysis, quality-guided phase unwrapping, tilt/piston removal, and Zernike fitting to produce calibrated surface or wavefront maps.

- ⬜ **Optical Bench Layout** _(stubbed)_: Top-down 2D and 3D scene editor for breadboard layouts with hole-grid snapping, mounted-component library (mirrors, mounts, posts, cages, kinematics), beam path visualization, and BOM export with vendor part numbers.

- ⬜ **Diffraction Grating Designer** _(stubbed)_: Computes blaze efficiency, dispersion, free spectral range, and resolving power for ruled, holographic, echelle, and volume phase holographic gratings via scalar and rigorous coupled-wave analysis across polarization and order.

- ⬜ **FDTD Nanophotonics Simulator** _(stubbed)_: Finite-difference time-domain solver for sub-wavelength structures (waveguides, resonators, metasurfaces, plasmonics) with PML boundaries, dispersive materials, mode and TFSF sources, and S-parameter, transmission, and near-to-far-field projection monitors.

- ⬜ **Camera Imaging Calculator** _(stubbed)_: Computes depth of field, hyperfocal distance, diffraction limit, angle of view, exposure value, equivalent focal length, and pixel-limited resolution for arbitrary sensor and lens combinations, with side-by-side format comparison.

- ⬜ **Radiometry Photometry Bench** _(stubbed)_: Converts between radiometric and photometric quantities (radiance, irradiance, luminance, illuminance, candela, lumen) under arbitrary spectra and observer functions; computes throughput, etendue, and source-detector link budgets with cosine and inverse-square geometry.

- ⬜ **Color Science Workbench** _(stubbed)_: Spectral-to-tristimulus pipeline for CIE 1931/2015 observers, chromaticity diagrams, CCT and CRI/TM-30, chromatic adaptation, ICC profile inspection, and rendering of arbitrary illuminant-reflectance-observer products.

- ⬜ **Spectrometer Designer** _(stubbed)_: Designs and analyzes Czerny-Turner, Echelle, Offner, and FT spectrometers, predicting spectral range, resolution, throughput, and stray light from grating, slit, detector, and folding-mirror choices. AI-integrated suggestion of layout topology from target resolving power and spectral band.

- ⬜ **Holography Designer** _(stubbed)_: Designs and simulates off-axis, in-line, and computer-generated holograms with iterative Fourier transform (Gerchberg-Saxton) and direct binary search algorithms; previews diffraction efficiency, speckle, and reconstruction under arbitrary illumination.

- ⬜ **Adaptive Optics Simulator** _(stubbed)_: Closed-loop AO simulator with Kolmogorov/von Karman turbulence screens, Shack-Hartmann or pyramid wavefront sensors, deformable mirror models, and controller (integral, LQG, predictive) design; reports residual wavefront error, Strehl, and temporal bandwidth.

- ⬜ **Nonlinear Optics Calculator** _(stubbed)_: Computes phase matching (angle, temperature, quasi-phase) for SHG/SFG/DFG/OPO in biaxial and uniaxial crystals, plus self-phase modulation, soliton number, B-integral, and supercontinuum estimates for pulse propagation through fibers and bulk media.

- ⬜ **Detector Noise Modeler** _(stubbed)_: End-to-end SNR and NEP calculator for CMOS, CCD, EMCCD, sCMOS, APD, PMT, and bolometer detectors including photon shot, read, dark, fixed-pattern, and excess noise, with integration-time and gain sweeps and limiting-magnitude or minimum-detectable-power outputs.


#### Physics and math

- ⬜ **Symbolic CAS** _(stubbed)_: Computer algebra system for simplification, expansion, factoring, solving equations, limits, derivatives, integrals, series, and matrix algebra. AI-integrated translation of natural-language problem statements into canonical symbolic expressions and step-by-step derivations.

- ⬜ **Numerical kernel** _(stubbed)_: Jupyter-style interactive notebook with NumPy/SciPy/Julia kernels, persistent variable state, and inline plot rendering. Routes natural-language requests to generated code blocks the user can review and re-run.

- ⬜ **Unit converter** _(stubbed)_: Bidirectional converter across SI, imperial, CGS, natural/Planck, astronomical, and engineering units with dimensional-analysis checks. Flags dimensionally inconsistent expressions before evaluation.

- ⬜ **Constants reference** _(stubbed)_: Searchable table of CODATA physical constants, particle properties, material constants, and astronomical data with uncertainties and citation back to source year.

- ⬜ **Equation library** _(stubbed)_: Browsable catalog of canonical equations across mechanics, EM, thermo, QM, relativity, and statistical mechanics, with variable definitions, assumptions, and one-click insertion into the symbolic or numerical workspace.

- ⬜ **Plotter 2D/3D** _(stubbed)_: Function, parametric, implicit, vector-field, contour, and surface plotting with interactive pan/zoom/rotate, LaTeX axis labels, and export to SVG/PNG/PGF. AI-integrated axis and styling suggestions from the data shape.

- ⬜ **ODE/PDE solver** _(stubbed)_: Numerical and symbolic solver for ordinary and partial differential equations with selectable stiffness-aware integrators (RK45, BDF, DOPRI, implicit), boundary-condition editor, and convergence diagnostics.

- ⬜ **Curve fitter** _(stubbed)_: Nonlinear least-squares, robust, and Bayesian regression with model library, residual diagnostics, and confidence/prediction bands. AI-integrated proposal of candidate functional forms from a scatter plot.

- ⬜ **Statistics workbench** _(stubbed)_: Descriptive statistics, hypothesis tests, distribution fitting, bootstrap/MCMC, and Bayesian inference with prior/posterior visualization and effect-size reporting.

- ⬜ **Linear algebra studio** _(stubbed)_: Matrix entry, decomposition (LU, QR, SVD, eigen, Cholesky), sparse solvers, condition-number inspection, and visualization of linear maps as transformations of basis vectors.

- ⬜ **Vector and tensor calculator** _(stubbed)_: Grad/div/curl/Laplacian in arbitrary coordinate systems, Christoffel symbols, Riemann/Ricci tensors, and index-notation manipulation for GR and continuum mechanics.

- ⬜ **Rigid-body simulator** _(stubbed)_: Time-domain simulation of constrained rigid bodies with contacts, joints, friction, and gravity. Exports trajectories, energy, and momentum traces; takes natural-language scene descriptions and emits an editable scene graph.

- ⬜ **FEM solver** _(stubbed)_: Mesh-based finite-element analysis for static and dynamic structural, thermal, and electromagnetic problems with material library, boundary-condition painter, and stress/strain/temperature field visualization.

- ⬜ **CFD playground** _(stubbed)_: 2D/3D incompressible and compressible flow solver (lattice-Boltzmann and finite-volume) with sketch-to-geometry input, turbulence model selection, and streamline/vorticity rendering.

- ⬜ **Ray-trace optics bench** _(stubbed)_: Sequential and non-sequential optical ray tracing for lens systems, mirrors, prisms, and fibers with aberration plots, spot diagrams, and MTF. AI-integrated starting-point design from a desired focal length, f-number, and field.

- ⬜ **Wave optics simulator** _(stubbed)_: Fourier optics, diffraction (Fraunhofer/Fresnel), interferometry, and beam-propagation method for Gaussian and structured beams with phase-mask editor.

- ⬜ **EM field solver** _(stubbed)_: FDTD and method-of-moments solver for time- and frequency-domain electromagnetics with antenna patterns, S-parameters, near/far-field plots, and PML boundary conditions.

- ⬜ **Circuit physics sim** _(stubbed)_: SPICE-compatible analog and mixed-signal simulator with transient, AC, noise, and Monte-Carlo analyses; integrates with the EM solver for parasitic extraction.

- ⬜ **Thermodynamics workbench** _(stubbed)_: State-equation evaluator (ideal gas, van der Waals, Peng-Robinson, IAPWS-IF97 for water/steam), PV/TS/HS diagram plotter, and cycle builder for Carnot, Rankine, Brayton, Otto, and Diesel.

- ⬜ **Heat-transfer solver** _(stubbed)_: Conduction, convection, and radiation problems with 1D/2D/3D meshes, time-stepping, view-factor computation, and thermal-resistance network builder.

- ⬜ **Quantum mechanics sandbox** _(stubbed)_: Bound-state, scattering, and time-dependent Schrödinger solver in 1D/2D/3D with interactive potential editor, eigenstate viewer, and density-matrix evolution for open systems.

- ⬜ **Relativity playground** _(stubbed)_: Special-relativity kinematics (Lorentz boosts, Minkowski diagrams) and general-relativity geodesic integrator for canonical metrics (Schwarzschild, Kerr, FLRW) with embedding-diagram and orbit visualization.

- ⬜ **N-body integrator** _(stubbed)_: Symplectic and high-order N-body simulator for celestial mechanics and molecular dynamics with adaptive timestepping, energy-drift monitoring, and Barnes-Hut/FMM acceleration.

- ⬜ **Signal processing lab** _(stubbed)_: FFT, STFT, wavelet, filter design (FIR/IIR, Butterworth/Chebyshev/elliptic), windowing, spectrogram, and system-identification tools. AI-integrated filter recommendation from a measured noise profile.

- ⬜ **Control systems designer** _(stubbed)_: Transfer-function and state-space modeling with root-locus, Bode, Nyquist, Nichols, pole placement, LQR/LQG, and PID auto-tuning against a plant model.

- ⬜ **Dimensional-analysis assistant** _(stubbed)_: Buckingham-pi reduction of governing equations to dimensionless groups, with named-group recognition (Reynolds, Nusselt, Prandtl, Mach, Weber) and scaling-law derivation.

- ⬜ **Error propagation tool** _(stubbed)_: Symbolic and Monte-Carlo propagation of measurement uncertainties through arbitrary expressions, with correlated inputs, sensitivity coefficients, and GUM-compliant reporting.

- ⬜ **Optimization studio** _(stubbed)_: Linear, nonlinear, integer, and global optimization with constraint editor, gradient/Hessian inspection, and multi-objective Pareto-front exploration. AI-integrated formulation from a natural-language problem statement.

- ⬜ **Data importer** _(stubbed)_: Loader for CSV, HDF5, Parquet, FITS, NetCDF, oscilloscope (Tektronix, LeCroy), Touchstone S-parameter, and lab-instrument formats with automatic schema and unit detection.

- ⬜ **LaTeX formula editor** _(stubbed)_: Live-rendered LaTeX/MathJax editor with palette, OCR from sketches or screenshots, and bidirectional conversion to symbolic CAS expressions.

- ⬜ **Material property database** _(stubbed)_: Searchable library of mechanical, thermal, optical, and electrical properties for metals, polymers, ceramics, composites, and semiconductors with temperature-dependent curves and provenance.

- ⬜ **Spectrum reference** _(stubbed)_: Atomic and molecular spectral-line database (NIST ASD, HITRAN) with line-list query, broadening models, and synthetic-spectrum generator for absorption and emission.


#### Mechanical and manufacturing

- ⬜ **Fastener selector** _(stubbed)_: screw/bolt/nut sizing by load and material, with metric and imperial standards, head/drive type filter, ISO/DIN/ANSI cross-reference, torque table, thread engagement calculator, McMaster cross-link for in-stock.

- ⬜ **PPE selector** _(stubbed)_: given a process and material, recommends specific personal protective equipment: respirator cartridge type (P100/organic-vapor/multi-gas/SCBA), glove material (nitrile vs. neoprene vs. butyl vs. PVA, with chemical-compatibility matrix), eye protection (shade for welding processes, splash for chemistry), hearing protection NRR matched to the noise level. Cites the SDS/IDLH for the specific exposure.

- ⬜ **Parametric CAD sketcher** _(stubbed)_: 2D constraint-based sketcher and 3D parametric modeler with feature tree, boolean operations, lofts, sweeps, and revolves. AI-integrated generation of sketches and feature trees from natural language descriptions or reference images.

- ⬜ **Mesh editor** _(stubbed)_: Imports, repairs, and edits STL/OBJ/3MF meshes with hole filling, decimation, remeshing, and boolean operations. Includes non-manifold detection and automatic repair suggestions.

- ⬜ **STEP/IGES converter** _(stubbed)_: Converts between STEP, IGES, BREP, STL, 3MF, OBJ, DXF, and SVG with healing of broken topology. Preserves assembly structure and metadata where possible.

- ⬜ **Slicer** _(stubbed)_: Generates G-code for FDM, SLA, and SLS printers with per-feature overrides, tree supports, ironing, and variable layer height. AI-integrated suggestion of slicing profiles from part geometry, material, and intended use.

- ⬜ **CAM toolpath generator** _(stubbed)_: Produces 2.5D, 3-axis, and 4/5-axis G-code with adaptive clearing, contour, pocket, and engraving operations. Includes post-processors for common controllers and a tool library with feeds/speeds.

- ⬜ **Feeds and speeds calculator** _(stubbed)_: Computes spindle RPM, feed rate, chip load, depth of cut, and material removal rate from tool geometry, material, and machine rigidity. Includes deflection and chatter warnings.

- ⬜ **FEA solver** _(stubbed)_: Linear and nonlinear static, modal, thermal, and buckling analysis on tet/hex meshes with contact and mixed boundary conditions. AI-integrated boundary condition suggestion from a description of the loading scenario.

- ⬜ **CFD solver** _(stubbed)_: Steady and unsteady incompressible/compressible flow with turbulence models, conjugate heat transfer, and porous media. Mesh generation from CAD with boundary layer refinement.

- ⬜ **Topology optimizer** _(stubbed)_: SIMP and level-set topology optimization under stress, displacement, and manufacturability constraints, with mesh export to CAD reconstruction. AI-integrated suggestion of design space, loads, and constraints from a functional description.

- ⬜ **Tolerance stackup analyzer** _(stubbed)_: Worst-case, RSS, and Monte Carlo tolerance stackup with GD&T support including position, profile, runout, and bonus tolerance. Sensitivity report identifies the dimensions driving variation.

- ⬜ **GD&T annotator** _(stubbed)_: Applies and validates ASME Y14.5 and ISO 1101 feature control frames on 2D drawings and 3D PMI views. AI-integrated suggestion of datums and tolerance schemes from part function.

- ⬜ **Drawing generator** _(stubbed)_: Produces dimensioned 2D drawings from 3D models with section, detail, and isometric views, BOM tables, and title block templating. Auto-projects views and infers reasonable dimensioning.

- ⬜ **Bearing and shaft designer** _(stubbed)_: Sizes ball, roller, plain, and thrust bearings with L10 life, dynamic load, and shaft fit calculations. Generates shoulder, fillet, and keyway dimensions per ISO standards.

- ⬜ **Gear designer** _(stubbed)_: Designs spur, helical, bevel, worm, and planetary gear trains with AGMA strength and pitting calculations, then exports involute profiles to CAD or DXF. Includes ratio solver for a target reduction.

- ⬜ **Spring designer** _(stubbed)_: Sizes compression, extension, torsion, and Belleville springs with stress, fatigue life, and natural frequency checks. Outputs manufacturable wire diameter, coil count, and free length.

- ⬜ **Material library** _(stubbed)_: Curated database of metals, polymers, ceramics, composites, and woods with mechanical, thermal, electrical, and processing properties plus cost and availability. AI-integrated material selection from functional requirements and constraints.

- ⬜ **Sheet metal unfolder** _(stubbed)_: Flattens bent sheet metal parts with k-factor and bend deduction tables, computes blank size, and outputs DXF for laser, plasma, or waterjet cutting. Detects undercuts and impossible bends.

- ⬜ **Weldment designer** _(stubbed)_: Applies structural profiles to a wireframe with mitered and butt joints, fillet and groove weld symbols, and AWS callouts. Computes weld throat stress under combined loading.

- ⬜ **Injection molding analyzer** _(stubbed)_: Predicts fill time, weld lines, air traps, sink marks, and warpage from part geometry and gate location. Suggests draft, wall thickness, and gate placement adjustments.

- ⬜ **3D printing failure diagnoser** _(stubbed)_: Analyzes photos or videos of failed prints to identify warping, stringing, layer shifts, under/over extrusion, and adhesion issues. AI-integrated recommendation of slicer and hardware fixes.

- ⬜ **G-code visualizer** _(stubbed)_: Renders toolpaths layer by layer or move by move with feed-rate, retract, and extrusion highlighting. Detects collisions, air-printing, and excessive travel.

- ⬜ **Kinematics simulator** _(stubbed)_: Assembles rigid bodies with revolute, prismatic, cylindrical, and gear joints; runs forward and inverse kinematics with collision detection. Plots position, velocity, acceleration, and joint torque over time.

- ⬜ **Vibration analyzer** _(stubbed)_: Modal extraction and harmonic, random, and shock response on FEA models or measured FRF data. AI-integrated correlation of measured modes with simulated ones for model updating.

- ⬜ **Heat transfer calculator** _(stubbed)_: Conduction, convection, and radiation calculators with fin, heat sink, and pipe network solvers. Includes view factor computation and transient lumped-capacitance models.

- ⬜ **Pneumatics and hydraulics designer** _(stubbed)_: Sizes cylinders, valves, lines, and accumulators by force, stroke, speed, and pressure drop, with circuit schematic capture and ISO 1219 symbols. Simulates pressure and flow over a duty cycle.

- ⬜ **DFM/DFA reviewer** _(stubbed)_: Flags geometry that is hard to machine, mold, cast, print, or assemble: thin walls, sharp internal corners, deep pockets, undercuts, fastener access. AI-integrated rewrite suggestions to reduce cost and lead time.

- ⬜ **BOM and cost estimator** _(stubbed)_: Rolls up multi-level bills of materials with vendor pricing, lead time, MOQ, and weight; estimates machining, printing, and molding cost from geometry. Exports CSV and supplier RFQs.

- ⬜ **Vendor and supplier search** _(stubbed)_: Searches McMaster, Misumi, Digi-Key, Grainger, and machine shop networks for parts matching a CAD model or spec sheet. AI-integrated extraction of part specs from a screenshot or description.

- ⬜ **PLM/revision tracker** _(stubbed)_: Versions CAD, drawings, BOMs, and G-code with branching, diffing, and ECO/ECR workflows. Visual diff of geometry highlights added, removed, and changed features.

- ⬜ **OCR for drawings** _(stubbed)_: Extracts dimensions, tolerances, GD&T frames, notes, and title block data from scanned or photographed engineering drawings into a structured model. AI-integrated reconstruction of geometry from a complete drawing set.

- ⬜ **Machine controller bridge** _(stubbed)_: Streams G-code to LinuxCNC, GRBL, Klipper, Marlin, Smoothie, and Mach3/4 over USB, serial, or network with live overrides and probe support. Captures probing results back into CAM and CAD.

- ⬜ **Probing and metrology suite** _(stubbed)_: Drives touch probes and CMMs for work coordinate setup, feature inspection, and best-fit alignment to nominal CAD. Generates inspection reports with deviation maps.

- ⬜ **Photogrammetry and scan-to-CAD** _(stubbed)_: Reconstructs meshes from photo sets or structured-light scans, then converts them to parametric features and primitives. AI-integrated segmentation of scans into recognizable mechanical features.

- ⬜ **Standards reference** _(stubbed)_: Searchable index of ISO, ANSI, ASME, DIN, JIS, AGMA, AWS, and ASTM standards with cross-references, tolerance tables, and fit charts. AI-integrated answers cite the specific clause and edition.

- ⬜ **Unit and conversion calculator** _(stubbed)_: Symbolic unit-aware calculator with engineering constants, dimensional analysis, and conversions across SI, imperial, and shop-floor units. Handles compound units like N·m/rad and W/(m·K).

- ⬜ **Pump and fan sizer** _(stubbed)_: Selects centrifugal and positive-displacement pumps and fans against a system curve with NPSH, efficiency, and viscosity corrections. Outputs operating point, power draw, and recommended drive.

- ⬜ **Pressure vessel and piping calculator** _(stubbed)_: ASME BPVC Section VIII and B31.1/B31.3 thickness, stress, and code compliance calculations for shells, heads, nozzles, and piping. Includes flange rating lookup.

- ⬜ **Lubricant and tribology advisor** _(stubbed)_: Recommends lubricants and surface treatments by load, speed, temperature, and environment, with Stribeck curve and wear-rate estimates. AI-integrated diagnosis of wear modes from photos of worn parts.

- ⬜ **Project and shop scheduler** _(stubbed)_: Plans jobs across machines, fixtures, operators, and material lead times with Gantt and capacity views. Predicts conflicts and suggests reorderings.


#### Maker fab and personal manufacturing

- ⬜ **FDM Slicer** _(stubbed)_: Slices STL/3MF/STEP meshes into G-code for FDM printers with per-object settings, variable layer height, tree and organic supports, ironing, and seam control. AI-integrated suggestion of starting parameters from material, geometry, and target strength.

- ⬜ **Resin Slicer** _(stubbed)_: Slices meshes into masked layer images for MSLA, DLP, and SLA printers with anti-aliasing, light PWM, hollow-and-drain, automatic raft and support trees, and per-layer exposure compensation.

- ⬜ **Printer Profile Manager** _(stubbed)_: Stores and edits machine, material, and process profiles with inheritance and diffing; imports from PrusaSlicer, OrcaSlicer, and Cura, and tracks per-printer calibration constants like flow, e-steps, PA, and bed mesh.

- ⬜ **Filament and Resin Library** _(stubbed)_: Catalogs filaments and resins with vendor specs, glass transition, shrinkage, drying schedule, shelf life, and spool inventory; logs print results to refine recommended temps, speeds, and exposure over time.

- ⬜ **Print Failure Diagnostician** _(stubbed)_: Analyzes photos of failed prints to identify causes (under-extrusion, warping, layer shift, elephant foot, ringing, delamination) and proposes profile changes, with AI integration for visual classification and remediation steps.

- ⬜ **Pressure Advance and Input Shaper Tuner** _(stubbed)_: Generates PA, input shaper, retraction, flow, temperature tower, and first-layer calibration test prints; reads resonance CSVs from Klipper or accelerometer logs and recommends shaper type and frequency.

- ⬜ **Mesh Repair and Orientation Optimizer** _(stubbed)_: Heals non-manifold meshes, fills holes, and re-meshes; finds print orientations that minimize support volume, maximize strength along load axes, and reduce visible seams on cosmetic faces.

- ⬜ **Lattice and TPMS Infill Generator** _(stubbed)_: Generates gyroid, Schwarz, voronoi, and graded lattices inside arbitrary shells with target density, stiffness, and porosity for lightweighting, conformal cushioning, and heat exchanger geometry.

- ⬜ **Laser Toolpath Generator** _(stubbed)_: Plans laser cutting and vector engraving paths with kerf compensation, lead-ins, tab bridges, focus offset, and multi-pass strategies; orders cuts to minimize travel and heat buildup, and tags parts to keep falling pieces in place.

- ⬜ **Laser Material Profile Library** _(stubbed)_: Stores power, speed, frequency, air-assist, and pass counts per material and thickness for CO2, fiber, and diode lasers; auto-generates test grids and refines profiles from measured kerf width, edge char, and cut-through.

- ⬜ **Raster Engraving Halftoner** _(stubbed)_: Converts bitmaps and photographs into laser raster jobs with Floyd-Steinberg, Stucki, Jarvis, and ordered dithering, gamma correction, and per-material grayscale calibration curves.

- ⬜ **CNC CAM Studio** _(stubbed)_: Generates 2.5D and 3-axis toolpaths (contour, pocket, adaptive clearing, parallel, scallop, drilling, engraving, V-carving) from STEP/DXF with stock-aware roughing, rest machining, and post-processors per controller.

- ⬜ **Feeds, Speeds and Tool Library** _(stubbed)_: Computes feeds, speeds, chip load, axial/radial engagement, and HP/torque per tool, material, and operation; maintains an endmill, drill, and tap library with stickout, runout, and wear tracking.

- ⬜ **G-code Visualizer and Simulator** _(stubbed)_: Renders FDM, laser, and CNC G-code with toolpath coloring by speed, layer, or extruder; simulates material removal, detects collisions and rapids through stock, and verifies post-processor output before machine time.

- ⬜ **Welding Parameter Calculator** _(stubbed)_: Computes amperage, voltage, wire feed speed, travel speed, shielding gas mix, and filler diameter for MIG, TIG, stick, and flux-core across common steel, stainless, and aluminum joints, with heat input, distortion, and bead profile estimates.

- ⬜ **Joinery Planner** _(stubbed)_: Designs mortise-and-tenon, dovetail, box, finger, lap, half-blind, and Domino joints with parametric sizing, grain orientation, and glue clearance; exports DXF or G-code for templates, router jigs, and CNC.

- ⬜ **Wood Movement Calculator** _(stubbed)_: Predicts seasonal dimensional change from species, grain orientation, starting and target moisture content, and ambient humidity range; flags panels and joints likely to cup, split, or bind.

- ⬜ **Nesting and Cut List Optimizer** _(stubbed)_: Packs parts onto sheet goods, plate, plywood, leather, or fabric with rotation locks, grain constraints, kerf, and common cuts; produces ordered, labeled cut lists for table saw, panel saw, laser, waterjet, or plasma.

- ⬜ **Tolerance and Fit Calculator** _(stubbed)_: Sizes clearance, transition, press, and snap-fit features per ISO 286 and process-specific shrinkage; recommends hole and shaft modifications by manufacturing method (FDM, resin, CNC, laser kerf, sheet bend).


#### Materials science and metallurgy

- ⬜ **Material safety database** _(stubbed)_: keyed by (material × process). Galvanized steel + welding: zinc oxide fumes, metal fume fever. PVC + laser cutting: HCl/chlorine, eats optics and lungs. Treated wood + burning: arsenic compounds. Foam + heating: HCN. Per entry: PEL/STEL dose, exposure mechanism, recommended PPE, ventilation rate, emergency-response steps.

- ⬜ **Periodic Property Explorer** _(stubbed)_: Interactive periodic table with sortable/colorable overlays for density, modulus, melting point, electronegativity, thermal/electrical conductivity, CTE, crystal structure, abundance, and cost per kg. Click an element to drill into isotopes, oxidation states, and common alloying roles.

- ⬜ **Alloy Database Browser** _(stubbed)_: Searchable database of steels, aluminums, titaniums, coppers, nickels, magnesiums, and superalloys keyed by UNS/AISI/EN/JIS designation, with composition ranges, tempers, and typical mechanical properties. Filter by yield, UTS, density, machinability, weldability, or cost.

- ⬜ **Ashby Chart Plotter** _(stubbed)_: Plots any pair of material properties (modulus vs density, strength vs cost, toughness vs hardness, etc.) as a Cambridge-style Ashby map with envelope overlays for metals, polymers, ceramics, composites, woods, and foams. Supports custom material indices and selection lines.

- ⬜ **Phase Diagram Viewer** _(stubbed)_: Binary and ternary equilibrium phase diagrams (Fe-C, Cu-Sn, Al-Si, Pb-Sn, Cu-Zn, Ag-Cu, Ti-Al, etc.) with lever-rule readouts at a clicked composition/temperature. Overlays metastable lines and identifies the phases present plus their fractions.

- ⬜ **TTT/CCT Plotter** _(stubbed)_: Time-temperature-transformation and continuous-cooling-transformation curves for common steels, with overlay of a user-defined cooling profile to predict resulting microstructure (martensite, bainite, pearlite, ferrite) and approximate hardness.

- ⬜ **Heat Treatment Scheduler** _(stubbed)_: Step-by-step recipe builder for anneal/normalize/quench/temper/age cycles with ramp rates, soak times, atmosphere, and quench medium. AI-integrated suggestion of a starting recipe from the alloy designation and a target hardness or microstructure.

- ⬜ **Hardness Converter** _(stubbed)_: Bidirectional conversion between Brinell, Rockwell B/C, Vickers, Knoop, Shore A/D, Leeb, and Mohs scales per ASTM E140, with approximate tensile-strength estimates for steels.

- ⬜ **Jominy Hardenability Estimator** _(stubbed)_: Predicts the Jominy end-quench curve from steel composition using Grossmann/ideal-diameter correlations, and estimates the maximum section size that will through-harden in a given quench medium.

- ⬜ **Strength Estimator** _(stubbed)_: Estimates yield strength, UTS, hardness, and ductility from composition, grain size (Hall-Petch), cold-work fraction, precipitate volume fraction, and temper. Includes Larson-Miller creep extrapolation for high-temperature service.

- ⬜ **Galvanic Corrosion Checker** _(stubbed)_: Looks up galvanic potentials in seawater and other electrolytes, flags incompatible material pairs by potential difference, and estimates area-ratio-weighted corrosion risk. Suggests insulating, coating, or sacrificial-anode mitigations.

- ⬜ **Corrosion Compatibility Matrix** _(stubbed)_: Material-vs-chemical resistance grid covering common metals, plastics, and elastomers against acids, bases, solvents, fuels, salts, and foods at various temperatures, sourced from compiled vendor and NACE tables.

- ⬜ **Pourbaix Diagram Viewer** _(stubbed)_: E-pH (Pourbaix) diagrams for iron, copper, aluminum, zinc, nickel, titanium, lead, and chromium, with overlay of a user's electrolyte conditions to predict passivation, immunity, or active corrosion regions.

- ⬜ **Polymer Property Tables** _(stubbed)_: Database of thermoplastics, thermosets, and elastomers (PLA, PETG, ABS, PC, PA6/66, POM, PEEK, PTFE, PP, PE, PMMA, PVC, TPU, silicone, EPDM, NBR, FKM, etc.) with Tg, Tm, HDT, modulus, elongation, density, dielectric, water absorption, and chemical resistance.

- ⬜ **Composite Laminate Calculator** _(stubbed)_: Classical lamination theory solver for fiber-reinforced laminates: stacks plies of CFRP/GFRP/AFRP with user-defined orientations, computes ABD matrix, in-plane and bending stiffness, ply stresses, and Tsai-Wu/Tsai-Hill/max-stress failure indices.

- ⬜ **Welding Procedure Helper** _(stubbed)_: Suggests filler-metal selection, preheat/interpass temperature, post-weld heat treatment, and shielding gas for MIG/TIG/stick/flux-core joining of two specified alloys. Includes carbon-equivalent and crack-susceptibility warnings.

- ⬜ **Machinability And Speeds Lookup** _(stubbed)_: Recommended surface speeds, feeds, and tool materials for turning, milling, drilling, and tapping a given alloy with HSS, carbide, or ceramic tooling; includes machinability index relative to AISI 1212.

- ⬜ **Surface Finish And Coating Selector** _(stubbed)_: Compares anodize types (I/II/III, hardcoat), passivation, electropolishing, electroplating (Zn, Ni, Cr, Cu, Au, Ag, Sn), PVD/DLC, powder coat, and paint systems by hardness, thickness, color, corrosion class (ASTM B117 hours), and cost.

- ⬜ **Tribology And Wear Estimator** _(stubbed)_: Archard wear-rate calculator with a friction-coefficient lookup for dry, boundary, and lubricated sliding pairs (steel-on-steel, bronze-on-steel, PTFE, UHMWPE, ceramics). Estimates PV limits for plain bearings.

- ⬜ **Fatigue And S-N Curve Tool** _(stubbed)_: Plots S-N curves for common alloys, applies Goodman/Soderberg/Gerber mean-stress corrections, and estimates endurance limit from UTS with surface, size, loading, temperature, and reliability factors per Shigley.

- ⬜ **Fracture Toughness And Crack Calculator** _(stubbed)_: Linear-elastic fracture mechanics solver: computes K_I for common geometries (edge crack, center crack, surface flaw), critical crack length from K_IC, leak-before-break checks, and Paris-law fatigue crack growth life.

- ⬜ **3D Print Material Picker** _(stubbed)_: Recommends FDM/SLA/SLS/MJF/DMLS materials for a part given service temperature, load, chemical exposure, optical/biocompatibility needs, dimensional tolerance, and budget. Flags anisotropy and post-processing requirements.

- ⬜ **Density And Mass Calculator** _(stubbed)_: Computes mass, volume, buoyancy, and material cost for stock shapes (round bar, plate, tube, sheet, hex) in any alloy, with unit conversion and packing-density estimates for powders and pellets.

- ⬜ **Thermal Expansion Mismatch Tool** _(stubbed)_: Computes interference, gap, or stress between two materials over a temperature range from CTE and modulus; useful for shrink fits, bimetallic strips, glass-to-metal seals, and PCB-to-component joints.

- ⬜ **Tempering Color And Spark Test Guide** _(stubbed)_: Visual chart of oxide tempering colors (straw to blue) versus temperature for plain carbon steels, plus a spark-stream identifier with reference imagery for distinguishing low/high carbon, tool, stainless, and cast iron grades.

- ⬜ **MSDS And Hazard Lookup** _(stubbed)_: Pulls safety data for metals dusts, fluxes, etchants, pickling acids, quench oils, and polymer outgassing products; flags incompatible storage, PPE requirements, and ventilation needs for the shop.


#### Acoustic, thermal, hydraulic, pneumatic engineering

- ⬜ **Noise level predictor** _(stubbed)_: Computes Leq, Lmax, L10/L90, NC/NR curves, and dBA/dBC weighting from source spectra and distance attenuation. Supports point, line, and area sources with ground/barrier corrections per ISO 9613.

- ⬜ **Room acoustics modeler** _(stubbed)_: Computes RT60 via Sabine, Eyring, and Millington formulas from surface areas and absorption coefficients. Predicts modal frequencies and Schroeder transition for rectangular rooms; AI-integrated material picks to hit a target reverberation curve.

- ⬜ **Sound transmission loss calculator** _(stubbed)_: Estimates STC, Rw, and OITC for single panels, double walls, and composite partitions using mass law, coincidence dip, and double-leaf resonance. Includes flanking path bookkeeping and a partition library.

- ⬜ **Anechoic chamber sizer** _(stubbed)_: Sizes wedge depth, cutoff frequency, and free-field volume for anechoic and hemi-anechoic rooms. Generates wedge tip/base geometry and predicts low-frequency absorption performance.

- ⬜ **Absorber and diffuser designer** _(stubbed)_: Designs porous, Helmholtz, membrane, and slot resonator absorbers to a target absorption curve. Includes QRD and PRD diffuser well-depth generation and far-field scattering estimates.

- ⬜ **Octave band spectrum tool** _(stubbed)_: Converts between narrowband, 1/1, 1/3, and 1/12 octave spectra with A/B/C/Z weighting. Performs band arithmetic (sum, difference, masking) and exports to common acoustic measurement formats.

- ⬜ **Heat exchanger sizer** _(stubbed)_: Sizes shell-and-tube, plate, double-pipe, and finned-tube exchangers using LMTD and effectiveness-NTU methods. Computes UA, fouling allowance, tube count, baffle spacing, and shell-side pressure drop per Kern/Bell-Delaware.

- ⬜ **Thermal network solver** _(stubbed)_: Builds and solves nodal RC networks for conduction, convection, and radiation with temperature-dependent properties. Supports steady-state and transient runs with view-factor coupling for radiative branches.

- ⬜ **Psychrometric calculator** _(stubbed)_: Evaluates moist-air states (T, RH, w, h, dew point, wet bulb) and plots processes on a psychrometric chart. Sizes coils, humidifiers, and evaporative coolers from inlet/outlet state pairs.

- ⬜ **Insulation thickness optimizer** _(stubbed)_: Finds economic insulation thickness for pipes, vessels, and walls from heat loss, energy cost, and capital cost. Checks surface temperature, condensation risk, and personnel protection limits.

- ⬜ **Pipe network solver** _(stubbed)_: Solves looped and branched liquid/gas networks via Hardy Cross or global Newton with Darcy-Weisbach and Colebrook-White friction. Handles pumps, valves, fittings, elevation, and parallel paths.

- ⬜ **Pressure drop and Moody tool** _(stubbed)_: Computes friction factor, Reynolds number, and dP for pipes, ducts, and fittings across laminar, transitional, and turbulent regimes. Includes Crane K-factor and equivalent-length libraries for valves and bends.

- ⬜ **Pump and fan curve fitter** _(stubbed)_: Fits H-Q, eta-Q, and NPSHr-Q polynomial curves from manufacturer points and applies affinity laws for speed/diameter scaling. Finds operating point against a system curve and flags off-BEP operation.

- ⬜ **NPSH and cavitation checker** _(stubbed)_: Computes NPSHa from suction-side geometry, vapor pressure, and elevation; compares against NPSHr with margin. Estimates cavitation damage risk and suction specific speed for centrifugal pumps.

- ⬜ **Valve Cv and Kv sizer** _(stubbed)_: Sizes control valves for liquid, gas, and two-phase service per ISA-75.01/IEC 60534 with choked-flow, FL, xT, and piping geometry corrections. Recommends trim style and predicts aerodynamic/hydrodynamic noise.

- ⬜ **Water hammer simulator** _(stubbed)_: Simulates transient pressure surges via method of characteristics with pump trip, valve closure, and check-valve slam scenarios. Sizes surge tanks, air vessels, and relief devices to keep pressures within envelope.

- ⬜ **Pneumatic actuator sizer** _(stubbed)_: Sizes single- and double-acting cylinders and rotary actuators from load, stroke, cycle time, and supply pressure. Computes air consumption (Nl/min), Cv of supply lines, and cushion requirements at end-of-stroke.

- ⬜ **Hydraulic circuit designer** _(stubbed)_: Schematic editor for hydraulic power packs with pumps, valves, accumulators, and actuators; simulates flow, pressure, and load dynamics. Sizes reservoirs, coolers, and filters from duty cycle and contamination targets.

- ⬜ **Compressed air system designer** _(stubbed)_: Sizes compressors, dryers, receivers, and distribution piping from CFM demand profiles and pressure-dew-point targets. Estimates leakage loss, specific energy (kW/cfm), and storage for peak shaving.

- ⬜ **Fluid properties database** _(stubbed)_: CoolProp/REFPROP-style lookup of density, viscosity, cp, k, Pr, surface tension, and saturation curves for water, refrigerants, oils, air, and mixtures. Returns properties along arbitrary T,P or T,x paths for use by other tools.

- ⬜ **View factor calculator** _(stubbed)_: Computes radiation view factors between surface pairs (parallel, perpendicular, coaxial disks, cylinders) via closed-form and numerical contour integration. Assembles a view-factor matrix with reciprocity and summation checks.

- ⬜ **Fan and duct noise predictor** _(stubbed)_: Predicts in-duct sound power from fans, dampers, and elbows per ASHRAE/AMCA methods and tracks attenuation through silencers, plenums, and terminations. Reports breakout noise and room NC at the diffuser.


#### Civil, architectural, and HVAC engineering

- ⬜ **Beam Designer** _(stubbed)_: Sizes wood, steel, and concrete beams under combined point and distributed loads using AISC, NDS, and ACI checks. Plots shear, moment, and deflection diagrams with allowable-stress and LRFD pass/fail flags.

- ⬜ **Column Calculator** _(stubbed)_: Evaluates axial, biaxial bending, and slenderness capacity for steel W-shapes, HSS, concrete, and timber columns. Includes interaction diagrams and effective-length factor lookup.

- ⬜ **Truss Solver** _(stubbed)_: Builds 2D and 3D pin-jointed trusses and solves member forces by method of joints or stiffness matrix. Supports parametric span/pitch sweeps with AI-integrated topology suggestion from load and span constraints.

- ⬜ **Frame Analyzer** _(stubbed)_: Performs linear and P-delta analysis of 2D moment frames with arbitrary releases, supports, and load cases. Exports envelope diagrams and unity-check tables per member.

- ⬜ **Foundation Sizer** _(stubbed)_: Designs spread footings, mat foundations, and drilled piers from soil bearing, lateral, and uplift inputs. Checks settlement, overturning, sliding, and reinforcement per ACI 318.

- ⬜ **Retaining Wall Designer** _(stubbed)_: Sizes cantilever, gravity, and segmental retaining walls against active/passive earth pressure, surcharge, and seismic load. Outputs stem reinforcement, heel/toe stability, and bearing pressure diagrams.

- ⬜ **Load Table Browser** _(stubbed)_: Searchable database of joist, deck, masonry, and lintel span tables from manufacturer and code sources. Filters by span, load, deflection limit, and material with AI-integrated explanation of why a row applies.

- ⬜ **Seismic Load Calculator** _(stubbed)_: Computes ASCE 7 equivalent lateral force, modal response, and base shear from site class, Ss/S1, and risk category. Distributes story shears and produces drift checks.

- ⬜ **Wind Load Calculator** _(stubbed)_: Generates ASCE 7 main wind-force and components-and-cladding pressures from exposure, topography, and building geometry. Produces pressure zone diagrams on plan and elevation.

- ⬜ **HVAC Load Calculator** _(stubbed)_: Performs Manual J / ASHRAE block and room-by-room heating and cooling load calculations from envelope, infiltration, internal gains, and design temperatures. Sizes equipment with bin-hour validation.

- ⬜ **Duct Sizer** _(stubbed)_: Sizes rectangular, round, and flat-oval ductwork using equal-friction, static-regain, or velocity methods. Computes pressure drop, noise rating, and fan operating point across the network.

- ⬜ **Pipe Sizer** _(stubbed)_: Sizes hydronic, steam, domestic water, and gas piping with Hazen-Williams, Darcy-Weisbach, and Spitzglass methods. Tracks fixture units, simultaneous demand, and pump head requirements.

- ⬜ **Psychrometric Chart** _(stubbed)_: Interactive ASHRAE psychrometric chart with state points, process lines (heating, cooling, mixing, humidification), and coil load extraction. Plots from typical-meteorological-year data overlays.

- ⬜ **Refrigeration Cycle Modeler** _(stubbed)_: Models vapor-compression, absorption, and heat-pump cycles using CoolProp fluid property tables. Outputs COP, capacity, and T-s/P-h diagrams for variable lift conditions.

- ⬜ **Building Energy Model** _(stubbed)_: Whole-building hourly simulation with multi-zone heat balance, schedules, and HVAC plant equipment. Compares retrofits against baseline EUI and ASHRAE 90.1 budgets.

- ⬜ **Daylight & Solar Analyzer** _(stubbed)_: Computes solar position, shading, daylight factor, and glare metrics from massing and window geometry. Generates annual sDA/ASE maps and overhang/fin optimization sweeps.

- ⬜ **Code Checker** _(stubbed)_: Cross-references IBC, IRC, IECC, ASHRAE 90.1, NFPA 13, and ADA against a project model for occupancy, egress, fire rating, envelope U-value, and accessibility. AI-integrated citation of the specific section that triggers each finding.

- ⬜ **Egress & Occupancy Planner** _(stubbed)_: Computes occupant load, required exits, travel distance, and corridor/stair widths from a floor plan. Highlights dead-end and common-path violations live as the plan edits.

- ⬜ **Site Plan Tool** _(stubbed)_: Drafts site plans with parcel, setback, easement, parking, and stormwater layers over imported survey or GeoJSON. Computes lot coverage, FAR, and impervious area against zoning rules.

- ⬜ **Stormwater & Drainage Designer** _(stubbed)_: Sizes culverts, swales, detention basins, and pipe networks using rational, SCS curve number, and Manning's methods. Generates hydrographs and routes flow through detention with stage-storage curves.

- ⬜ **Earthwork Calculator** _(stubbed)_: Computes cut/fill volumes between existing and proposed surfaces from contour or TIN input. Produces mass-haul diagrams and grading-balance optimization.

- ⬜ **Concrete Mix Designer** _(stubbed)_: Proportions concrete mixes by ACI 211 absolute-volume method targeting strength, slump, and durability class. Tracks SCM substitution, embodied carbon, and cost per cubic yard.

- ⬜ **Rebar Detailer** _(stubbed)_: Lays out reinforcement for slabs, beams, walls, and footings with development length, splice, and hook checks per ACI 318. Produces bar bending schedules and weight takeoffs.

- ⬜ **Bolted & Welded Connection Designer** _(stubbed)_: Designs steel connections (shear tabs, end plates, base plates, moment connections) per AISC 360 with block shear, prying, and weld capacity checks. Outputs detail drawings and bolt schedules.

- ⬜ **Section Property Calculator** _(stubbed)_: Computes area, centroid, moments of inertia, plastic modulus, torsion constant, and shear center for arbitrary built-up sections. Imports DXF outlines and rolled-shape libraries.

- ⬜ **Quantity Takeoff & Cost Estimator** _(stubbed)_: Extracts material quantities from a model or marked-up plan and prices them against RSMeans-style unit-cost libraries. Rolls into CSI-formatted cost reports with regional adjustment factors.


#### Civil utility (water, waste, sanitation)

- ⬜ **Pressurized network solver** _(stubbed)_: EPANET-style hydraulic engine for looped water distribution networks. Solves steady-state and extended-period simulation with Hazen-Williams, Darcy-Weisbach, or Chezy-Manning headloss, supports pumps, valves (PRV/PSV/FCV/TCV), tanks, and demand patterns, and reports node pressures, link flows, and tank trajectories.

- ⬜ **Pipe friction calculator** _(stubbed)_: Solves the Colebrook-White equation iteratively (and Swamee-Jain explicit form) for friction factor given Reynolds number and relative roughness. Cross-checks against Hazen-Williams and Manning, with a roughness library for new and aged ductile iron, PVC, HDPE, concrete, and lined steel.

- ⬜ **Pump curve matcher** _(stubbed)_: Plots manufacturer H-Q curves against system curves to find operating point, NPSH margin, and BEP distance. Handles parallel/series pumping, variable-speed (affinity laws), and wire-to-water efficiency; AI-integrated pump shortlist from a head/flow/duty target and a vendor catalog.

- ⬜ **Open channel flow tool** _(stubbed)_: Manning's and Chezy solver for trapezoidal, circular, rectangular, and irregular cross-sections. Computes normal depth, critical depth, Froude number, and gradually-varied-flow profiles (M1/M2/S1/S2) for storm and sanitary conduits.

- ⬜ **TR-55 runoff calculator** _(stubbed)_: NRCS curve-number method for peak discharge and runoff volume from small watersheds. Includes the TR-20/TR-55 tabular hydrograph method, time-of-concentration via sheet/shallow/channel flow segments, and 24-hour Type I/IA/II/III rainfall distributions.

- ⬜ **Stormwater network router** _(stubbed)_: SWMM-style 1D dynamic-wave solver for combined and separate sewer systems. Routes hydrographs through pipes, manholes, weirs, orifices, storage units, and pumps with surcharge and flooding tracking; supports LID controls (bioretention, permeable pavement, green roof).

- ⬜ **BMP sizer** _(stubbed)_: Sizes stormwater best-management practices: detention/retention ponds (modified Puls routing), bioretention cells, infiltration trenches, swales, and sand filters. Computes water-quality volume, channel-protection volume, and overflow weir/orifice geometry against local design storms.

- ⬜ **Sanitary sewer designer** _(stubbed)_: Gravity main sizing against minimum scour velocity (typically 2 ft/s) and maximum d/D ratios, with peaking factors (Harmon, Babbitt, Federov) from contributing population. Generates slope, invert, and cover profiles, flags drop manholes, and checks H2S generation risk via Pomeroy-Parkhurst.

- ⬜ **Septic system sizer** _(stubbed)_: Designs onsite wastewater systems from perc-test rates, soil texture, daily flow, and bedroom count. Sizes septic tank volume, drainfield trench length, dosing chamber, and pressure distribution laterals against state code (e.g. EPA 625, Ten States Standards) and reservation setbacks.

- ⬜ **Treatment train designer** _(stubbed)_: Process design for activated sludge (CMAS, SBR, MBR, A2O), trickling filter, RBC, lagoons, and constructed wetlands. Computes SRT, MLSS, F/M, aerator demand, clarifier surface overflow rate, and effluent BOD/TSS/NH3-N from influent load and kinetic coefficients (Metcalf & Eddy).

- ⬜ **Disinfection CT solver** _(stubbed)_: Computes CT (concentration times contact time) for free chlorine, chloramine, chlorine dioxide, and ozone against Giardia/virus log-removal credits per the SWTR. Includes UV dose calculation with transmittance, lamp aging, and validated reactor RED for Cryptosporidium.

- ⬜ **Water quality kinetics** _(stubbed)_: Models chlorine residual decay (bulk + wall reaction), DBP formation (THM/HAA), nitrification in distribution, and corrosion indices (Langelier, Ryznar, CCPP). Couples to the network solver for age-and-quality propagation through the system.

- ⬜ **Leak locator** _(stubbed)_: Acoustic correlation and pressure-transient leak detection from time-series sensor data. Cross-correlates hydrophone signals to estimate leak position along a pipe segment and runs inverse transient analysis against the hydraulic model; AI-integrated anomaly flagging from DMA night-flow patterns.

- ⬜ **IDF curve generator** _(stubbed)_: Fits intensity-duration-frequency curves from rainfall gauge records or NOAA Atlas 14 grids. Outputs design storms (Chicago, NRCS, alternating block, Huff) at user-selected return periods for the runoff and routing tools.

- ⬜ **Utility GIS overlay** _(stubbed)_: Layered map view of mains, laterals, valves, hydrants, manholes, lift stations, and outfalls over basemap and parcel data. Imports shapefile/GeoJSON/GeoPackage, snaps the hydraulic and sewer models to real geometry, and exports as-builts with attribute schemas (Esri Utility Network, INSPIRE).

- ⬜ **Asset deterioration model** _(stubbed)_: Pipe break and condition forecasting from age, material, soil corrosivity, traffic loading, and historical work-order data. Produces likelihood-of-failure scores, renewal prioritization, and capital plan curves; AI-integrated cohort survival fits where break records are sparse.

- ⬜ **SCADA timeseries viewer** _(stubbed)_: Imports historian exports (OSIsoft PI, Ignition, InfluxDB, CSV) and aligns flow, pressure, level, turbidity, and chlorine traces against the hydraulic model. Computes mass balance, unaccounted-for water, pump run hours, and minimum night flow per district metered area.


#### Heavy engineering: naval, petroleum, mining, nuclear

- ⬜ **Hull resistance estimator** _(stubbed)_: Holtrop-Mennen displacement-hull and Savitsky planing-hull regressions across a speed range; outputs frictional, residuary, and total resistance with EHP/BHP and Froude/Reynolds context.

- ⬜ **Seakeeping RAO analyzer** _(stubbed)_: Strip-theory heave, pitch, and roll response amplitude operators against JONSWAP, Pierson-Moskowitz, and Bretschneider spectra; reports significant motions, MII, and slamming/wetness counts.

- ⬜ **Propeller series matcher** _(stubbed)_: Wageningen B-series and Gawn polynomial open-water curves matched to engine torque and hull wake; finds optimum diameter, pitch, and blade-area ratio for the design point.

- ⬜ **Hydrostatics and stability calculator** _(stubbed)_: Builds Bonjean curves, KN tables, and intact/damage GZ curves from an offsets table; checks IMO IS Code and SOLAS damaged-stability criteria.

- ⬜ **Mooring catenary solver** _(stubbed)_: Static and quasi-dynamic multi-segment catenary for spread and turret moorings; reports line tensions, vessel offsets, watch circles, and anchor uplift.

- ⬜ **Ship structural midship designer** _(stubbed)_: Section modulus, neutral axis, and hull-girder bending checks against IACS CSR longitudinal strength; plate buckling per DNV/ABS allowables.

- ⬜ **Cathodic protection sizer** _(stubbed)_: Sacrificial-anode and ICCP current-demand sizing for ship hulls, jackets, and pipelines; coating breakdown factors per DNV-RP-B401 with anode mass and life.

- ⬜ **Well log analyzer** _(stubbed)_: Loads LAS and DLIS curves; computes Vsh from GR/SP, porosity from density-neutron, Sw via Archie or Indonesia, and net-pay flags with depth-aligned plotting.

- ⬜ **Reservoir grid viewer** _(stubbed)_: Reads ECLIPSE EGRID, INIT, and UNRST and CMG output; slices and animates pressure, saturation, and streamlines, with well trajectories and completion intervals.

- ⬜ **PVT and equation-of-state analyzer** _(stubbed)_: Peng-Robinson and Soave-Redlich-Kwong fluid models with volume shift; flashes mixtures, tunes BIPs to CCE/DL/CVD lab data, and exports black-oil PVT tables.

- ⬜ **Decline curve analyzer** _(stubbed)_: Fits Arps exponential, hyperbolic, and harmonic plus Duong and SEDM unconventional models; bootstraps EUR P10/P50/P90 uncertainty and exports type curves.

- ⬜ **Nodal analysis tool** _(stubbed)_: Vogel and Fetkovich IPR intersected with Hagedorn-Brown, Beggs-Brill, or mechanistic VLP correlations; sensitivity over choke, tubing size, and reservoir pressure.

- ⬜ **Drilling hydraulics and torque/drag** _(stubbed)_: Bingham, power-law, and Herschel-Bulkley rheology for ECD, swab/surge, and bit pressure drop; soft-string torque-drag along the wellbore trajectory.

- ⬜ **Material balance solver** _(stubbed)_: Havlena-Odeh straight-line plots and tank models for solution-gas, gas-cap, and water-drive reservoirs; estimates OOIP/OGIP and aquifer strength.

- ⬜ **Geosteering and stratigraphy viewer** _(stubbed)_: Cross-section builder with LWD curves, dip picks, and lateral-vs-pilot correlation; AI-assisted marker matching against a type log.

- ⬜ **Tubular and casing designer** _(stubbed)_: Burst, collapse, axial, and triaxial (von Mises) checks per API 5C3 with design factors for load cases (kick, displacement, stimulation, plug-and-perf).

- ⬜ **Fracture-pressure and pore-pressure predictor** _(stubbed)_: Eaton, Bowers, and Daines models for pore and frac pressure from seismic and offset logs; produces a depth-vs-pressure window for mud weight planning.

- ⬜ **Block model and ore-grade estimator** _(stubbed)_: Ordinary kriging, indicator kriging, IDW, and nearest-neighbor estimation onto a regular block grid; leave-one-out cross-validation and variogram fitting.

- ⬜ **Pit optimizer** _(stubbed)_: Lerchs-Grossmann and pseudoflow ultimate pit shells with nested pits by revenue factor; pushback sequencing and stripping-ratio rollups.

- ⬜ **Underground stope designer** _(stubbed)_: Sublevel, cut-and-fill, and longhole geometries with Mathews stability number and dilution-skin estimation; backfill strength and sequencing checks.

- ⬜ **Blast pattern designer** _(stubbed)_: Burden, spacing, subdrill, and stemming optimizer with Kuz-Ram and Swebrec fragmentation prediction; PPV attenuation and airblast contours.

- ⬜ **Slope stability analyzer** _(stubbed)_: Limit-equilibrium (Bishop, Janbu, Spencer, Morgenstern-Price) and strength-reduction factor analysis for benches and overall pit walls.

- ⬜ **Mine ventilation network solver** _(stubbed)_: Hardy Cross and Kirchhoff solver for airway pressure and flow; auxiliary fans, diesel particulate heat load, and radon decay-product balance.

- ⬜ **Haulage and fleet simulator** _(stubbed)_: Discrete-event truck-shovel cycle times with queueing, dispatch policies, and rimpull-grade curves; OEE, tonnes-per-hour, and fuel rollups.

- ⬜ **Drill core and chip logger** _(stubbed)_: Lithology, structure (alpha/beta), RQD, and assay capture against a depth-registered core photo strip; QA/QC against standards, blanks, and duplicates.

- ⬜ **Resource classification tool** _(stubbed)_: Inferred, indicated, and measured tagging per JORC and NI 43-101 from variogram range, drill spacing, and confidence shells; auditable category boundaries.

- ⬜ **Heap-leach and tank-leach modeller** _(stubbed)_: Column kinetics, cyanide or acid consumption, and Merrill-Crowe or SX-EW recovery; pad inventory and pregnant solution grade over time.

- ⬜ **Neutronics shielding calculator** _(stubbed)_: Point-kernel gamma attenuation with buildup factors and ANSI/ANS-6.4.3 source terms; multilayer dose-rate maps for Co-60, Cs-137, and mixed spectra.

- ⬜ **Reactor core point-kinetics sim** _(stubbed)_: Six-group delayed-neutron point kinetics with xenon, samarium, doppler, and moderator feedback; rod-worth curves and SCRAM transients.

- ⬜ **Burnup and depletion tool** _(stubbed)_: Bateman-equation depletion across ENDF/B nuclides; tracks isotopic vector, reactivity letdown, and spent-fuel composition by assembly.

- ⬜ **Cross-section viewer** _(stubbed)_: Loads ENDF/B-VIII and JEFF evaluations; plots resolved and unresolved resonances and reconstructs cross sections at user temperature with SLBW Doppler broadening.

- ⬜ **Decay heat estimator** _(stubbed)_: ANSI/ANS-5.1 fission-product decay heat and explicit chain integration for actinides; cooling curves from shutdown through long-term storage.

- ⬜ **Criticality safety checker** _(stubbed)_: keff lookup against LEU/HEU handbook benchmarks and simplified MCNP-style fissile-array geometry; subcritical margin and double-contingency review.

- ⬜ **ALARA dose planner** _(stubbed)_: Time, distance, and shielding optimizer for a work package against zone dose-rate maps; tracks collective and individual dose budgets per task.

- ⬜ **Activation and waste classifier** _(stubbed)_: 10 CFR 61 Class A/B/C and IAEA categorization from neutron flux, irradiation history, and material composition; long-lived nuclide flagging.

- ⬜ **Containment thermal-hydraulics sketcher** _(stubbed)_: Lumped-parameter blowdown, pressure-suppression, and hydrogen-burn estimates for LOCA scenarios; PCT and containment pressure timelines.

- ⬜ **Pressure vessel designer** _(stubbed)_: ASME BPVC Section VIII Division 1 and 2 shell, head, flange, and nozzle-reinforcement calculations with MAWP and MDMT checks.

- ⬜ **Pipe stress and flexibility analyzer** _(stubbed)_: B31.3 and B31.8 sustained, expansion, and occasional stress per Markl SIFs and flexibility factors; supports, anchors, and thermal growth.

- ⬜ **Fatigue and S-N analyzer** _(stubbed)_: Rainflow counting and Miner-rule damage against DNV-RP-C203, BS 7608, and API 579 curves; mean-stress and weld-class corrections.

- ⬜ **Corrosion and metallurgy advisor** _(stubbed)_: Galvanic series, mixed-potential, CO2/H2S corrosion (de Waard-Milliams), and NACE MR0175 sour-service envelope; material selection notes for CRAs.

- ⬜ **Pump and compressor curve tool** _(stubbed)_: Affinity laws, NPSH margin, surge and stonewall checks; overlays vendor curves on system head and shifts for speed, impeller trim, and viscosity.

- ⬜ **Multiphase pipeline flow tool** _(stubbed)_: Beggs-Brill, OLGAS-style mechanistic, and Lockhart-Martinelli regimes for two- and three-phase pipelines; slug-catcher sizing and erosional velocity.

- ⬜ **Subsea riser and umbilical analyzer** _(stubbed)_: Catenary, lazy-wave, and steel catenary riser configurations with VIV fatigue screening per DNV-RP-F204; touchdown-point fatigue hot spots.

- ⬜ **Materials property database** _(stubbed)_: Indexed steels, nickel alloys, titanium, concrete, and composites with creep, fracture toughness, KIC, and irradiation-embrittlement (RTndt shift) curves.

- ⬜ **Standards and code lookup** _(stubbed)_: Indexed ASME, API, ABS, DNV, ISO, ASTM, IAEA, ICRP, and NRC references with AI-assisted clause finder and citation cross-walk between codes.

- ⬜ **Engineering unit converter** _(stubbed)_: Field and SI conversions across heavy-industry units: bbl, mcf, BTU, troy oz, rem/Sv, dpm, gpm, m3/h, psi, bar, ksi, and per-foot vs per-meter rates.

- ⬜ **Quick FEM previewer** _(stubbed)_: Linear-elastic 2D meshing with plane-stress, plane-strain, and axisymmetric solvers for stress and modal sanity checks before sending to a full FEA package.

- ⬜ **P&ID and flowsheet sketcher** _(stubbed)_: ISA S5.1 symbol library with intelligent line connectors; emits line lists, valve lists, and stream tables suitable for hand-off to a process simulator.

- ⬜ **HAZID and HAZOP register** _(stubbed)_: Node-by-node guideword prompting (NO, MORE, LESS, REVERSE) with consequence, safeguard, and SIL assignment; AI-suggested deviations from a P&ID.

- ⬜ **Reliability and RAM modeller** _(stubbed)_: Fault-tree and reliability block diagram solver with Weibull, exponential, and lognormal distributions; computes availability, MTTR, and production efficiency.

- ⬜ **Bathymetry and DEM viewer** _(stubbed)_: GeoTIFF, BAG, and XYZ terrain rendering with slope, aspect, and contour generation; overlay for cable routes, anchor patterns, and tailings dam siting.

- ⬜ **Seismic SEG-Y previewer** _(stubbed)_: Loads SEG-Y 2D and 3D volumes; arbitrary inline/crossline/time slices with horizon picking and well-tie wavelet check.

- ⬜ **GIS and georeference helper** _(stubbed)_: Reprojects between WGS84, UTM, state plane, and local mine grids; reads shapefiles, GeoPackage, and KML with attribute joins for lease, license, and concession boundaries.


#### Aerospace and orbital mechanics

- ⬜ **Orbital Propagator** _(stubbed)_: Propagates state vectors and TLEs forward and backward in time using SGP4/SDP4, two-body Kepler, or a numerical integrator with J2/J4, atmospheric drag, solar radiation pressure, and third-body perturbations. Outputs ephemeris, ground tracks, and 3D orbit visualizations.

- ⬜ **TLE Workbench** _(stubbed)_: Fetches, parses, edits, and validates two-line element sets from Celestrak or local catalogs, with checksum repair, epoch shifting, and side-by-side diffing of revisions for the same NORAD ID.

- ⬜ **Element Set Converter** _(stubbed)_: Round-trips between Cartesian state, classical orbital elements, equinoctial elements, modified equinoctial, TLE mean elements, and Keplerian osculating elements, with frame transforms across ECI, ECEF, RIC, LVLH, and J2000.

- ⬜ **Ground Track Visualizer** _(stubbed)_: Renders satellite ground tracks, sub-solar points, and instrument footprints on a 2D Earth map and 3D globe, with revisit overlays, swath width from sensor FOV, and exportable KML/GeoJSON.

- ⬜ **Lambert Solver** _(stubbed)_: Solves the two-point boundary value problem for ballistic transfers between two position vectors and a time of flight, supporting multi-revolution solutions, prograde/retrograde branches, and Izzo's universal-variable formulation.

- ⬜ **Porkchop Plot Generator** _(stubbed)_: Sweeps launch and arrival date grids to produce C3, departure v-infinity, arrival v-infinity, declination of launch asymptote, and total delta-v contour plots for interplanetary or lunar transfers. AI-integrated suggestion of promising windows from mission constraints.

- ⬜ **Delta-V Budget Planner** _(stubbed)_: Composes mission delta-v budgets from Hohmann, bi-elliptic, plane change, combined, phasing, and finite-burn maneuvers, with margin policies and side-by-side comparison of architectures.

- ⬜ **Rocket Equation Solver** _(stubbed)_: Evaluates Tsiolkovsky's equation for single and multi-stage vehicles, solving for delta-v, propellant mass, payload mass, or Isp; includes staging optimization and gravity/drag loss models for ascent.

- ⬜ **Launch Window Finder** _(stubbed)_: Computes daily launch windows from a given site to a target orbit or interplanetary trajectory, honoring inclination reachability, ascending-node alignment, eclipse, range-safety azimuth, and lighting constraints.

- ⬜ **Re-entry Calculator** _(stubbed)_: Simulates ballistic and lifting re-entry trajectories with selectable atmosphere and drag models, reporting peak g-load, peak heating rate, integrated heat load, downrange/crossrange, and a landing footprint ellipse.

- ⬜ **Atmosphere Model Browser** _(stubbed)_: Evaluates US Standard 1976, NRLMSISE-00, Jacchia-Bowman, and exponential atmospheres for density, temperature, pressure, and composition versus altitude, with solar/geomagnetic activity inputs.

- ⬜ **Gravity Field Explorer** _(stubbed)_: Computes gravitational acceleration and potential from point mass, J2/J4 zonals, and full EGM2008/GRGM spherical harmonic models for Earth, Moon, Mars, and asteroids, with geoid and free-air anomaly visualization.

- ⬜ **Attitude Dynamics Simulator** _(stubbed)_: Propagates rigid-body attitude using quaternions, MRPs, or Euler angles under reaction wheel, CMG, magnetic torquer, and RCS actuation, with gravity gradient, aero, SRP, and residual dipole disturbance torques.

- ⬜ **RCS & Thruster Sizer** _(stubbed)_: Sizes reaction control thrusters and propellant tanks from required impulse bits, minimum-impulse pulses, slew rate, and translation delta-v, accounting for plume impingement and duty cycle.

- ⬜ **Link Budget Calculator** _(stubbed)_: Builds RF and optical link budgets between spacecraft and ground stations or relay satellites, computing EIRP, free-space loss, atmospheric and rain attenuation, G/T, C/N0, Eb/N0, and margin versus required BER.

- ⬜ **Power & Eclipse Budget** _(stubbed)_: Models solar array power generation versus sun angle, eclipse fraction, and degradation, sizes battery depth-of-discharge for the worst-case orbit, and balances against a load timeline keyed to mission modes.

- ⬜ **Constellation Designer** _(stubbed)_: Lays out Walker-Delta, Walker-Star, flower, and custom constellations, evaluating global coverage, revisit time, gap statistics, and inter-satellite link geometry; supports automated phasing optimization.

- ⬜ **Gravity Assist Planner** _(stubbed)_: Designs single and multi-flyby trajectories using patched-conics with v-infinity matching, periapsis altitude constraints, and Tisserand parameter plots for tour design across moons or planets.

- ⬜ **Mission Timeline Planner** _(stubbed)_: Sequences mission phases, maneuvers, ground passes, eclipses, and deliverables on a Gantt-style timeline with dependencies, resource consumption, and automatic conflict detection against the propagated ephemeris.

- ⬜ **Rendezvous & Proximity Ops** _(stubbed)_: Plans and visualizes rendezvous, station-keeping, and docking using Clohessy-Wiltshire and Tschauner-Hempel relative motion, with V-bar/R-bar approach corridors, plume cone checks, and abort trajectories.


#### Aerodynamics and CFD

- ⬜ **Airfoil Library** _(stubbed)_: Browsable, searchable catalog of UIUC, NACA, Selig, Eppler, Wortmann, and Drela airfoils with coordinates, thumbnails, and metadata (max thickness, camber, design Re). Supports tagging, favorites, and side-by-side overlay of section geometry.

- ⬜ **NACA Generator** _(stubbed)_: Parametric generator for 4-digit, 5-digit, and 6-series NACA sections from camber, thickness, and reflex parameters. Outputs cleanly redistributed point clouds with cosine spacing suitable for panel solvers.

- ⬜ **Airfoil Shape Editor** _(stubbed)_: Direct manipulation of camber line and thickness distribution via Bezier or PARSEC control points, with live update of leading-edge radius, trailing-edge angle, and area. Includes AI-integrated suggestion of a starting shape from target Cl, Re, and thickness constraints.

- ⬜ **Polar Solver** _(stubbed)_: XFOIL-style viscous/inviscid 2D solver that sweeps angle of attack and Reynolds number to produce Cl, Cd, Cm, and L/D polars. Handles forced and free transition with selectable Ncrit and supports batch runs across an airfoil set.

- ⬜ **Cp Plotter** _(stubbed)_: Plots chordwise pressure coefficient distributions for a given alpha and Re, with upper/lower surface separation, sonic line marker, and animated alpha sweep. Overlays multiple airfoils or operating points for direct comparison.

- ⬜ **Boundary Layer Calculator** _(stubbed)_: Integral boundary-layer solver using Thwaites for laminar regions and Head's or Green's lag-entrainment for turbulent regions, reporting displacement thickness, momentum thickness, shape factor, and skin friction. Flags separation and laminar bubbles along the chord.

- ⬜ **Transition Predictor** _(stubbed)_: e^N envelope and database-Tollmien-Schlichting transition estimator that locates the natural transition point on each surface as a function of Ncrit, surface roughness, and pressure gradient. Integrates with the polar solver to refine drag at low Re.

- ⬜ **Panel Method Studio** _(stubbed)_: 2D and 2.5D source-doublet panel method with linear-strength panels, Kutta condition enforcement, and wake relaxation. Visualizes panel-by-panel velocity, vorticity, and influence coefficients for teaching and debugging.

- ⬜ **Vortex Lattice Method** _(stubbed)_: AVL-style 3D VLM for wings, tails, and canards with control surface deflections, dihedral, sweep, and twist. Returns span loading, induced drag, stability derivatives, and trim solutions.

- ⬜ **Lifting Line Analyzer** _(stubbed)_: Prandtl lifting-line solver with arbitrary planform, taper, twist, and section polars per station. Reports elliptic-comparison efficiency, local Cl distribution, and stall onset station.

- ⬜ **Wing and Fuselage Drag Estimator** _(stubbed)_: Component drag buildup combining skin friction, form factor, interference, induced, wave, and trim drag for wings, fuselages, nacelles, and pylons. Cross-checks against the VLM and polar solver and breaks down the drag stack in a stacked bar.

- ⬜ **CFD Mesh Generator** _(stubbed)_: Generates structured C-meshes and O-meshes around airfoils and unstructured triangular/tetrahedral meshes around arbitrary STL geometry. Provides hyperbolic extrusion for boundary-layer prism layers with target y+ and growth ratio.

- ⬜ **RANS Solver** _(stubbed)_: Local finite-volume Navier-Stokes solver for 2D and small 3D cases with Spalart-Allmaras and k-omega SST turbulence models, compressible and incompressible branches. Streams residuals, force coefficients, and convergence plots live during the run.

- ⬜ **Flow Visualizer** _(stubbed)_: Interactive viewer for pressure, Mach, vorticity, and velocity fields with streamlines, line integral convolution, isosurfaces, and animated particle traces. Reads VTK, CGNS, and the workbench's native solver output.

- ⬜ **Multi-Element Airfoil Analyzer** _(stubbed)_: Solves slat, main, and flap configurations with gap and overlap parameters, including confluent boundary layers and wake-on-element interactions. Optimizes gap/overlap for max Cl_max at a chosen Re.

- ⬜ **Propeller and Rotor Designer** _(stubbed)_: Blade-element momentum theory tool for propellers, ducted fans, and rotors that uses section polars from the airfoil database to predict thrust, torque, and efficiency across J and RPM. Includes Prandtl tip-loss and induced swirl corrections.

- ⬜ **Compressible Flow Calculator** _(stubbed)_: Isentropic, normal shock, oblique shock, Prandtl-Meyer expansion, Fanno, and Rayleigh tables and solvers with full inverse modes. Plots shock polars and theta-beta-M diagrams for supersonic geometry design.

- ⬜ **Standard Atmosphere** _(stubbed)_: ISA and non-standard atmosphere model returning density, pressure, temperature, viscosity, speed of sound, and Reynolds-per-meter as a function of altitude and offset. Converts between TAS, EAS, CAS, and Mach for any given condition.

- ⬜ **Wind Tunnel Importer** _(stubbed)_: Imports tabular and image-based polar data from NACA reports, UIUC low-Re tests, and CSV/JSON exports, applying solid-blockage, wake-blockage, and streamline-curvature corrections. Aligns imported data with solver output for validation overlays.

- ⬜ **Aero Optimizer** _(stubbed)_: Gradient and genetic optimizer that drives airfoil shape, planform, or twist distribution toward objectives like min Cd at fixed Cl, max L/D, or robust performance across an operating envelope. AI-integrated proposal of seed designs and constraint sets from a natural-language goal.


#### Aviation, amateur radio, and RF

- ⬜ **METAR/TAF decoder** _(stubbed)_: Parses raw METAR, SPECI, TAF, PIREP, and AIRMET/SIGMET text into structured fields with plain-English summaries. Chains stations along a planned route and flags ceiling, visibility, wind, and icing trends against personal minimums.

- ⬜ **Flight planner** _(stubbed)_: Builds VFR and IFR routes with winds aloft interpolation, fuel burn from POH performance, alternates selection, and great-circle or airway routing. Exports to ForeFlight, SimBrief, FltPlan, and GPX, and renders the plog with time, distance, and fuel per leg.

- ⬜ **Weight and balance calculator** _(stubbed)_: Stores per-tail aircraft envelopes, station arms, and useful load. Plots CG against the forward and aft envelope across fuel burn, validates zero-fuel and ramp limits, and supports multi-leg loading scenarios.

- ⬜ **Chart viewer** _(stubbed)_: Renders georeferenced FAA sectional, TAC, IFR low/high enroute, terminal area, and approach plates with day/night palettes. Overlays the active route, airspace classes, MEAs, MOAs, and NOTAM polygons.

- ⬜ **Approach plate library** _(stubbed)_: Indexes terminal procedure publications (IAP, STAR, DP, airport diagrams) by airport, procedure name, and equipment requirement. Supports briefing strips, missed-approach extraction, and minima lookup by aircraft category.

- ⬜ **Aircraft logbook** _(stubbed)_: Records flights with PIC/SIC, day/night, instrument actual/simulated, approaches, holds, and landings. Tracks currency for BFR, IPC, medical, type ratings, and rolling 90/6 requirements, and exports to LogTen, MyFlightbook, and FAA 8710 forms.

- ⬜ **NOTAM and TFR browser** _(stubbed)_: Pulls FAA, ICAO, and military NOTAMs, decodes the four-letter codes, and filters by route corridor, altitude band, and time window. Highlights stadium TFRs, VIP movements, and active SUA.

- ⬜ **Performance calculator** _(stubbed)_: Interpolates POH takeoff, landing, climb, and cruise charts for density altitude, weight, wind, runway slope, and surface. Computes accelerate-stop distance, single-engine service ceiling, and drift-down for twins.

- ⬜ **E6B flight computer** _(stubbed)_: Solves wind triangle, true airspeed, true altitude, density altitude, fuel burn, time and distance, crosswind components, and unit conversions. Available as both an interactive panel and a callable solver for chained calculations.

- ⬜ **ADS-B traffic console** _(stubbed)_: Decodes 1090ES and 978 UAT from a local dongle (dump1090/dump978), shows traffic with Mode S registry lookup, and logs tracks for replay. Cross-references with FlightAware and OpenSky for missing aircraft.

- ⬜ **ADIF logbook** _(stubbed)_: Logs amateur radio QSOs across HF/VHF/UHF with band, mode, RST, grid, name, and contest exchanges. Imports and exports ADIF 3, deduplicates, and syncs to LoTW, eQSL, QRZ Logbook, and Club Log.

- ⬜ **Award tracker** _(stubbed)_: Scores progress on DXCC, WAS, WAC, VUCC, IOTA, SOTA, POTA, and CQ zones per band and mode. Renders needed-entity heatmaps on an azimuthal map and surfaces openings where missing entities are workable now.

- ⬜ **Contest logger** _(stubbed)_: Captures contest QSOs with super-check partial, real-time dupe checking, rate windows, and rover/multi-op section support. Exports Cabrillo for ARRL, CQ WW, WPX, NAQP, and IARU.

- ⬜ **Callsign lookup** _(stubbed)_: Queries QRZ, HamQTH, FCC ULS, Industry Canada, and OFCOM databases with an offline cache. Returns operator name, grid, license class, expiration, and prior QSO context from the local log.

- ⬜ **Repeater and satellite database** _(stubbed)_: Browses RepeaterBook, RFinder, and PNW data by location, band, tone, and mode (DMR, D-STAR, YSF, P25, M17). Predicts amateur satellite passes from current TLEs with Doppler tuning curves and squint angle.

- ⬜ **Rig control bridge** _(stubbed)_: Talks Hamlib/rigctl to Yaesu, Icom, Kenwood, Elecraft, and FLEX radios for frequency, mode, split, and PTT. Integrates with logger, digital modes, rotators (rotctl), and the SDR console for transceive.

- ⬜ **CW trainer and decoder** _(stubbed)_: Generates Koch and Farnsworth practice with adjustable WPM, character set, and weight, and decodes received audio with adaptive noise gating. Includes paddle/keyer simulation and contest-style head-copy drills.

- ⬜ **Digital mode bridge** _(stubbed)_: Wraps WSJT-X (FT8, FT4, JT65, Q65, MSK144), JS8Call, fldigi (PSK31, RTTY, Olivia, Contestia), and VARA. Routes decodes into the ADIF log and exposes a programmatic API for automated CQ, sked, and grid-square chasing.

- ⬜ **NEC antenna designer** _(stubbed)_: Edits wire and surface geometries, runs NEC2, NEC4, and MININEC engines, and plots 2D/3D gain patterns, SWR sweeps, current distribution, and near-field. Supports radials, lossy ground, tapered elements, and Sommerfeld-Norton modeling.

- ⬜ **Smith chart and matching** _(stubbed)_: Plots impedance and admittance with frequency markers, computes L, Pi, T, and stub matching networks, and shows Q-circles and VSWR contours. Synthesizes a schematic with realistic component Q and self-resonance.

- ⬜ **Propagation predictor** _(stubbed)_: Runs VOACAP, ITURHFProp, P2P, and area coverage with current SSN, SFI, Kp, and aurora ingest from NOAA SWPC. Renders MUF/LUF maps, gray-line, sporadic-E, tropo ducting, and meteor scatter windows.

- ⬜ **Transmission line calculator** _(stubbed)_: Computes coax and ladder line loss, velocity factor, electrical length, matched and mismatched line behavior, and balun/unun turn ratios. Includes a library of common cables (LMR, RG, Heliax, Andrew) with temperature derating.

- ⬜ **RF filter designer** _(stubbed)_: Synthesizes lumped LC, microstrip, stripline, helical, and SAW filters with Butterworth, Chebyshev, elliptic, Bessel, and Gaussian responses. Outputs schematic, layout estimate, and Touchstone S-parameters for cascade simulation.

- ⬜ **Touchstone viewer** _(stubbed)_: Loads .s1p through .s4p files and plots magnitude, phase, group delay, Smith, and polar. Cascades networks, de-embeds fixtures, converts between S, Y, Z, T, and ABCD, and exports back to Touchstone or CITI.

- ⬜ **Spectrum analyzer console** _(stubbed)_: Drives RTL-SDR, HackRF, Airspy, USRP, LimeSDR, and PlutoSDR for live FFT, waterfall, peak table, occupied bandwidth, ACPR, and channel power. Records sweeps for long-term band-occupancy studies.

- ⬜ **SDR receiver console** _(stubbed)_: Tunes a soft radio with AM, FM, NFM, SSB, CW, DSD+ digital voice, POCSAG, ACARS, and APRS demodulators. Supports squelch, AGC, noise blanker, notch, and a software panadapter slaved to the rig control bridge.

- ⬜ **IQ recorder and classifier** _(stubbed)_: Captures baseband to SigMF, WAV, or cfile with metadata, and replays into demodulators or external GNU Radio flowgraphs. AI-integrated signal classifier proposes modulation, baud, and likely service from a short capture.

- ⬜ **Band plan and regulatory checker** _(stubbed)_: Holds IARU Region 1/2/3 band plans, FCC Part 97/15/87/80/90 allocations, and ITU footnotes. Validates a planned transmission against license class privileges, power limits, mode restrictions, and channel spacing.

- ⬜ **GNU Radio flowgraph runner** _(stubbed)_: Loads .grc flowgraphs, parameterizes them, and runs them headless against live SDR hardware or recorded IQ. Captures sink outputs (audio, file, ZMQ) and pipes them into the logger or classifier.

- ⬜ **PCB RF stackup helper** _(stubbed)_: Solves microstrip, stripline, CPWG, and differential impedance for common laminates (FR-4, Rogers 4350B, Duroid, Isola). Estimates dielectric and conductor loss, via inductance, and recommends stitching for a target return path.

- ⬜ **QTH and station planner** _(stubbed)_: Models the home or portable station with antenna heights, terrain (SRTM), HAAT, and obstruction profiles. Runs Longley-Rice/ITM, Splat!, and HFTA for VHF/UHF and HF takeoff angle studies.

- ⬜ **Pre-flight briefing aggregator** _(stubbed)_: Pulls 1800wxbrief, Leidos, GFA, winds aloft, icing/turbulence (GTG, FIP), PIREPs, and SWPC HF impact tables into a single brief. Stores the brief with the flight log entry for FAR 91.103 compliance evidence.


#### Automotive and vehicle systems

- ⬜ **OBD-II Live Console** _(stubbed)_: Connects to an ELM327 or J2534 adapter over USB/Bluetooth and streams live PIDs, freeze-frame data, and readiness monitors. Reads and clears DTCs with a built-in code dictionary across OBD-II, mode 22, and common manufacturer extensions.

- ⬜ **CAN Bus Analyzer** _(stubbed)_: Sniffs raw CAN frames at user-set baud rates, decodes them against loaded DBC/KCD files, and visualizes signals as time-series. Supports replay, filtering, and bit-level inspection for reverse-engineering proprietary buses.

- ⬜ **ECU Map Editor** _(stubbed)_: Opens fuel, ignition, boost, and VE tables from common ECU formats (Megasquirt, Speeduino, Haltech exports) as editable 2D/3D surfaces. AI-integrated smoothing and outlier detection suggest table edits from logged operating-point density.

- ⬜ **Dyno Chart Plotter** _(stubbed)_: Imports chassis or engine dyno runs (CSV, SAE J1349) and overlays power and torque curves with correction factors, smoothing, and run-to-run delta comparison. Computes area-under-curve, peak holds, and powerband width.

- ⬜ **Gear Ratio Designer** _(stubbed)_: Models transmission, final drive, and tire circumference to plot shift points, road speed per gear, and engine RPM crossover against a power curve. Solves for ideal ratios given target top speed, launch RPM, or course-specific shift count.

- ⬜ **Engine Geometry Lab** _(stubbed)_: Computes bore, stroke, rod ratio, deck height, swept and clearance volume, static and dynamic compression ratio, and piston speed. Includes a valve-event and cam-timing visualizer with overlap, LSA, and effective duration.

- ⬜ **Volumetric Efficiency Calculator** _(stubbed)_: Back-calculates VE and BSFC from MAF/MAP, intake air temperature, RPM, displacement, and injector duty cycle. Plots VE surfaces against load and RPM for tuning verification against logged data.

- ⬜ **Turbo and Supercharger Sizing** _(stubbed)_: Plots required mass flow and pressure ratio against compressor maps loaded as polylines, with surge and choke margins highlighted. AI-integrated starting point picks a turbo from target power, altitude, and engine displacement.

- ⬜ **AFR and Lambda Analyzer** _(stubbed)_: Reads wideband O2 logs, correlates lambda against load, RPM, and coolant temperature, and highlights lean spikes, transient enrichment errors, and closed-loop trim drift. Generates fuel table correction suggestions in percent.

- ⬜ **Spring and Motion Ratio Calculator** _(stubbed)_: Solves wheel rate, spring rate, ride frequency, and damping ratio from suspension geometry, sprung mass, and motion ratio. Includes a coilover preload, droop, and bump-stop engagement model with corner-by-corner output.

- ⬜ **Damper Tuning Workbench** _(stubbed)_: Imports shock-dyno CSV traces and overlays force-velocity curves with bump/rebound separation, hysteresis loops, and digressive/linear/progressive classification. Cross-plots damper position with vehicle telemetry to find packer hits and frequency-domain artifacts.

- ⬜ **Suspension Geometry Visualizer** _(stubbed)_: Sketches double-wishbone, MacPherson, or multi-link geometry in 2D, computes roll center, instant center, anti-dive, anti-squat, and camber/bump-steer curves through travel. Outputs kinematics CSV for use in a vehicle model.

- ⬜ **Pacejka Tire Model Lab** _(stubbed)_: Fits the Magic Formula (PAC2002 / MF6.1) to measured Fy/Fx/Mz data and plots slip angle, slip ratio, and combined-grip friction ellipses. Generates parameter sets ready for export to vehicle-dynamics solvers.

- ⬜ **Tire Pressure and Temperature Logger** _(stubbed)_: Captures internal/external IR tire temperatures, hot and cold pressures, and tread depth per corner across sessions. Plots inside/middle/outside temperature spread against camber and pressure to guide alignment changes.

- ⬜ **Corner Weight and Setup Sheet** _(stubbed)_: Logs scale pad weights, computes cross-weight, front/rear and left/right bias, and tracks the effect of ride-height adjustments in real time. Stores per-event setup snapshots with alignment, sway bars, dampers, and tire data.

- ⬜ **Brake Bias and Thermal Calculator** _(stubbed)_: Computes brake bias from master cylinder, caliper piston area, pad mu, and rotor radius, and estimates rotor temperature rise per stop from kinetic energy and cooling coefficients. Plots bias against deceleration and weight transfer.

- ⬜ **Aero CdA and Downforce Estimator** _(stubbed)_: Solves CdA and ClA from coast-down runs, constant-speed power data, or wind-tunnel CSV, and models drag, lift, and balance shifts versus ride height and yaw. Predicts top speed and braking-distance delta for a setup change.

- ⬜ **Telemetry Log Viewer** _(stubbed)_: Loads MoTeC i2, AiM, RaceCapture, VBox, and generic CSV logs into a multi-channel scope with math channels, lap splits, and X-Y plots. Overlays multiple laps, computes time-loss histograms, and exports synchronized video frames.

- ⬜ **GPS Lap and Sector Analyzer** _(stubbed)_: Builds a track map from GPS traces, auto-detects start/finish and segments, and reports per-sector deltas, theoretical best, and ideal racing line offset. Supports outlap/inlap trimming and weather-tagged stint stats.

- ⬜ **Race Strategy Planner** _(stubbed)_: Models fuel burn, tire deg, pit-stop loss, and safety-car probability to optimize stint length and tire choice across a race distance. AI-integrated what-if suggestions propose strategy alternatives from historical stint data.

- ⬜ **Vehicle Dynamics Simulator** _(stubbed)_: Runs quarter-car, bicycle, and seven-post models with user-defined mass, geometry, tires, and aero to simulate skidpad, slalom, and acceleration tests. Outputs lateral G, yaw response, and load transfer for comparison against logged data.

- ⬜ **Quarter-Mile Predictor** _(stubbed)_: Estimates 0-60, 60-foot, ET, and trap speed from weight, power curve, drivetrain loss, tire grip, gearing, and drag, with launch-RPM and clutch-slip sweeps. Reverse-solves for the power needed to hit a target ET.

- ⬜ **Wheel and Tire Fitment Calculator** _(stubbed)_: Computes rolling circumference, speedometer error, fender clearance, scrub radius shift, and load-rating delta when changing wheel offset, width, or tire size. Visualizes the resulting wheel pose inside a fender outline.

- ⬜ **EV Pack and Range Calculator** _(stubbed)_: Designs series/parallel battery configurations from cell datasheets, computes pack voltage, capacity, internal resistance, and sag under load, and estimates range from a drive cycle. Models regen recovery and DC fast-charge time curves.

- ⬜ **Cooling System Calculator** _(stubbed)_: Sizes radiator core area, fan CFM, oil cooler, and intercooler from engine heat rejection, coolant flow, and ambient conditions. Plots coolant and IAT temperatures over a simulated track stint.

- ⬜ **VIN Decoder and Service Notebook** _(stubbed)_: Decodes VINs to year/make/model/engine/build options against an offline NHTSA-derived database, and keeps a maintenance log with parts, fluids, torque specs, and mileage reminders per vehicle.

- ⬜ **VIN Decoder & Service Schedule** _(stubbed)_: Decodes 17-character VINs against NHTSA vPIC data to identify make, model, year, engine, plant, and equipment. Generates the OEM-recommended maintenance schedule with mileage and time intervals, due-soon alerts, and per-vehicle service history.

- ⬜ **OBD-II Live Dashboard** _(stubbed)_: Connects to ELM327 (USB, Bluetooth, Wi-Fi) adapters to stream PIDs in real time: RPM, MAP, MAF, O2, fuel trims, coolant, intake air, timing advance. Logs sessions to CSV/Parquet and overlays multiple runs for diagnostic comparison.

- ⬜ **DTC Fault-Code Lookup** _(stubbed)_: Resolves OBD-II generic and manufacturer-specific P/B/C/U codes with descriptions, common causes, and ranked fix probabilities. AI-assisted triage cross-references freeze-frame data, mileage, and prior repairs to suggest a diagnostic order.

- ⬜ **Service Log & Receipts** _(stubbed)_: Per-vehicle ledger of oil changes, tire rotations, fluid flushes, filters, brake jobs, and timing belts. Captures odometer, parts numbers, fluid specs, torque values, cost, vendor, and scanned receipts; exports a complete service history PDF.

- ⬜ **Tire & Wheel Calculator** _(stubbed)_: Converts between metric and inch tire sizes, computes overall diameter, sidewall height, revs-per-mile, and speedometer error from stock. Checks load and speed ratings, wheel offset/backspace fitment, and tire pressure recommendations by load.

- ⬜ **Brake Pad & Rotor Reference** _(stubbed)_: Cross-references pad and rotor part numbers across FMSI, ATE, Brembo, Akebono, EBC, and OEM catalogs. Tracks pad thickness, rotor min-thickness, bedding procedures, and per-axle wear logs with projected replacement mileage.

- ⬜ **Fluids & Capacities Database** _(stubbed)_: Engine oil grade and capacity, coolant type, ATF/CVT/MTF, brake fluid spec, power steering, differential, transfer case, and refrigerant charge by vehicle. Flags incompatible mixes (e.g., DOT 3/4/5, OAT/IAT coolants, PAG vs POE oils).

- ⬜ **Torque Spec & Fastener Lookup** _(stubbed)_: OEM torque specifications for wheel lugs, head bolts, cam caps, suspension, brake calipers, and oil drain plugs. Includes torque-angle sequences, reuse rules for stretch bolts, and unit conversion between ft-lb, Nm, and in-lb.

- ⬜ **Wiring Diagram & Pinout Browser** _(stubbed)_: Renders interactive vehicle wiring diagrams with connector pinouts, ground points, fuse box maps, and CAN bus topology. Supports search by circuit, component, or DTC and overlays measured voltages from a connected multimeter or scope.

- ⬜ **CAN Bus & UDS Inspector** _(stubbed)_: Sniffs raw CAN frames at configurable baud rates, decodes against DBC files, and performs UDS/KWP2000 service requests (read DTCs, clear codes, read DIDs, actuator tests). Includes bit-rate detection and J1939 support for diesels.

- ⬜ **Motorcycle Chain & Sprocket Sizer** _(stubbed)_: Computes chain length, sprocket ratios, final drive, top speed per RPM, and gearing changes by tooth count. Validates pitch (520/525/530, 428), o-ring vs x-ring compatibility, and tracks chain stretch from pin-to-pin measurements.

- ⬜ **Valve Clearance & Shim Calculator** _(stubbed)_: Records measured intake/exhaust valve clearances for shim-under-bucket, shim-over-bucket, and screw-adjust heads. Computes the exact replacement shim sizes from current shim thickness and target clearance, with shim inventory tracking.

- ⬜ **Carburetor & EFI Tuning Workbench** _(stubbed)_: Jet size selector by altitude, temperature, and humidity for common carbs (Mikuni, Keihin, Dell'Orto). For EFI, imports fuel and ignition tables (Tunerstudio, EcuTek, HPTuners exports) and visualizes AFR/timing maps against logged data.

- ⬜ **Bicycle Gear-Inch Calculator** _(stubbed)_: Computes gear inches, gain ratio, development, and speed-at-cadence for any chainring/cog/wheel combination. Compares 1x, 2x, 3x, and internally-geared hubs side-by-side; flags cross-chain and unusable overlapping gears.

- ⬜ **Frame Geometry Comparator** _(stubbed)_: Overlays stack, reach, head/seat tube angles, BB drop, chainstay, wheelbase, and standover across multiple bicycles or motorcycles. Computes effective top tube and saddle/bar position deltas to predict fit changes between frames.

- ⬜ **Parts Compatibility Checker** _(stubbed)_: Validates bottom bracket, headset, axle, brake mount, derailleur hanger, freehub, and seatpost standards against frame and component specs. For motos and cars, checks bolt patterns, hub bores, caliper mounts, and ECU/harness compatibility.

- ⬜ **Tire Wear & Alignment Tracker** _(stubbed)_: Logs tread depth at multiple points per tire, projects remaining life by mileage, and flags wear patterns (inner/outer edge, cupping, feathering) indicative of toe, camber, or pressure issues. Tracks alignment specs before and after adjustment.

- ⬜ **Battery & Charging-System Analyzer** _(stubbed)_: Records resting voltage, cranking voltage, alternator output under load, and parasitic draw measurements over time. Estimates lead-acid, AGM, and Li-ion battery health from internal-resistance and CCA test results.

- ⬜ **Fuel Economy & Cost-of-Ownership Log** _(stubbed)_: Per-fillup MPG/L-per-100km tracking with rolling averages, plus depreciation, insurance, registration, fuel, parts, and labor totals. Computes cost-per-mile and break-even points for repairs vs replacement.

- ⬜ **Recall & TSB Watcher** _(stubbed)_: Monitors NHTSA, Transport Canada, and manufacturer feeds for open recalls and Technical Service Bulletins matching saved VINs. Surfaces affected components, remedy status, and applicable production date ranges.

- ⬜ **Repair Procedure & Torque-Sequence Viewer** _(stubbed)_: Step-by-step disassembly/reassembly guides with torque sequences, special-tool callouts, fluid refill amounts, and required adapters. AI summarizes long FSM passages and answers free-form questions grounded in the loaded manual.


#### Robotics and 3D

- ⬜ **Robot description editor** _(stubbed)_: Loads, edits, and previews URDF, SDF, and MJCF robot models with live joint sliders, link inspection, and inertial/collision/visual geometry overlays. AI-integrated generation of URDF skeletons from natural-language descriptions or CAD assembly imports.

- ⬜ **Kinematics solver** _(stubbed)_: Computes forward and inverse kinematics for serial, parallel, and branching manipulators with selectable backends (KDL, IKFast, TRAC-IK, numerical Jacobian) and exposes manipulability, condition number, and joint-limit margin per pose.

- ⬜ **Path planner** _(stubbed)_: Plans collision-free joint and Cartesian paths using sampling-based (RRT, RRT*, PRM, BIT*), grid-based (A*, D* Lite), and optimization-based (CHOMP, STOMP, TrajOpt) planners with constraint plugins for orientation, dual-arm, and mobile bases.

- ⬜ **Trajectory designer** _(stubbed)_: Authors Cartesian and joint-space trajectories from waypoints with B-spline, quintic, minimum-jerk, and time-optimal (TOPP-RA) parameterization. Plots position, velocity, acceleration, and jerk envelopes against actuator limits.

- ⬜ **Reachability analyzer** _(stubbed)_: Sweeps the workspace of a manipulator on a configurable voxel grid and produces reachability, dexterity, and manipulability heatmaps with collision-aware filtering and base-placement optimization.

- ⬜ **Collision checker** _(stubbed)_: Performs broad and narrow-phase collision and distance queries between robot links, meshes, point clouds, primitives, and swept volumes via FCL/HPP-FCL with per-pair allowed-collision matrices.

- ⬜ **Physics simulator** _(stubbed)_: Drives rigid-body and articulated-system simulation through MuJoCo, Bullet, Drake, or Genesis backends with contact dynamics, joint friction, sensor models, and headless batch rollouts for policy evaluation and domain randomization.

- ⬜ **3D scene viewer** _(stubbed)_: Renders meshes, point clouds, coordinate frames, robot states, sensor readings, occupancy grids, and trajectories in a unified scene graph with time scrubbing, picking, and snapshot export.

- ⬜ **Point cloud workbench** _(stubbed)_: Loads PLY, PCD, LAS, E57, and depth-image clouds, performs voxel downsampling, outlier removal, normal estimation, plane/cluster segmentation, and pairwise/global registration (ICP, GICP, NDT, Teaser++). AI-integrated semantic and instance segmentation against open-vocabulary prompts.

- ⬜ **CAD import and conversion** _(stubbed)_: Converts STEP, IGES, JT, and Parasolid bodies to triangle meshes or implicit surfaces with topology preservation, assembly extraction into URDF link trees, and inertial-tensor computation from solid density.

- ⬜ **Motion capture studio** _(stubbed)_: Ingests BVH, FBX, C3D, and live OptiTrack, Vicon, and Rokoko streams; cleans marker gaps, solves skeletons, retargets to arbitrary URDFs, and exports joint trajectories for replay or imitation learning.

- ⬜ **Camera calibrator** _(stubbed)_: Performs intrinsic, extrinsic, stereo, and rolling-shutter calibration from checkerboard, ChArUco, or AprilTag captures, with reprojection error heatmaps, pose-coverage prompts, and Kalibr-compatible export.

- ⬜ **Hand-eye and base calibrator** _(stubbed)_: Solves AX=XB and AX=ZB calibrations for camera-on-arm and camera-on-base mounts using Tsai-Lenz, Park-Martin, and Daniilidis dual-quaternion methods with bootstrap uncertainty estimates.

- ⬜ **SLAM workbench** _(stubbed)_: Runs visual, LIDAR, and visual-inertial SLAM pipelines (ORB-SLAM3, RTAB-Map, LIO-SAM, Kiss-ICP) over recorded sessions with keyframe inspection, loop-closure editing, and exported map output to OctoMap, mesh, or Gaussian splat.

- ⬜ **Transform tree inspector** _(stubbed)_: Visualizes the live TF/TF2 coordinate-frame graph, flags missing, stale, or cyclic transforms, and edits static transform publishers with side-by-side numeric and 3D representation.

- ⬜ **ROS bag and MCAP inspector** _(stubbed)_: Opens .bag and .mcap recordings, indexes topics and message types, plots numeric channels, decodes image and point-cloud streams, and replays selected slices into the scene viewer or simulator.

- ⬜ **Sensor stream viewer** _(stubbed)_: Subscribes to live LIDAR, depth, IMU, force/torque, encoder, and camera streams over ROS/ROS2, Zenoh, MQTT, and LCM with per-channel decimation, synchronized recording, and back-pressure stats.

- ⬜ **Computer vision pipeline** _(stubbed)_: Composes OpenCV and Kornia operators (detection, segmentation, optical flow, feature matching, pose estimation) as a runnable graph with live preview against cameras, video, or bag playback. AI-integrated swap-in of vision foundation models (SAM, DINOv2, YOLO, Grounding DINO) as graph nodes.

- ⬜ **Pose estimation studio** _(stubbed)_: Estimates 6D object poses from RGB and RGB-D inputs using FoundationPose, MegaPose, fiducial markers (AprilTag, ArUco), and template matching, with ground-truth annotation and refinement tooling.

- ⬜ **Photogrammetry and radiance-field workbench** _(stubbed)_: Reconstructs scenes via structure-from-motion (COLMAP, OpenMVG), NeRF (nerfstudio), and Gaussian splatting (gsplat, SuGaR) from image sets, with mesh, point cloud, and splat export plus pose-graph editing.

- ⬜ **Octomap and occupancy editor** _(stubbed)_: Builds, edits, and queries 2D and 3D occupancy grids, OctoMaps, and ESDFs with frustum carving, raycasting, dilation, and probabilistic thresholding for navigation and manipulation planning.

- ⬜ **Gripper and end-effector designer** _(stubbed)_: Parametrically generates parallel-jaw, suction, soft, and adaptive grippers, attaches them to the active URDF, and previews antipodal and force-closure grasp candidates on target meshes or point clouds. AI-integrated grasp ranking via GraspNet-style scorers.

- ⬜ **Gait and legged-locomotion designer** _(stubbed)_: Authors stepping patterns, contact schedules, and CoM/ZMP references for bipeds and quadrupeds, tunes MPC and whole-body controllers, and plays results back in the physics simulator.

- ⬜ **Behavior tree and FSM editor** _(stubbed)_: Visually composes behavior trees and finite-state machines for robot autonomy with BT.CPP, PyTrees, and SMACH export, blackboard inspection, and live-tick debugging against a running stack.

- ⬜ **Slicer for additive manufacturing** _(stubbed)_: Slices STL, 3MF, and STEP for FDM, SLA, and SLS with adaptive layer height, tree and lattice supports, infill patterns, and G-code or printer-native output, including simulated toolpath preview and collision check against printer envelope.


#### Energy systems

- ⬜ **Solar Resource Fetcher** _(stubbed)_: Pulls TMY3, PVGIS, and NSRDB irradiance, ambient temperature, and wind data for any latitude/longitude with offline caching. Exposes GHI/DNI/DHI hourly series and monthly summaries for downstream PV and thermal tools.

- ⬜ **Sun Path & Shading Analyzer** _(stubbed)_: Plots solar altitude/azimuth across the year on a stereographic chart and overlays a user-drawn horizon (terrain, buildings, trees). Computes per-hour shade losses and annual energy lost to obstructions.

- ⬜ **PV Array Layout Designer** _(stubbed)_: Lays out modules on a rooftop or ground plot with row spacing, tilt, and azimuth, computing inter-row self-shading by ground coverage ratio. Reports kWp density and structural footprint.

- ⬜ **Tilt & Azimuth Optimizer** _(stubbed)_: Sweeps tilt and azimuth across the TMY year to find the angle that maximizes annual yield, winter-only yield, or a custom-weighted load match. Returns isocontour plots and sensitivity bands.

- ⬜ **Module I-V Curve Simulator** _(stubbed)_: Builds the single-diode-model I-V and P-V curves for any datasheet module across irradiance and cell temperature, including bypass-diode behavior under partial shading. Useful for picking MPPT windows and string sizing.

- ⬜ **String & Inverter Sizing** _(stubbed)_: Solves Vmp/Voc string windows against inverter MPPT range and absolute max DC voltage at record-low ambient (per NEC 690.7). Recommends string length, parallel count, and DC/AC ratio with AI-assisted starting point from module + inverter pair.

- ⬜ **MPPT Tracker Modeler** _(stubbed)_: Simulates perturb-and-observe, incremental conductance, and sweep-based MPPT algorithms against time-varying irradiance and shading. Quantifies tracking efficiency and mismatch losses versus a global-MPP oracle.

- ⬜ **Inverter Efficiency Mapper** _(stubbed)_: Loads CEC/Sandia inverter coefficients and renders the efficiency surface across DC input voltage and load fraction. Computes weighted CEC efficiency and predicts clipping losses at a chosen DC/AC ratio.

- ⬜ **Load Profile Builder** _(stubbed)_: Constructs 8760-hour residential, commercial, or off-grid load shapes from appliance schedules, occupancy, HVAC, EV charging, and well pumps. Imports CSV smart-meter data and decomposes into base, thermal, and discretionary components.

- ⬜ **Battery Bank Sizer** _(stubbed)_: Sizes a battery bank for target days of autonomy, depth-of-discharge limits, and round-trip efficiency, with chemistry-specific derating (LiFePO4, NMC, lead-acid AGM, flow). Reports usable kWh, cycle life at the chosen DoD, and replacement cadence.

- ⬜ **Battery Chemistry Comparator** _(stubbed)_: Side-by-side comparison of energy density, cycle life vs DoD, calendar aging, temperature derating, C-rate limits, and $/kWh-cycle across chemistries. Highlights the break-even crossover for a given duty cycle.

- ⬜ **Charge Controller Selector** _(stubbed)_: Picks PWM vs MPPT controllers and sizes them against array Isc, Voc-cold, and battery bus voltage with the required safety margins. Flags when paralleling controllers or moving to a higher bus voltage beats a single oversize unit.

- ⬜ **Off-Grid System Sizer** _(stubbed)_: End-to-end sizing of array, battery, controller, and inverter against a load profile and worst-month solar resource, including loss-of-load probability. AI-integrated suggestion of a starting topology from constraints (budget, autonomy, climate).

- ⬜ **Hybrid Dispatch Simulator** _(stubbed)_: Hourly dispatch of PV, battery, grid, wind, and diesel/propane generator under a chosen control strategy (cycle charging, load following, peak shaving). Reports renewable fraction, generator runtime, fuel use, and unmet load.

- ⬜ **Wind Resource & Weibull Fitter** _(stubbed)_: Fits Weibull shape and scale parameters to measured or reanalysis wind speed time series and extrapolates to hub height via log or power law. Plots wind rose, turbulence intensity, and exceedance curves.

- ⬜ **Wind Turbine Power Curve Studio** _(stubbed)_: Loads or sketches power curves, applies air density correction, hub-height extrapolation, and wake losses, then crosses with the site resource to compute annual energy production and capacity factor.

- ⬜ **Grid-Tie & Net Metering Calculator** _(stubbed)_: Models monthly bill impact under net metering, net billing, time-of-use, and demand charges, including export caps and true-up. Compares self-consumption vs full export economics.

- ⬜ **DC/AC Cable & Conduit Sizer** _(stubbed)_: Sizes conductors for voltage drop, ampacity, and temperature derating under NEC/IEC tables, including continuous-load 1.25x factors and conduit fill. Outputs a bill of materials with copper vs aluminum cost comparison.

- ⬜ **Single-Line Diagram Editor** _(stubbed)_: Schematic editor with PV-specific blocks (combiner, RSD, disconnect, inverter, ATS, meter, breaker panel) that auto-labels wire gauges and OCPD ratings from upstream calculations. Exports PDF for permit submittal.

- ⬜ **Soiling, Snow & Degradation Model** _(stubbed)_: Applies monthly soiling losses, snow coverage from temperature and snowfall, and linear or step-wise module degradation across a 25-year horizon. Feeds the LCOE and lifetime-yield estimators.

- ⬜ **LCOE & Payback Analyzer** _(stubbed)_: Computes levelized cost of energy, net present value, IRR, and simple/discounted payback with degradation, O&M, inverter replacements, ITC/rebates, and escalating utility rates. Sensitivity tornado on the top cost drivers.

- ⬜ **Thermal Performance Modeler** _(stubbed)_: Estimates cell temperature from POA irradiance, ambient, and wind via Sandia, NOCT, or PVsyst models, then applies temperature coefficients to module power. Compares roof-mount, ground-mount, and bifacial backside ventilation.

- ⬜ **Protection & Grounding Calculator** _(stubbed)_: Sizes fuses, breakers, and DC arc-fault/rapid-shutdown devices, plus equipment grounding conductor and bonding per NEC 690/705 and IEC 62548. Verifies short-circuit current paths and series fuse compatibility.

- ⬜ **Demand Response & TOU Optimizer** _(stubbed)_: Schedules battery charge/discharge and deferrable loads (EV, water heater, HVAC pre-cool) against TOU tariffs and demand charges. Returns the optimal dispatch and the marginal value of an additional kWh of storage.


#### Chemistry and lab

- ⬜ **Molecule editor** _(stubbed)_: 2D structure drawing with atom, bond, ring, and template tools; keeps SMILES, InChI, and molfile representations live-synced as you draw. AI-integrated parsing of pasted text, hand-drawn sketches, or photographs of structures into editable molecules.

- ⬜ **3D molecule viewer** _(stubbed)_: Renders structures from SMILES, MOL, SDF, PDB, and CIF in ball-and-stick, space-fill, ribbon, and surface modes. Supports measurement of distances, angles, dihedrals, electron density isosurfaces, and animated trajectories from MD or IRC files.

- ⬜ **Name and identifier translator** _(stubbed)_: Round-trips between SMILES, canonical SMILES, InChI, InChIKey, IUPAC name, CAS number, and common synonyms. AI-integrated naming of arbitrary structures and structure-from-name resolution for ambiguous or trivial inputs.

- ⬜ **Reaction balancer** _(stubbed)_: Balances arbitrary chemical equations including redox half-reactions, disproportionation, and net-ionic forms; reports coefficients, atom counts, charge balance, and oxidation state changes per atom.

- ⬜ **Stoichiometry calculator** _(stubbed)_: Computes limiting reagent, theoretical yield, percent yield, atom economy, and mass/mol/volume/concentration conversions for arbitrary balanced reactions; handles solvates, hydrates, and purity adjustments.

- ⬜ **Retrosynthesis planner** _(stubbed)_: AI-integrated decomposition of a target molecule into candidate precursors with scored multi-step routes, named reactions, suggested conditions, and cross-references to literature procedures.

- ⬜ **Forward reaction predictor** _(stubbed)_: Predicts likely products and selectivities from reactants, reagents, solvent, and temperature; ranks side products, flags regiochemistry and stereochemistry concerns, and offers AI-integrated arrow-pushing mechanism walkthroughs.

- ⬜ **NMR simulator** _(stubbed)_: Predicts 1H, 13C, DEPT, COSY, HSQC, and HMBC spectra from a structure; reports chemical shifts, multiplicities, coupling constants, and integrations, with overlay against an imported experimental spectrum.

- ⬜ **Vibrational spectrum simulator** _(stubbed)_: Predicts IR and Raman bands from structure via group contribution or computed force constants; annotates functional group regions and matches against measured JCAMP-DX traces.

- ⬜ **Mass spectrum workbench** _(stubbed)_: Predicts EI, ESI, and APCI fragmentation trees; computes isotope envelopes and exact masses; matches an observed spectrum against a candidate structure or a local library, with AI-integrated formula generation from accurate mass.

- ⬜ **Spectrum viewer** _(stubbed)_: Loads JCAMP-DX, mzML, nmrML, Bruker, and CSV traces with peak picking, integration, baseline and phase correction, deconvolution, and side-by-side overlay of predicted versus measured.

- ⬜ **Periodic table explorer** _(stubbed)_: Interactive table with isotopes, electron configurations, oxidation states, ionization energies, electronegativities, atomic radii, abundances, and crystal structures; filterable and plottable by any property.

- ⬜ **Electronic lab notebook** _(stubbed)_: Date-stamped entries with embedded structures, procedures, observations, attached spectra, photographs, and reaction templates; cross-links reagents, products, equipment, and hazards so a synthesis is reproducible from the record alone.

- ⬜ **Chemical inventory** _(stubbed)_: Tracks reagents by CAS, location, container, amount, lot, purity, expiry, and supplier; logs check-outs against notebook entries, flags peroxide-formers past their date, and generates reorder lists.

- ⬜ **Safety data lookup** _(stubbed)_: Pulls SDS data (GHS pictograms, H and P statements, flash point, LD50, exposure limits) for a compound by name or CAS. AI-integrated summary of the hazards, PPE, waste stream, and emergency response relevant to the specific procedure being planned.

- ⬜ **Incompatibility checker** _(stubbed)_: Cross-references a reagent and waste list against storage and reactivity matrices; warns on oxidizer-fuel pairs, water-reactives, peroxide formers, shock sensitives, and forbidden waste combinations before they are mixed.

- ⬜ **Solution and buffer calculator** _(stubbed)_: Computes molarity, molality, normality, mass percent, ppm, and serial dilutions; designs buffers via Henderson-Hasselbalch with ionic-strength and activity corrections, and outputs a weigh-and-dissolve recipe.

- ⬜ **Property predictor** _(stubbed)_: Estimates pKa, logP, logD, solubility, melting point, boiling point, vapor pressure, and basic ADMET descriptors from structure; reports species distribution versus pH and confidence intervals from the underlying model.

- ⬜ **Crystallography viewer** _(stubbed)_: Loads CIF and PDB files and visualizes unit cell, symmetry operations, asymmetric unit, packing diagrams, and Miller planes; simulates powder XRD patterns and indexes a measured pattern against candidate cells.

- ⬜ **Conformer and minimizer** _(stubbed)_: Runs UFF, MMFF, GAFF, and AMBER energy minimization and stochastic conformer search; reports relative energies, RMSDs, and Boltzmann populations and exports the lowest-energy ensemble for downstream calculation.

- ⬜ **Quantum chemistry interface** _(stubbed)_: Generates input files for Psi4, ORCA, NWChem, and xTB and parses outputs for energies, optimized geometries, frequencies, NMR shieldings, and orbitals. AI-integrated suggestion of method and basis from system size, element set, and target property.

- ⬜ **Kinetics and calibration fitter** _(stubbed)_: Fits concentration-versus-time data to zeroth, first, second-order, Arrhenius, and Michaelis-Menten models, and fits linear or weighted calibration curves; reports rate constants, half-lives, LOD, LOQ, R squared, and back-calculated unknowns.

- ⬜ **Chromatography method designer** _(stubbed)_: Plans HPLC, UPLC, and GC methods from analyte properties; predicts retention, resolution, and peak shape under a chosen column, mobile phase, gradient, and temperature. AI-integrated suggestion of starting conditions from a target analyte mixture.

- ⬜ **Polymer builder** _(stubbed)_: Constructs homopolymer, copolymer, and block-copolymer chains from monomer repeats with chosen tacticity and dispersity; reports Mn, Mw, PDI, and predicts Tg, Tm, and solubility parameter from group contributions.

- ⬜ **Apparatus and glassware sketcher** _(stubbed)_: Schematic editor for distillation, reflux, Schlenk, flow, and electrochemical setups with a library of standard glassware, joints, condensers, and stirrers; outputs an annotated diagram that drops directly into the notebook entry.


#### Biology and bioinformatics

- ⬜ **Sequence Editor** _(stubbed)_: Multi-format viewer and editor for nucleotide and protein sequences (FASTA, GenBank, EMBL, FASTQ). Feature annotations, translation tracks, codon highlighting, GC content overlay, and per-base quality display for reads.

- ⬜ **Local BLAST Console** _(stubbed)_: Runs blastn, blastp, blastx, tblastn, and tblastx against locally indexed databases (NCBI nr/nt mirrors, RefSeq, UniProt, custom). Tabular and pairwise output, E-value and bitscore filtering, HSP visualization, and direct drop-back into the sequence editor.

- ⬜ **Multiple Sequence Alignment** _(stubbed)_: Wraps MUSCLE, MAFFT, Clustal Omega, and T-Coffee with a common UI. Conservation tracks, consensus extraction, gap-stripping, profile-to-profile alignment, and export to PHYLIP, NEXUS, and Stockholm.

- ⬜ **Pairwise Aligner** _(stubbed)_: Smith-Waterman and Needleman-Wunsch with configurable scoring matrices (BLOSUM, PAM, custom), gap penalties, and seed-and-extend for long sequences. Dot-plot view for spotting repeats, inversions, and duplications.

- ⬜ **Genome Browser** _(stubbed)_: Track-based viewer for assemblies and annotations (FASTA + GFF3, GTF, BED, BigWig, VCF, BAM, CRAM). Zoom from chromosome to base, synteny ribbons across loaded genomes, and read pileups with mismatch coloring.

- ⬜ **Phylogenetic Tree Builder** _(stubbed)_: Constructs trees with neighbor-joining, maximum-likelihood (RAxML/IQ-TREE-style), and Bayesian (MrBayes-style) backends from an alignment. Bootstrap support, ultrafast bootstrap, model selection (ModelTest), and interactive radial/rectangular/circular tree rendering with reroot, collapse, and taxon coloring.

- ⬜ **Protein Structure Viewer** _(stubbed)_: 3D viewer for PDB, mmCIF, and MMTF with cartoon, surface, stick, and electrostatics renderings. Sequence-to-structure linkage, residue selection by SQL-like query, B-factor and conservation coloring, and side-by-side superposition with TM-align/CE.

- ⬜ **Structure Predictor** _(stubbed)_: Local AlphaFold/ESMFold/RoseTTAFold-style monomer and complex structure prediction from sequence, with confidence (pLDDT, PAE) overlays. AI-integrated prompting for template selection and multimer stoichiometry from the project context.

- ⬜ **Primer Designer** _(stubbed)_: Designs PCR, qPCR, sequencing, and cloning primers from a target region with Primer3-style Tm, GC, hairpin, self-dimer, and cross-dimer checks. Specificity check against a chosen genome via the local BLAST console; AI-integrated suggestion of a starting region when only the goal (e.g., spanning an exon junction) is given.

- ⬜ **Plasmid Map Editor** _(stubbed)_: Circular and linear vector design with feature annotation, restriction site mapping, ORF detection, and in-silico cloning (Gibson, Golden Gate, restriction-ligation, In-Fusion). Reads and writes GenBank and SnapGene .dna; renders publication-grade maps.

- ⬜ **Restriction Enzyme Mapper** _(stubbed)_: Catalogs enzyme cut sites across a sequence with isoschizomer awareness, methylation sensitivity, and star activity flags. Picks single, double, and triple digests for a desired fragment pattern and simulates the resulting gel.

- ⬜ **CRISPR Guide Designer** _(stubbed)_: Designs sgRNAs for Cas9, Cas12a, base editors, and prime editors with on-target efficiency scoring (Doench, DeepCRISPR) and genome-wide off-target enumeration. Repair-template builder for HDR knock-ins and library design for pooled screens.

- ⬜ **Codon Optimizer** _(stubbed)_: Re-codes a protein sequence for expression in a chosen host (E. coli, yeast, CHO, HEK, plant, custom codon table) while honoring constraints: avoided restriction sites, avoided repeats, target GC window, and preserved regulatory motifs. Multi-objective with tunable weights.

- ⬜ **Read Mapper and Variant Caller** _(stubbed)_: Short and long read alignment (BWA-MEM, minimap2, bowtie2) followed by variant calling (bcftools, DeepVariant-style, Strelka-style) with VCF annotation against gene models. QC, coverage, duplicate marking, and indel realignment.

- ⬜ **Expression Analysis** _(stubbed)_: RNA-seq and single-cell pipeline: quantification (salmon/kallisto/STAR), differential expression (DESeq2/edgeR/limma-voom), PCA/UMAP/t-SNE, heatmaps, volcano plots, and GSEA against MSigDB. Reads in counts matrices or runs from FASTQ end-to-end.

- ⬜ **Pathway and Network Viewer** _(stubbed)_: Loads KEGG, Reactome, WikiPathways, and BioCyc maps and overlays expression or variant data onto nodes. Force-directed and pathway-layout rendering for protein-protein interaction networks (STRING, BioGRID) with enrichment analysis.

- ⬜ **Gene Ontology Enrichment** _(stubbed)_: Computes GO term, KEGG pathway, and Reactome enrichment for a gene list with hypergeometric and GSEA statistics, FDR correction, and revigo-style redundancy reduction. Output ties back to the pathway viewer.

- ⬜ **Molecular Docking** _(stubbed)_: Protein-ligand docking (AutoDock Vina, smina, GNINA) and protein-protein docking (HADDOCK-style) with binding-site detection, pose scoring, and interaction-diagram rendering. Reads SMILES, MOL2, SDF, and PDB ligands; integrates with the chemistry specialist for ligand prep.

- ⬜ **Sanger Trace Viewer** _(stubbed)_: Reads .ab1 and .scf chromatograms, calls and edits bases, trims by quality, assembles contigs from forward and reverse reads, and compares the consensus against a reference with mismatch highlighting.

- ⬜ **Gel Simulator** _(stubbed)_: Predicts agarose and polyacrylamide gel banding from sequences, restriction digests, or PCR product lists at a given percentage and ladder. Useful for planning a digest before pipetting and for sanity-checking an unexpected band.

- ⬜ **Lab Protocol Planner** _(stubbed)_: Assay and protocol authoring with reagent table, dilution calculator (C1V1 = C2V2, serial dilutions, master mixes), buffer recipe builder, timer chaining, and step-by-step worksheet export. AI-integrated draft of a starting protocol from an experimental goal.

- ⬜ **Sequence Property Calculator** _(stubbed)_: Computes molecular weight, isoelectric point, extinction coefficient, hydrophobicity (Kyte-Doolittle, GRAVY), instability index, codon adaptation index, and Tm for DNA/RNA/protein inputs. Batch mode over FASTA.

- ⬜ **Variant Annotator and Interpreter** _(stubbed)_: Annotates VCFs with gene models, consequence (VEP/SnpEff-style), population frequencies (gnomAD), clinical significance (ClinVar), and predicted impact (SIFT, PolyPhen, CADD, AlphaMissense). Filters and ranks for follow-up; outputs an annotated VCF and a candidate table.

- ⬜ **Metagenomics Classifier** _(stubbed)_: Taxonomic profiling of microbiome reads with Kraken2/Bracken, Centrifuge, and MetaPhlAn backends, plus functional profiling (HUMAnN). Alpha/beta diversity, stacked barplots, and differential abundance against sample metadata.

- ⬜ **Bioinformatics Format Converter** _(stubbed)_: One-stop conversion across FASTA, FASTQ, GenBank, EMBL, GFF/GTF, BED, VCF, SAM/BAM/CRAM, PDB/mmCIF, NEXUS, PHYLIP, Stockholm, and Newick. Validates input, flags malformed records, and preserves annotations across compatible formats.

- ⬜ **Antibody and Epitope Designer** _(stubbed)_: Linear and conformational B-cell epitope prediction, MHC-I/II binding prediction (NetMHCpan-style), antibody CDR annotation (Kabat, Chothia, IMGT numbering), humanization suggestions, and developability flags (aggregation, deamidation, glycosylation sites).

- ⬜ **Microscopy Image Analyzer** _(stubbed)_: Opens TIFF, OME-TIFF, CZI, ND2, and LIF stacks with channel splitting, z-projection, and time-series scrubbing. Segmentation (Cellpose, StarDist), cell and colony counting, intensity quantification per ROI, and CSV export of measurements.


#### Microbiology, virology, and immunology

- ⬜ **Sequence viewer** _(stubbed)_: Renders DNA, RNA, and protein sequences with ORF detection, six-frame translation, restriction sites, GC content tracks, and codon usage heatmaps. Supports FASTA, GenBank, EMBL, and GFF3 with synteny ribbons across loaded records.

- ⬜ **BLAST runner** _(stubbed)_: Executes local blastn, blastp, blastx, tblastn, and psiblast against bundled or custom databases. Parses results into hit tables, alignment views, and taxonomy breakdowns; supports remote NCBI fallback when offline corpora miss.

- ⬜ **Multiple sequence aligner** _(stubbed)_: Aligns nucleotide or protein sets via MAFFT, MUSCLE, or Clustal Omega with column conservation scoring and consensus extraction. Exports to Stockholm, FASTA, and PHYLIP.

- ⬜ **Genome assembler** _(stubbed)_: Runs SPAdes, Flye, Unicycler, or Canu pipelines for short, long, and hybrid reads with quality trimming via fastp and assembly QC via QUAST and BUSCO.

- ⬜ **Read mapper** _(stubbed)_: Aligns reads to a reference with BWA-MEM, minimap2, or Bowtie2, calls variants via bcftools or DeepVariant, and produces coverage tracks and consensus FASTAs.

- ⬜ **Genome annotator** _(stubbed)_: Predicts CDS, rRNA, tRNA, and CRISPR arrays via Prokka, Bakta, or Pharokka for prokaryotes and phages. Augments with eggNOG, InterProScan, and AMRFinderPlus functional layers.

- ⬜ **Plasmid editor** _(stubbed)_: Edits circular constructs with restriction cloning, Gibson, Golden Gate, and Type IIS workflows. Validates junctions, autosuggests overhangs, and exports SBOL and SnapGene-compatible GenBank.

- ⬜ **Variant caller** _(stubbed)_: Calls SNPs, indels, and structural variants on bacterial, viral, and eukaryotic data with bcftools, GATK, or LoFreq for low-frequency variants in viral quasispecies.

- ⬜ **Pangenome analyzer** _(stubbed)_: Builds core and accessory gene matrices via Roary, Panaroo, or PPanGGOLiN, computes presence-absence, gene gain-loss, and rarefaction curves across isolate sets.

- ⬜ **Strain typer** _(stubbed)_: Runs MLST, cgMLST, wgMLST, spaTyping, serotyping (SISTR, SeqSero, Kaptive), and SNP-based clustering for outbreak investigation. Outputs minimum spanning trees and transmission inference.

- ⬜ **AMR profiler** _(stubbed)_: Screens isolates for resistance genes, plasmid replicons, and point mutations via AMRFinderPlus, CARD-RGI, ResFinder, and PlasmidFinder. Cross-references with EUCAST and CLSI breakpoint tables.

- ⬜ **Virulence factor scanner** _(stubbed)_: Detects virulence determinants and pathogen-associated islands via VFDB, Virulencefinder, IslandViewer, and PHASTER for prophage discovery.

- ⬜ **Taxonomic classifier** _(stubbed)_: Classifies metagenomic reads or MAGs with Kraken2, Bracken, MetaPhlAn, GTDB-Tk, or sourmash and reports relative abundance, alpha and beta diversity.

- ⬜ **Metagenome binner** _(stubbed)_: Bins contigs into MAGs via MetaBAT2, CONCOCT, and DAS_Tool, scores completeness and contamination with CheckM2, and annotates community function via HUMAnN.

- ⬜ **Viral genome workbench** _(stubbed)_: Performs reference-based viral consensus calling, Pangolin or Nextclade lineage assignment, glycoprotein cleavage site detection, and recombination scanning via RDP and 3SEQ.

- ⬜ **Protein structure predictor** _(stubbed)_: Runs AlphaFold, ColabFold, ESMFold, or RoseTTAFold locally for monomer and complex prediction with confidence (pLDDT, PAE) overlays and PDB or mmCIF export.

- ⬜ **Structure viewer** _(stubbed)_: Renders proteins, ligands, and nucleic acids in cartoon, surface, and electrostatic modes via PyMOL or Mol* bindings with structural alignment via TM-align or Foldseek.

- ⬜ **Docking and binding workbench** _(stubbed)_: Runs AutoDock Vina, DiffDock, or HADDOCK for ligand and protein-protein docking; scores poses and computes interface residues for inhibitor and antibody studies.

- ⬜ **Antibody and epitope mapper** _(stubbed)_: Numbers antibody sequences (IMGT, Kabat, Chothia, AHo), models Fv structures via ABodyBuilder, and predicts linear and conformational B-cell and T-cell epitopes (BepiPred, DiscoTope, NetMHCpan).

- ⬜ **HLA typer and neoantigen pipeline** _(stubbed)_: Calls class I and II HLA types from sequencing data via OptiType or arcasHLA and ranks candidate neoepitopes by binding affinity, presentation, and immunogenicity.

- ⬜ **BCR/TCR repertoire analyzer** _(stubbed)_: Processes AIRR-seq data with MiXCR or IgBLAST; computes V(D)J usage, CDR3 diversity (Shannon, Simpson, D50), clonotype tracking, and lineage trees via Change-O.

- ⬜ **Flow cytometry workbench** _(stubbed)_: Loads FCS files, applies compensation, transforms (logicle, arcsinh), gates manually or via FlowSOM and UMAP, and exports population statistics and publication-grade plots.

- ⬜ **Growth curve plotter** _(stubbed)_: Imports plate reader OD or fluorescence time series, fits Gompertz, logistic, and Baranyi models, and reports lag time, max growth rate, carrying capacity, and AUC with blank subtraction and outlier removal.

- ⬜ **MIC and antibiogram analyzer** _(stubbed)_: Computes MIC50, MIC90, and susceptibility distributions from broth or disk diffusion data, applies EUCAST or CLSI breakpoints, and renders cumulative antibiograms and resistance trend dashboards.

- ⬜ **Plaque and CFU counter** _(stubbed)_: Counts plaques, colonies, and zones of inhibition from plate photos with shape and intensity filters, edge artifact rejection, and dilution series back-calculation to titer.

- ⬜ **Restriction and cloning planner** _(stubbed)_: Searches for restriction sites, plans isocaudomer swaps, simulates digests, ligations, Gateway, and TOPO reactions, and predicts agarose gel band patterns.

- ⬜ **qPCR and dPCR analyzer** _(stubbed)_: Ingests amplification curves, computes Cq via second derivative max, performs standard curve, efficiency, and melt analysis, and runs ddCt or Pfaffl relative quantification with reference gene validation (geNorm, NormFinder).

- ⬜ **ELISA and Luminex analyzer** _(stubbed)_: Fits 4PL and 5PL standard curves with weighted regression, back-calculates analyte concentrations, flags out-of-range wells, and computes inter and intra-plate CVs.

- ⬜ **Mass spec and proteomics pipeline** _(stubbed)_: Runs MaxQuant, FragPipe, or DIA-NN on shotgun proteomics data, identifies and quantifies peptides and PTMs, and links results to UniProt and STRING for pathway context.

- ⬜ **Metabolic model workbench** _(stubbed)_: Builds and curates genome-scale models via CarveMe or gapseq, runs FBA, FVA, and pFBA with COBRApy, and predicts essential genes and growth media.

- ⬜ **Pathway and ontology enricher** _(stubbed)_: Maps gene or protein sets onto KEGG, Reactome, GO, and MetaCyc with hypergeometric or GSEA enrichment, and renders pathway diagrams with expression overlays.

- ⬜ **Biosecurity screener** _(stubbed)_: Screens designed sequences against select agent and dual-use signature databases (e.g. IGSC-style) and flags hits with risk context before synthesis order export.

- ⬜ **LIMS and sample tracker** _(stubbed)_: Tracks isolates, strains, plates, freezer boxes, passages, and chain of custody with barcode and QR generation, audit trails, and BioSample/SRA submission manifests.

- ⬜ **Lab protocol and ELN** _(stubbed)_: Authors and runs protocols with step timers, reagent inventory deduction, instrument metadata capture, and signed read-only snapshots for compliance and reproducibility.

- ⬜ **Outbreak and transmission tracker** _(stubbed)_: Integrates strain typing, geocoded metadata, and timed phylogenies via Nextstrain or BEAST to infer transmission chains, R_e, and introduction events.

- ⬜ **Vaccine antigen designer** _(stubbed)_: Selects conserved epitopes across strains, scaffolds multi-epitope constructs with linkers and adjuvant fusions, and evaluates population coverage across HLA distributions.

- ⬜ **RNA structure and riboswitch tool** _(stubbed)_: Predicts secondary structures via RNAfold, ViennaRNA, and CONTRAfold, scans for riboswitches and CRISPR scaffolds, and renders dot-bracket and arc diagrams.

- ⬜ **Glycan and post-translational analyzer** _(stubbed)_: Predicts N- and O-glycosylation, phosphorylation, ubiquitination, and signal peptide cleavage via NetNGlyc, NetOGlyc, SignalP, and DeepTMHMM with topology diagrams.


#### Genomics, synthetic biology, and neuroscience

- ⬜ **Construct assembly planner** _(stubbed)_: Plans Gibson, Golden Gate, MoClo, and In-Fusion assemblies from fragment graphs, emits junction overhangs and primer orders, and simulates the final plasmid. Detects illegal internal cut sites for the chosen type IIS enzyme.

- ⬜ **Alignment viewer** _(stubbed)_: Runs MUSCLE, MAFFT, or Clustal alignments, renders conservation and consensus tracks, and builds neighbor-joining or maximum-likelihood phylogenies from the result.

- ⬜ **Read aligner and variant caller** _(stubbed)_: Wraps BWA-MEM, minimap2, bcftools, and DeepVariant for short- and long-read alignment and SNV/indel calling, with BAM and VCF inspection plus depth and coverage QC.

- ⬜ **Single-cell expression viewer** _(stubbed)_: Loads h5ad and loom matrices, computes Scanpy-style PCA, UMAP, and Leiden clustering, and supports marker-gene browsing and cluster annotation against reference atlases.

- ⬜ **Liquid-handler protocol writer** _(stubbed)_: Generates Opentrons OT-2 Python and Hamilton Venus scripts from plate layouts, source/destination maps, and pipetting plans, with tip-tracking and deck-collision validation.

- ⬜ **EEG/MEG analyzer** _(stubbed)_: Loads EDF, BDF, BrainVision, FIF, and BIDS recordings, performs ICA artifact removal, time-frequency decomposition (Morlet, multitaper), source localization, and evoked-response averaging in an MNE-style workflow.

- ⬜ **MRI/fMRI viewer** _(stubbed)_: Renders NIfTI and DICOM volumes with multiplanar reformat, BOLD activation overlays, motion-correction QC, and atlas-based parcellation lookup.

- ⬜ **Spike sorter** _(stubbed)_: Runs Kilosort and MountainSort drift-corrected template matching on Neuropixels and tetrode data, with manual curation of clusters, waveform inspection, and ISI, CCG, and refractory-period diagnostics.

- ⬜ **Calcium imaging analyzer** _(stubbed)_: Performs NoRMCorre motion correction, CaImAn CNMF source extraction, dF/F trace generation, and spike inference from two-photon and miniscope recordings.

- ⬜ **Pose tracker** _(stubbed)_: Trains and runs DeepLabCut and SLEAP keypoint models on behavioral video, with bout segmentation, kinematic feature extraction, and frame-level annotation review.

- ⬜ **Connectome browser** _(stubbed)_: Loads DTI tractography, EM-derived synapse graphs, and Allen and FlyWire connectomes; computes graph metrics, traces multi-hop paths, and renders 3D wiring with cell-type filtering.

- ⬜ **Behavioral assay logger** _(stubbed)_: Captures timestamped event streams synced with video and physiology, encodes trial and block structure, and exports BIDS-compatible session manifests for downstream analysis.

- ⬜ **Stimulus presentation builder** _(stubbed)_: Composes PsychoPy-style visual and auditory paradigms with frame-accurate timing, photodiode and TTL sync output, randomized trial schedules, and per-subject parameterization.

- ⬜ **Neuron and network simulator** _(stubbed)_: Runs NEURON compartmental models and Brian2 integrate-and-fire networks with parameter sweeps, parallel batch execution, and post-hoc analysis of membrane traces and population rasters.


#### Medical, pharmacology, healthcare

- ⬜ **Drug Interaction Checker** _(stubbed)_: Cross-references a list of medications against a local interaction database (DrugBank / RxNorm / OpenFDA mirrors), surfacing severity tier, mechanism, and clinical advice for each pair and N-way combination.

- ⬜ **Dosage Calculator** _(stubbed)_: Computes weight-, BSA-, and renal-adjusted dosing for adult, pediatric, and neonatal patients, with Cockcroft-Gault / CKD-EPI eGFR, ideal body weight, and infusion rate (mL/hr) conversions.

- ⬜ **Pharmacokinetics Simulator** _(stubbed)_: Models plasma concentration over time for one-, two-, and three-compartment PK with IV bolus, infusion, and oral absorption; supports population variability and Monte Carlo dosing-regimen exploration.

- ⬜ **Anatomy Atlas Viewer** _(stubbed)_: Interactive 3D atlas built on permissive datasets (BodyParts3D, Z-Anatomy), with layer toggles for skeletal, muscular, vascular, nervous, and lymphatic systems and cross-section slicing.

- ⬜ **DICOM Viewer** _(stubbed)_: Loads CT, MRI, PET, and ultrasound DICOM series locally with windowing presets, multi-planar reformatting, MIP/MinIP, measurement tools, and segmentation overlay support.

- ⬜ **ICD / CPT / SNOMED Lookup** _(stubbed)_: Search and reverse-lookup across ICD-10/11, CPT, HCPCS, SNOMED CT, and LOINC code sets, with synonym expansion and AI-assisted mapping from free-text clinical phrases to candidate codes.

- ⬜ **Clinical Trial Search** _(stubbed)_: Queries ClinicalTrials.gov, EU CTR, and WHO ICTRP via local indexes, with filters for phase, condition, intervention, geography, and eligibility; supports patient-criteria matching against a de-identified profile.

- ⬜ **Literature Search** _(stubbed)_: Federated search over PubMed, Europe PMC, and bioRxiv/medRxiv with MeSH expansion, citation graph traversal, and AI summarization of abstracts into structured PICO (population, intervention, comparator, outcome) tuples.

- ⬜ **Patient Case Notebook** _(stubbed)_: De-identified case workspace with HPI, ROS, exam, labs, imaging, problem list, and differential; enforces PHI redaction on import and supports SOAP, H&P, and discharge-summary templates.

- ⬜ **Differential Diagnosis Generator** _(stubbed)_: Takes signs, symptoms, labs, and demographics and produces a ranked differential with prior probabilities, distinguishing features, and suggested next tests; AI-integrated reasoning with citation back to source guidelines.

- ⬜ **Lab Reference Ranges** _(stubbed)_: Looks up reference intervals for chemistry, hematology, coagulation, endocrine, and tumor markers with age/sex/pregnancy stratification, unit conversion (SI vs conventional), and panic-value flagging.

- ⬜ **ECG Caliper** _(stubbed)_: Loads 12-lead ECG images or waveforms and provides digital calipers for intervals (PR, QRS, QT, QTc Bazett/Fridericia/Framingham), axis calculation, and ischemia/arrhythmia pattern annotation.

- ⬜ **Risk Score Calculator** _(stubbed)_: Library of validated clinical risk scores (CHA2DS2-VASc, HAS-BLED, Wells, PERC, MELD, APACHE II, SOFA, CURB-65, FRAX, ASCVD, Framingham) with inputs, score, and stratified recommendations.

- ⬜ **Drug Identifier** _(stubbed)_: Identifies pills by imprint, shape, color, and scoring against the NLM Pillbox / RxImage data; supports photo upload with AI-integrated visual matching as a triage step before manufacturer confirmation.

- ⬜ **Medical Image Segmenter** _(stubbed)_: Runs local segmentation models (TotalSegmentator, MONAI) on CT/MRI volumes for organs, lesions, and vessels, with manual brush/polygon correction and volumetric statistics export.

- ⬜ **Pathway Browser** _(stubbed)_: Visualizes biochemical and signaling pathways (Reactome, KEGG-equivalent open data, WikiPathways) with gene/protein/metabolite overlays and drug-target annotation.

- ⬜ **Genomic Variant Annotator** _(stubbed)_: Annotates VCF variants against ClinVar, gnomAD, dbSNP, and OMIM with ACMG classification, pharmacogenomic (CPIC/PharmGKB) star-allele calling, and predicted protein effect.

- ⬜ **Microbiology Antibiogram** _(stubbed)_: Builds and queries local antibiograms from culture-and-sensitivity data, suggests empirical therapy by site of infection and local resistance patterns, and tracks MIC trends over time.

- ⬜ **Nutrition and Metabolic Calculator** _(stubbed)_: Computes BMR/TDEE (Mifflin-St Jeor, Harris-Benedict), macronutrient targets, fluid maintenance (Holliday-Segar), TPN/PPN composition, and refeeding-syndrome risk.

- ⬜ **Vitals and Growth Charts** _(stubbed)_: Plots longitudinal vitals and pediatric growth (WHO 0-2, CDC 2-20) with percentiles and Z-scores, BP percentiles for age/height/sex, and early-warning-score (NEWS2, PEWS) computation.


#### Psychology and behavioral science

- ⬜ **Experiment timeline builder** _(stubbed)_: Constructs trial sequences, blocks, and conditions for within- and between-subjects designs with a PsychoPy/jsPsych-style component graph. Exports to runnable scripts and a frame-accurate preview.

- ⬜ **Counterbalancing designer** _(stubbed)_: Generates Latin squares, Williams designs, and blocked randomization schedules to balance order effects across participants. Verifies condition coverage and outputs per-participant assignment files.

- ⬜ **Likert scale builder** _(stubbed)_: Composes ordinal item batteries with balanced anchors, reverse-scored items, and forced-choice variants. AI-integrated wording critique flags double-barreled, leading, or low-readability items.

- ⬜ **Power analysis calculator** _(stubbed)_: Computes a priori, post hoc, and sensitivity power for t-tests, ANOVA, regression, chi-square, and mixed-effects designs in a G*Power-style interface. Simulates power curves over varied effect sizes and sample sizes.

- ⬜ **Statistical test guide** _(stubbed)_: Recommends a test from data type, design, and assumption checks via a decision tree, then runs the test and reports the result with assumption diagnostics. Suggests nonparametric or robust alternatives when assumptions fail.

- ⬜ **Behavioral event coder** _(stubbed)_: Frame-by-frame video and audio coding with mutually exclusive and overlapping event tracks, hotkey schemas, and modifiers, in the style of BORIS and ELAN. Produces state and point event logs with durations and transitions.

- ⬜ **Qualitative theme tagger** _(stubbed)_: NVivo-style coding of text, audio transcripts, and image regions with hierarchical codebooks, memos, and query matrices. AI-integrated initial code suggestions seed open coding for the researcher to accept, merge, or reject.

- ⬜ **Reaction time task runner** _(stubbed)_: Presents stimuli and records keypress, mouse, and touch responses with sub-millisecond timestamping and frame-locked onsets. Supports Stroop, flanker, go/no-go, n-back, and IAT templates with custom block scripts.

- ⬜ **Inter-rater reliability calculator** _(stubbed)_: Computes Cohen's and Fleiss' kappa, Krippendorff's alpha, ICC variants, and percent agreement from coder x item matrices. Highlights disagreement cells for adjudication and recoder calibration.

- ⬜ **Psychometric scale validator** _(stubbed)_: Runs reliability (Cronbach's alpha, McDonald's omega), item-total correlations, and exploratory and confirmatory factor analysis with fit indices. Generates a scale report with item-drop simulations and loading plots.

- ⬜ **Mixed-effects model builder** _(stubbed)_: Specifies lme4-style random and fixed effects with crossed and nested factors, fits with REML or ML, and compares models via likelihood ratio and AIC. Outputs marginal and conditional R-squared, EMMs, and contrast tests.

- ⬜ **Bayesian analysis workbench** _(stubbed)_: Fits Bayesian t-tests, ANOVA, regression, and brms-style multilevel models with prior elicitation helpers and posterior diagnostics. Reports Bayes factors, credible intervals, and posterior predictive checks.

- ⬜ **Pre-registration drafter** _(stubbed)_: Fills OSF and AsPredicted templates from the experiment design, hypotheses, and planned analyses already specified in the workbench. Locks the protocol with a hash for later deviation tracking.

- ⬜ **Vignette factorial designer** _(stubbed)_: Builds factorial vignette and conjoint stimuli by combining attribute levels, with fractional-factorial reduction and balanced presentation across respondents. Exports to survey runners with per-respondent randomization.

- ⬜ **Ecological momentary assessment scheduler** _(stubbed)_: Designs ESM/EMA and diary protocols with signal-contingent, interval-contingent, and event-contingent prompts across days. Generates participant schedules, compliance dashboards, and aggregated multilevel-ready datasets.

- ⬜ **Facial action coder** _(stubbed)_: Detects AU activations and intensities from video via an OpenFace-style pipeline, with head pose and gaze direction tracks. Aligns AU streams to event codes and stimulus onsets for time-locked analysis.

- ⬜ **Eye-tracking analyzer** _(stubbed)_: Parses raw gaze samples into fixations and saccades (I-VT, I-DT), maps to AOIs, and computes dwell time, time-to-first-fixation, and transition matrices. Renders heatmaps and scanpaths over stimulus images.

- ⬜ **Behavioral game lab** _(stubbed)_: Runs dictator, ultimatum, public goods, trust, and prisoner's dilemma paradigms with parameterized payoff structures and optional partner-matching protocols. Logs decisions, RTs, and earnings for multilevel analysis.


#### Sociology, political science, public policy

- ⬜ **Census/ACS pull** _(stubbed)_: Queries Census Bureau APIs (Decennial, ACS 1/5-year, PEP) by geography (tract, block group, PUMA, county) and variable code, joins TIGER/Line shapefiles, and caches as a typed table with margin-of-error columns preserved.

- ⬜ **Microdata explorer** _(stubbed)_: Loads IPUMS, CPS, and PUMS extracts with variable harmonization across years, applies replicate weights for variance estimation, and exposes a tabulation grid with crosstabs and significance flags.

- ⬜ **Shapefile/GIS overlay** _(stubbed)_: Reads shapefiles, GeoJSON, GeoPackage, and KML, reprojects between CRS (NAD83, WGS84, state plane), performs spatial joins, buffers, and dissolves, and renders choropleths with classed breaks (Jenks, quantile, equal interval).

- ⬜ **Redistricting plan editor** _(stubbed)_: Builds and edits district plans from precinct or block building blocks, enforces contiguity and population equality tolerances, and shows live compactness (Polsby-Popper, Reock, Schwartzberg), VRA-relevant demographic splits, and partisan composition.

- ⬜ **Ensemble plan analyzer** _(stubbed)_: Runs Markov chain (ReCom) ensembles of legal districting plans for a state, scores each on partisan bias, efficiency gap, mean-median, and seat-vote curves, and locates the user's plan in the resulting distribution as an outlier test.

- ⬜ **Precinct-to-district joiner** _(stubbed)_: Disaggregates precinct-level election returns to census blocks via population weights, then reaggregates to arbitrary district plans, with reconciliation reports for split precincts and missing returns.

- ⬜ **Survey weighting designer** _(stubbed)_: Applies design weights, raking (anesrake-style iterative proportional fitting), post-stratification, and trimming against population targets, and reports effective sample size, design effect, and weight distribution diagnostics.

- ⬜ **MRP estimator** _(stubbed)_: Fits multilevel regression and post-stratification models for small-area opinion estimation, accepting individual survey responses plus a post-stratification frame, and returning area-level estimates with credible intervals.

- ⬜ **Regression dashboard** _(stubbed)_: Fits OLS, GLM (logit, probit, Poisson, negative binomial), fixed/random effects, multilevel, and survival models, with cluster-robust and HC standard errors, and emits publication-ready regression tables (stargazer/modelsummary style) in LaTeX, HTML, and Markdown.

- ⬜ **Causal inference kit** _(stubbed)_: Implements difference-in-differences (including Callaway-Sant'Anna and Sun-Abraham for staggered adoption), regression discontinuity with optimal bandwidth selection, instrumental variables (2SLS, LIML), synthetic control, and matching (propensity, Mahalanobis, CEM) with balance diagnostics.

- ⬜ **Ideal-point estimator** _(stubbed)_: Estimates legislator and voter ideal points from roll call or survey response matrices using NOMINATE, IDEAL (Bayesian IRT), and Wordfish for text, and plots cutting lines and party medians over time.

- ⬜ **Network analysis canvas** _(stubbed)_: Loads edge lists and adjacency matrices for co-sponsorship, donation, citation, or interpersonal networks, computes degree, betweenness, eigenvector, and PageRank centralities, runs Louvain and Leiden community detection, and renders force-directed layouts.

- ⬜ **Policy citation manager** _(stubbed)_: Stores statutes, regulations, court cases, agency reports, and academic articles with Bluebook, APA, and Chicago formatting, deduplicates via DOI/SSRN/CourtListener IDs, and exports BibTeX and CSL-JSON for use in manuscripts.

- ⬜ **Document text analyzer** _(stubbed)_: Ingests legislative text, hearing transcripts, agency comments, and news corpora, fits LDA and BERTopic topic models, runs dictionary methods (LIWC-style, MFD), and computes readability (Flesch-Kincaid) and policy-area classifiers with AI-integrated few-shot labeling against a Comparative Agendas Project codebook.

- ⬜ **Inequality and segregation metrics** _(stubbed)_: Computes Gini, Theil T/L, Atkinson, and 90/10 ratios for income/wealth distributions, plus dissimilarity, isolation, exposure, and Moran's I for residential segregation, with decomposition by group and geography.

- ⬜ **Polling aggregator** _(stubbed)_: Pulls survey results from public archives (Roper, FiveThirtyEight, election polling repos), normalizes likely-voter/registered-voter screens and party-ID weightings, and fits a state-space poll-of-polls with house-effect and mode-effect adjustments.

- ⬜ **Codebook manager** _(stubbed)_: Generates and maintains DDI/Codebook-compliant data dictionaries with variable labels, value labels, skip patterns, and provenance for survey and administrative datasets, with diff views across waves and harmonization mappings.

- ⬜ **Map composer** _(stubbed)_: Lays out static and interactive maps with insets, legends, scale bars, north arrows, and cartogram (Dorling, Gastner-Newman) options, and exports to SVG, PNG, and web tiles (MBTiles, PMTiles) for offline serving.


#### Climate, weather, and atmospheric science

- ⬜ **GRIB2/NetCDF inspector** _(stubbed)_: Opens GRIB1/GRIB2, NetCDF3/4, HDF5, and Zarr stores; lists messages, variables, dimensions, CF attributes, and projection metadata; previews fields as quicklook maps with subsetting by bbox, level, and time.

- ⬜ **NWP model field viewer** _(stubbed)_: Renders GFS, HRRR, NAM, RAP, ECMWF, ICON, and GEM output as filled contours, streamlines, and wind barbs over Cartopy basemaps; supports run cycle picker, forecast hour scrubber, and side-by-side model comparison.

- ⬜ **Skew-T/Log-P sounding plotter** _(stubbed)_: Plots radiosonde, model, and dropsonde profiles with parcel paths, mixed-layer and most-unstable lifts, CAPE/CIN/LCL/LFC/EL, PWAT, and freezing levels using MetPy thermodynamics; loads RAOB, BUFKIT, and GFS/HRRR sounding files.

- ⬜ **Hodograph & severe parameters** _(stubbed)_: Draws 0-1, 0-3, 0-6 km hodographs with storm motion (Bunkers right/left) and computes SRH, EHI, STP, SCP, SHIP, and effective bulk shear for severe weather analysis.

- ⬜ **NEXRAD Level II viewer** _(stubbed)_: Decodes WSR-88D Archive II volumes via Py-ART; renders reflectivity, velocity, spectrum width, ZDR, KDP, CC, and RhoHV with PPI, RHI, and pseudo-CAPPI views; supports dealiasing, dual-pol HCA, and storm-relative motion.

- ⬜ **MRMS mosaic browser** _(stubbed)_: Streams MRMS composite reflectivity, QPE, rotation tracks, and hail size grids; overlays SPC storm reports, NWS warnings, and county/CWA polygons for verification.

- ⬜ **GOES ABI composer** _(stubbed)_: Loads GOES-16/18 and Himawari L1b/L2 NetCDF; generates true color, AirMass, day cloud phase, dust, fire, and GeoColor RGBs using Satpy recipes; supports CONUS, full disk, and mesoscale sector animation.

- ⬜ **Lightning density mapper** _(stubbed)_: Ingests GOES GLM flash/group/event data and NLDN/ENTLN CG/IC stroke feeds; produces flash extent density, total optical energy, and flash rate time series with parallax correction.

- ⬜ **METAR/SYNOP station plotter** _(stubbed)_: Pulls ASOS, AWOS, METAR, and SYNOP observations from MADIS, Iowa Mesonet, or Synoptic API; renders WMO-standard station models with wind barbs, sky cover, present weather, and pressure tendency.

- ⬜ **Hurricane track plotter** _(stubbed)_: Loads HURDAT2, IBTrACS, and ATCF a-deck/b-deck files; plots best track, intensity, wind radii, and forecast cones; overlays ensemble spaghetti from GEFS/ECMWF-EPS and computes ACE/PDI.

- ⬜ **HYSPLIT trajectory runner** _(stubbed)_: Wraps HYSPLIT for forward/back trajectories, dispersion, and concentration runs from GDAS/HRRR ARL files; generates ensemble trajectories and source-receptor matrices with plume animation.

- ⬜ **CMIP6 scenario browser** _(stubbed)_: Indexes local or Pangeo cloud CMIP6 Zarr stores; filters by experiment (historical, ssp245, ssp585), model, variant, and frequency; extracts time series, climatologies, and ensemble means with regridding via xESMF.

- ⬜ **ERA5/reanalysis extractor** _(stubbed)_: Fetches ERA5, MERRA-2, JRA-55, and NCEP/NCAR reanalysis subsets from CDS, GES DISC, or Copernicus Marine; caches monthly tiles locally and exports point, transect, or area-weighted time series.

- ⬜ **Drought & precip index calculator** _(stubbed)_: Computes SPI, SPEI, PDSI, scPDSI, EDDI, and percent of normal from GHCN, PRISM, CHIRPS, or model precip and PET; produces gridded anomaly maps and basin-aggregated drought monitor overlays.

- ⬜ **Fire weather index suite** _(stubbed)_: Calculates Canadian FWI (FFMC, DMC, DC, ISI, BUI, FWI), NFDRS, KBDI, Haines, and Hot-Dry-Windy indices from station or model surface fields; flags red-flag conditions on a map.

- ⬜ **Air quality aggregator** _(stubbed)_: Merges AirNow, OpenAQ, PurpleAir (with EPA correction), and EPA AQS feeds with TEMPO and TROPOMI satellite NO2/HCHO/CO; computes AQI, exceedance hours, and HYSPLIT-coupled smoke source attribution.

- ⬜ **Reprojection & regrid kitchen** _(stubbed)_: Resamples between lat/lon, Lambert Conformal, polar stereographic, rotated pole, and EASE-Grid using wgrib2, CDO, NCO, and xESMF; handles conservative, bilinear, and nearest-neighbor methods with mask-aware weights.

- ⬜ **Bias correction & downscaling** _(stubbed)_: Applies quantile mapping, BCSD, BCCA, and EQM bias correction to model output against a reference (Livneh, PRISM, ERA5-Land); supports statistical downscaling and constructed-analogue methods for climate impact studies.

- ⬜ **Storm chase nowcaster** _(stubbed)_: Combines latest HRRR, RAP mesoanalysis, MRMS, and GOES bands into a single map; AI-integrated highlighting of likely supercell, MCS, or QLCS modes from radar morphology and environmental shear/CAPE fields.

- ⬜ **Wind rose & climatology builder** _(stubbed)_: Builds wind roses, joint speed-direction distributions, diurnal and seasonal climatologies, and return-period estimates (GEV/GPD) from station or reanalysis records; exports for siting and renewable-energy studies.


#### Astronomy and astrophysics

- ⬜ **Sky Chart** _(stubbed)_: Interactive planetarium rendering stars, deep-sky objects, planets, and constellations for any location and time. Supports horizon view, equatorial grid, telrad overlays, and tonight-visible filtering.

- ⬜ **Ephemeris Calculator** _(stubbed)_: Computes positions, rise/set/transit times, illumination, and apparent magnitudes for planets, moons, asteroids, and comets using JPL DE kernels and MPC orbital elements. AI-integrated natural-language queries like 'when does Jupiter clear my eastern treeline'.

- ⬜ **FITS Viewer** _(stubbed)_: Loads FITS images and cubes with WCS-aware pan/zoom, header inspection, multiple stretch functions (linear, log, asinh, zscale), and side-by-side blink comparison. Includes region drawing, pixel statistics, and HDU navigation.

- ⬜ **Image Stacker** _(stubbed)_: Aligns and combines astrophotography subexposures with star detection, registration, sigma-clipped stacking, and dark/flat/bias calibration. Supports drizzle integration, dithered mosaics, and per-frame quality scoring.

- ⬜ **Aperture Photometry** _(stubbed)_: Measures source brightness with circular, elliptical, or PSF-fitted apertures and annular sky subtraction. Outputs differential and absolute magnitudes against catalog comparison stars with airmass and color-term correction.

- ⬜ **Plate Solver** _(stubbed)_: Determines WCS astrometric solution for any image by matching detected stars against Gaia or Tycho catalogs. Returns RA/Dec center, rotation, pixel scale, and field distortion polynomial.

- ⬜ **Telescope Pointing** _(stubbed)_: Drives ASCOM/INDI/Alpaca mounts with goto, sync, plate-solve-and-recenter, and meridian flip handling. Includes pier-side awareness, slew limits, and pointing model fitting from sky calibration runs.

- ⬜ **Imaging Sequencer** _(stubbed)_: Schedules multi-target imaging sessions with filter wheel, focuser, rotator, autoguider, and dome control. Handles autofocus runs, dithering between subs, weather aborts, and meridian flips.

- ⬜ **Polar Alignment** _(stubbed)_: Computes polar alignment error from drift exposures or three-point plate solves, then displays altitude/azimuth adjustment vectors. Tracks convergence as the user nudges the mount.

- ⬜ **Autoguider** _(stubbed)_: Locks on a guide star or multi-star centroid and emits pulse-guide corrections to the mount via ST-4 or ASCOM. Computes RMS, peak, and seeing estimates with calibration assistant.

- ⬜ **Orbital Simulator** _(stubbed)_: Integrates N-body trajectories for solar-system bodies, spacecraft, and arbitrary scenarios using symplectic or Runge-Kutta integrators with JPL ephemerides as initial conditions. Includes patched-conics and lambert-solver helpers for mission design sketches.

- ⬜ **Redshift Distance** _(stubbed)_: Converts between redshift, comoving distance, luminosity distance, angular-diameter distance, lookback time, and age for user-selectable cosmologies (LCDM, wCDM, custom). Includes K-correction helpers and a cosmic-calculator-style table view.

- ⬜ **Light Curve Analyzer** _(stubbed)_: Plots time-series photometry with Lomb-Scargle and Box-Least-Squares periodograms, phase folding, transit-shape fitting, and detrending. AI-integrated classification hint between eclipsing binary, transit, RR Lyrae, and flare candidates.

- ⬜ **Catalog Browser** _(stubbed)_: Queries Gaia, 2MASS, SDSS, Pan-STARRS, SIMBAD, NED, VizieR, and MPC with cone, box, and crossmatch searches. Loads results as overlays on the sky chart and FITS viewer with persistent local caches.

- ⬜ **Observatory Planner** _(stubbed)_: Generates nightly observing plans optimized for altitude, moon separation, twilight, meridian crossing, and target priorities. Produces airmass plots, finder charts, and exportable session scripts.

- ⬜ **Solar Activity Monitor** _(stubbed)_: Pulls current and historical sunspot number, F10.7 flux, Kp/Ap, X-ray flux, and CME alerts from SWPC and SILSO. Overlays aurora oval forecasts on a world map for the user's location.

- ⬜ **Satellite Tracker** _(stubbed)_: Propagates TLE/OMM elements with SGP4 to predict satellite passes, ISS sightings, Starlink trains, and iridium-style flares. Supports radio-link Doppler computation and antenna-rotator output.

- ⬜ **Exoplanet Modeler** _(stubbed)_: Fits transit and radial-velocity datasets to Keplerian + limb-darkened-transit models using MCMC or nested sampling. Reports planet radius, mass, eccentricity, and stellar-density posteriors with corner plots.

- ⬜ **Optics Designer** _(stubbed)_: Computes telescope, eyepiece, and camera-train parameters: focal ratio, exit pupil, true field of view, pixel scale, critical sampling, and diffraction limit. Includes Bahtinov-mask focus simulator and Strehl-from-collimation estimator.

- ⬜ **Light Pollution Map** _(stubbed)_: Renders Bortle/SQM sky-brightness maps from VIIRS data with limiting-magnitude estimates per filter and zenith angle. Suggests dark-sky sites within a chosen drive radius.

- ⬜ **Radio Astronomy Toolkit** _(stubbed)_: Decodes hydrogen-line (1420 MHz) drift scans, pulsar dedispersion, and meteor-scatter detections from RTL-SDR/AirSpy inputs. Includes waterfall display, baseline subtraction, and galactic-plane survey mode.

- ⬜ **Meteor Shower Forecaster** _(stubbed)_: Predicts ZHR, radiant position, and parent-body geometry for active and historical meteor showers. Cross-checks against user-supplied all-sky camera detections for automatic shower attribution.

- ⬜ **Stellar Atmosphere Synth** _(stubbed)_: Generates synthetic stellar spectra from Teff, log g, and metallicity via PHOENIX/ATLAS grid interpolation, with broadband-color and bolometric-correction outputs. Useful for SED fitting and ETC color predictions.

- ⬜ **Exposure Time Calculator** _(stubbed)_: Estimates SNR, limiting magnitude, and saturation for a given telescope, camera, filter, target, sky brightness, and seeing. Inverts to recommend per-sub exposure and total integration time for a target SNR.


#### Zoology, veterinary, wildlife

- ⬜ **Camera trap classifier** _(stubbed)_: Runs MegaDetector-style animal/person/vehicle detection on camera trap image folders, then routes detections through species classifiers (e.g. SpeciesNet, Wildlife Insights taxonomies) with per-site whitelists. Emits CamtrapDP-conformant CSVs with bounding boxes, confidence, and burst grouping.

- ⬜ **Acoustic species ID** _(stubbed)_: Analyzes audio files with BirdNET-Analyzer / Perch / BatDetect2 backends to produce time-stamped species predictions, confidence, and a reviewable spectrogram of each hit. Supports region/season filters and custom classifiers trained on the user's own annotations.

- ⬜ **Spectrogram annotator** _(stubbed)_: Interactive spectrogram and waveform viewer for bird, bat, cetacean, and anuran recordings with selection boxes, call-type labels, and Raven/Audacity label export. AI-integrated suggestion of call boundaries and species from the active selection.

- ⬜ **Allometric dose calculator** _(stubbed)_: Computes interspecific drug doses by metabolic body-weight scaling (BW^0.75) with species-specific overrides for sensitivities (e.g. alpha-2 in domestic cats, opioid response in ungulates, ivermectin in collies/MDR1, succinylcholine in raptors). Cross-checks against a local Plumb's-style formulary and flags contraindications.

- ⬜ **Anesthesia protocol planner** _(stubbed)_: Builds field-immobilization and surgical-anesthesia protocols (e.g. BAM, TKM, medetomidine-ketamine, isoflurane top-up) with calculated volumes, dart sizes, induction/recovery timing, and reversal agents. Generates a printable monitoring sheet with target HR/RR/SpO2/EtCO2 ranges for the species.

- ⬜ **Necropsy report builder** _(stubbed)_: Structured gross-pathology entry following standard organ-system templates (Davis, AFIP) with diagram annotation, sample inventory (histo, tox, micro), and chain-of-custody labels. Exports to PDF and to OIE/WOAH wildlife mortality reporting formats.

- ⬜ **Body condition scorer** _(stubbed)_: Species-specific BCS scoring (1-5 or 1-9 scales) for canids, felids, equids, ruminants, psittacines, raptors, chelonians, and cetaceans with photo references for each score. Optional photogrammetric estimation of girth/length ratios from a calibrated image.

- ⬜ **Wildlife telemetry analyzer** _(stubbed)_: Ingests GPS/Argos/VHF collar fixes (Movebank, Vectronic, Lotek formats), filters for DOP and speed outliers, and computes home ranges (MCP, KDE, AKDE, LoCoH), step-selection functions, and migration phenology. Renders tracks on offline tiles with utilization-distribution overlays.

- ⬜ **Occupancy and capture-recapture** _(stubbed)_: Fits single- and multi-season occupancy, N-mixture, and spatially explicit capture-recapture models (PRESENCE/unmarked/secr/oSCR style) to detection histories from camera traps and acoustic arrays. Reports detection probability, density estimates, and goodness-of-fit diagnostics.

- ⬜ **Species distribution modeler** _(stubbed)_: Trains MaxEnt / Random Forest / boosted-tree SDMs on occurrence records and WorldClim/Chelsa/soil rasters with k-fold spatial cross-validation and variable-importance plots. Projects to future climate scenarios and writes range rasters as GeoTIFF.

- ⬜ **Ethogram and behavior coder** _(stubbed)_: BORIS-style live and video behavior coding with user-defined ethograms, modifiers, point and state events, multi-subject scoring, and inter-observer reliability (Cohen's kappa, ICC). Exports event sequences for sequential and bout analysis.

- ⬜ **Track and sign identifier** _(stubbed)_: Image-based identification of mammal tracks, scat, browse, and other field sign with morphometric measurement overlays (stride, straddle, pad dimensions) and regional species shortlists. AI-integrated suggestion ranked against a reference photo library.

- ⬜ **Parasitology bench** _(stubbed)_: Fecal egg count workflow (McMaster, Mini-FLOTAC, Wisconsin) with image-assisted ova/oocyst identification, FECR calculation for anthelmintic resistance, and a blood-smear differential counter. Tracks herd/colony parasite loads over time.

- ⬜ **Vaccine and prophylaxis scheduler** _(stubbed)_: Generates vaccine, anthelmintic, and zoonosis-screening schedules per WSAVA/AAHA/AAFP/AAEP guidelines for domestic species and per AZA/EAZA/AAZV husbandry manuals for exotics. Tracks lot numbers, sites, adverse events, and titer results.

- ⬜ **Studbook and pedigree manager** _(stubbed)_: PopLink/SPARKS/PMx-style studbook keeper with kinship and mean-kinship calculation, inbreeding coefficients, gene-drop simulation, and breeding-pair recommendations. Imports/exports ZIMS Species360 and standard studbook formats.

- ⬜ **Incubation and reproduction tracker** _(stubbed)_: Species-specific gestation, incubation, and hatching calculators (mammals, birds, reptiles with TSD curves) with weight-loss and temperature/humidity logging. Predicts hatch/parturition windows and flags deviations from reference curves.

- ⬜ **Diet and nutrition formulator** _(stubbed)_: Formulates rations against NRC/AZA nutrient targets for the species and life stage, balancing prey items, produce, and pellets for ME, protein, Ca:P, vitamin A/E, and taurine. Computes feeding cost and writes daily-prep sheets for keepers.

- ⬜ **Wildlife disease surveillance** _(stubbed)_: Logs syndromic mortality and morbidity events, maps clusters, and runs SaTScan-style space-time scans for outbreaks (HPAI, CWD, white-nose, ranavirus, chytrid). Generates submission packets for state/federal wildlife health labs.

- ⬜ **Morphometric and aging tool** _(stubbed)_: Records standard morphometrics (skull, wing chord, tarsus, carapace, dentition wear, cementum annuli, otolith rings) with species reference ranges for sex and age class determination. Supports DXA/CT-derived measurements and exports to Darwin Core.


#### Paleontology and archaeology

- ⬜ **Ancient tools and weapons typology** _(stubbed)_: classifies hand-axes, projectile points, swords, bows, slings, etc. by typology/period/material/culture. Replica construction notebook (knapping, smelting, lashing), conservation/restoration log, experimental archaeology trial logger, regional comparison atlas.

- ⬜ **Specimen catalog** _(stubbed)_: Tracks fossils, artifacts, and samples with accession numbers, provenience, taxonomy, collector, lithology, and curation status. Generates Darwin Core exports and printable specimen labels with QR codes for tray/drawer linking.

- ⬜ **Stratigraphic column builder** _(stubbed)_: Draws measured sections with lithology fills, grain-size profile, bed thickness, fossil/sample horizons, and contact types in the style of SedLog and StratDraw. Exports SVG and correlates multiple columns side-by-side along a chosen datum.

- ⬜ **Harris matrix editor** _(stubbed)_: Builds and validates stratigraphic context relationships (above, below, equals, cuts, fills) for excavation units, detects cycles, and renders the matrix as a layered DAG. Imports context sheet CSVs from single-context recording systems.

- ⬜ **Radiocarbon calibrator** _(stubbed)_: Calibrates uncalibrated 14C dates against IntCal20, SHCal20, and Marine20 curves with reservoir corrections, producing probability density distributions and 1/2-sigma ranges. Supports R_Combine, R_Date, sequence, and phase Bayesian models in the OxCal idiom.

- ⬜ **Geochronology calculator** _(stubbed)_: Computes ages for K-Ar, 40Ar/39Ar, U-Pb concordia, U-Th, OSL/TL, ESR, and amino acid racemization from raw measurements with full uncertainty propagation. Plots isochrons, concordia diagrams, and probability-density spectra.

- ⬜ **Dig site GIS** _(stubbed)_: Maps excavation grids, trenches, features, and finds in geographic and local site coordinates with total-station/GNSS import (DXF, CSV, Leica GSI). Overlays satellite, DEM hillshade, magnetometry, and GPR slices, and produces single-context plans.

- ⬜ **Photogrammetry pipeline** _(stubbed)_: Builds 3D models of fossils, artifacts, trenches, and rock-art panels from photo sets using SfM/MVS with scale-bar and coded-target detection. Outputs textured meshes, orthophotos, and DEMs compatible with MeshLab and CloudCompare.

- ⬜ **Mesh reconstruction lab** _(stubbed)_: Repairs, retopologizes, mirrors, and digitally rearticulates 3D fossil scans; segments matrix from bone, fills missing elements from bilateral counterparts, and aligns fragments via ICP. AI-integrated landmark suggestion seeds the alignment from comparable taxa.

- ⬜ **Geometric morphometrics** _(stubbed)_: Performs Procrustes superimposition, PCA, CVA, and thin-plate-spline warps on 2D/3D landmark and semi-landmark configurations. Imports TPS and Morphologika files and outputs deformation grids and disparity plots.

- ⬜ **Phylogenetics workbench** _(stubbed)_: Edits character-taxon matrices in NEXUS/TNT format, runs parsimony, maximum likelihood, and tip-dated Bayesian inference, and visualizes consensus trees with bootstrap/posterior support. Maps stratigraphic ranges and computes ghost lineages.

- ⬜ **Taxonomy browser** _(stubbed)_: Browses paleobiological and zoological taxonomy with synonymy lists, type specimens, stratigraphic ranges, and paleogeographic occurrences sourced from PBDB, GBIF, ITIS, and the Treatise on Invertebrate Paleontology. Resolves authorities and flags nomina dubia.

- ⬜ **Faunal analysis** _(stubbed)_: Records NISP, MNI, MNE, MAU, %survivorship, age-at-death, and butchery/burning marks for zooarchaeological assemblages. Computes diversity indices, taphonomic profiles, and skeletal-part representation plots.

- ⬜ **Lithic typology** _(stubbed)_: Classifies stone tools and debitage by techno-typological scheme (Bordes, Tixier, chaine operatoire stages), records metrics (length, width, thickness, platform attributes), and tracks raw-material sourcing. Generates refit diagrams and reduction-sequence flow charts.

- ⬜ **Ceramic seriation** _(stubbed)_: Builds seriation matrices from sherd type counts using frequency (Ford) and occurrence (Robinson) methods, with correspondence analysis and battleship-curve plots. Links sherd drawings, fabric descriptions, and rim-diameter charts.

- ⬜ **Sediment and soil logger** _(stubbed)_: Records Munsell color, texture, structure, inclusions, pH, and organic content for excavation contexts and core samples with controlled vocabularies. Plots grain-size distributions from sieve/laser data and computes sorting, skewness, and kurtosis.

- ⬜ **Stable isotope analyzer** _(stubbed)_: Ingests delta 13C, delta 15N, delta 18O, and 87Sr/86Sr measurements from bone collagen, enamel, and shell to reconstruct diet, mobility, and paleoclimate. Plots mixing models (FRUITS/MixSIAR-style) and applies trophic enrichment factors.

- ⬜ **Body size and mass estimator** _(stubbed)_: Estimates body mass, stance, and locomotor parameters from limb element dimensions (femoral circumference, humeral length) using regression equations for mammals, dinosaurs, and other clades. Reports prediction intervals and the source equation used.

- ⬜ **Paleogeographic mapper** _(stubbed)_: Projects occurrences and sample sites onto paleocontinental reconstructions (PaleoMAP, GPlates rotation files) for chosen ages. Plots paleolatitude tracks, paleoclimate belts, and ocean-current overlays for biogeographic analysis.

- ⬜ **Geophysics survey processor** _(stubbed)_: Processes magnetometry, resistivity, and ground-penetrating-radar grids with gridding, despiking, destriping, gain control, and time-slice extraction. Detects linear and rectilinear anomalies and overlays them on site basemaps.

- ⬜ **Bibliographic search** _(stubbed)_: Searches GeoRef, Biodiversity Heritage Library, JSTOR, BHL, ADS, and CrossRef for paleontological and archaeological literature with citation graph traversal and DOI resolution. Maintains a local BibTeX/CSL library with PDF storage and OCR full-text indexing.


#### Oceanography and marine science

- ⬜ **Tide predictor** _(stubbed)_: Computes tidal elevation at any station using NOAA/IHO harmonic constituents. Plots high/low tables, lunar phase overlay, and supports custom constituent fits from a user-supplied time series.

- ⬜ **Tidal current rose** _(stubbed)_: Predicts ebb/flood currents at NOAA current stations and renders rose diagrams of speed and direction over arbitrary intervals. Handles reversing, rotary, and hydraulic current types.

- ⬜ **Bathymetry viewer** _(stubbed)_: Renders GEBCO, ETOPO, and BAG/GeoTIFF bathymetry with hillshade, contour lines, and along-track profile extraction. Supports vertical exaggeration sliders and slope/aspect derivatives.

- ⬜ **CTD profile plotter** _(stubbed)_: Plots temperature, salinity, density, oxygen, and fluorometry against depth from CNV, ODV, and netCDF casts. Applies SBE-style despiking, loop-edit, and bin-averaging with per-channel QC flags.

- ⬜ **T-S diagram explorer** _(stubbed)_: Overlays multiple CTD casts on a temperature-salinity diagram with TEOS-10 isopycnals and labelled water mass envelopes (NACW, AAIW, NADW, etc.) for mixing-line analysis.

- ⬜ **TEOS-10 calculator** _(stubbed)_: Computes absolute salinity, conservative temperature, in-situ and potential density, sound speed, freezing point, and specific volume anomaly using the GSW (Gibbs SeaWater) toolbox formulas.

- ⬜ **Sonar log viewer** _(stubbed)_: Parses Lowrance SL2/SL3, Humminbird DAT, and XTF side-scan files. Displays waterfall sidescan and down-imaging with bottom-track overlay, GPS track, and per-ping water column extraction.

- ⬜ **Ocean color analyzer** _(stubbed)_: Fetches and visualizes chlor-a, Kd490, PAR, and SST from MODIS-Aqua, VIIRS, Sentinel-3 OLCI, and PACE L2/L3 products. Performs time-series extraction over user-drawn ROIs with cloud masking.

- ⬜ **Plankton micrograph classifier** _(stubbed)_: AI-assisted taxonomic ID for diatoms, dinoflagellates, foraminifera, and copepods from microscope images, with confidence scores and links to WoRMS records. Logs cell counts per FOV into a field notebook.

- ⬜ **Marine species ID** _(stubbed)_: Photo-based identification for reef fish, invertebrates, macroalgae, and corals with region-aware priors (Caribbean, Indo-Pacific, NE Pacific, etc.). Surfaces diagnostic features and GBIF occurrence overlays.

- ⬜ **Dive log + decompression planner** _(stubbed)_: Records depth/time profiles from Suunto, Shearwater, and Garmin downloads. Runs Buhlmann ZH-L16C with configurable gradient factors and VPM-B; plans multi-gas, trimix, and CCR dives with bailout schedules.

- ⬜ **Hydroacoustic spectrogram** _(stubbed)_: Loads WAV/FLAC hydrophone recordings and renders calibrated spectrograms with band-limited SPL, 1/3-octave noise, and AI-assisted detection of cetacean calls, snapping shrimp, and vessel signatures.

- ⬜ **AIS vessel decoder** _(stubbed)_: Decodes NMEA-0183 AIVDM/AIVDO and NMEA-2000 PGNs from a serial, UDP, or file source. Plots vessel positions, CPA/TCPA against own-ship, and replays historical traffic at adjustable speed.

- ⬜ **Wave spectrum analyzer** _(stubbed)_: Computes 1D and directional wave spectra from NDBC buoy spectral files or raw heave/pitch/roll. Fits JONSWAP and Pierson-Moskowitz parameters and derives Hs, Tp, mean direction, and spreading.

- ⬜ **ADCP profile processor** _(stubbed)_: Reads RDI PD0, Nortek Signature, and Sontek files; performs beam-to-earth coordinate transform, bottom-track removal, and shear/Reynolds stress estimation with bin-mapping and side-lobe masking.

- ⬜ **Argo float fetcher** _(stubbed)_: Queries the Argo GDAC for profiles by region, date, WMO ID, or float type (core, BGC, deep). Builds trajectory maps and overlays T/S casts against climatology (WOA, Roemmich-Gilson).

- ⬜ **Carbonate chemistry calculator** _(stubbed)_: Runs CO2SYS-style computations across DIC, alkalinity, pH, pCO2, and aragonite/calcite saturation with selectable K1/K2 constants and total/seawater pH scales for ocean acidification work.

- ⬜ **Sound speed profile builder** _(stubbed)_: Generates c(z) profiles via Mackenzie, Chen-Millero, and Del Grosso equations from CTD casts or climatology, exports to Bellhop/Kraken envfiles, and previews ray traces for sonar range prediction.

- ⬜ **Lagrangian drift simulator** _(stubbed)_: Tracks particle trajectories through HYCOM, CMEMS, or user-supplied velocity fields with Stokes drift and windage. Supports larval competency windows and beaching/MPA-intersection statistics.

- ⬜ **Nautical chart viewer** _(stubbed)_: Renders S-57/S-101 ENCs and BSB raster charts with depth contours, navaids, and AIS overlay. Supports route planning with cross-track error, waypoint export to GPX, and tidal-height-aware UKC checks.

- ⬜ **Coral DHW tracker** _(stubbed)_: Pulls NOAA Coral Reef Watch SST anomaly and Degree Heating Weeks for any reef polygon, tracks bleaching alert level history, and overlays in-situ HOBO logger data for ground-truth comparison.

- ⬜ **Mixed layer depth detector** _(stubbed)_: Identifies MLD on CTD or glider casts using threshold (de Boyer Montegut), gradient, and curvature methods. Flags barrier layers and compensation depth from paired PAR/oxygen.

- ⬜ **Reef transect logger** _(stubbed)_: Structured entry for belt-transect and point-intercept surveys (Reef Life Survey, AGRRA, CoralWatch protocols) with size-class abundance, substrate cover, and offline-first sync to a portable SQLite store.

- ⬜ **NetCDF/HDF5 explorer** _(stubbed)_: Browses CF-compliant ocean datasets with dimension-aware slicing, attribute inspection, and quicklook plots for ROMS, MOM6, HYCOM, WW3, and Argo files. Exports subsets to Zarr or COG for downstream use.


#### Ecology, conservation, sustainability

- ⬜ **Occurrence record harvester** _(stubbed)_: Pulls species occurrence records from GBIF, iNaturalist, eBird, VertNet, and BOLD by taxon, bounding box, or date range. Normalizes Darwin Core fields, flags georeferencing issues, and deduplicates across sources.

- ⬜ **Diversity index calculator** _(stubbed)_: Computes Shannon, Simpson, Pielou evenness, Chao1, ACE, Hill numbers, and Faith's PD from community matrices. Supports alpha, beta (Jaccard, Bray-Curtis, Sorensen), and gamma diversity with rarefaction and extrapolation curves.

- ⬜ **Landscape fragmentation analyzer** _(stubbed)_: Computes FRAGSTATS-style patch, class, and landscape metrics: patch density, edge density, mean nearest neighbor, contagion, Shannon's landscape diversity, and effective mesh size from categorical land cover rasters.

- ⬜ **Connectivity modeler** _(stubbed)_: Builds resistance surfaces and runs Circuitscape current-flow and Conefor graph-theoretic connectivity (dPC, IIC) between habitat patches. Identifies pinch points, corridors, and critical stepping-stone nodes.

- ⬜ **LCA inventory builder** _(stubbed)_: Assembles life-cycle inventories with process flows from ecoinvent, USLCI, and Agribalyse libraries. Runs ReCiPe, TRACI, CML, and ILCD impact assessment methods with contribution and sensitivity analysis.

- ⬜ **Carbon footprint calculator** _(stubbed)_: Computes GHG inventories across Scope 1/2/3 using EPA, DEFRA, IPCC AR6, and EXIOBASE emission factors. Supports product, organizational, and project boundaries with biogenic carbon and land-use change accounting.

- ⬜ **Invasive species alert** _(stubbed)_: Cross-references occurrence streams against GISD, CABI, and EDDMapS invasive lists for the target region. Projects invasion fronts from current detections and AI-integrated risk scoring on climate suitability and propagule pressure.

- ⬜ **Citizen-science submission helper** _(stubbed)_: Formats field observations into iNaturalist, eBird, GBIF, and CitSci.org submission payloads with EXIF georeferencing and Darwin Core tagging. AI-integrated species suggestion from photo and call clip with confidence and reviewer notes.

- ⬜ **Bioacoustic classifier** _(stubbed)_: Processes audio recordings with BirdNET, Perch, BatDetect2, and Koogu for bird, bat, and anuran identification. Generates spectrograms, detection timestamps, vocal activity rates, and species accumulation curves.

- ⬜ **Remote sensing indexer** _(stubbed)_: Pulls Sentinel-2, Landsat, MODIS, and PlanetScope tiles via STAC and computes NDVI, EVI, NDWI, NBR, and tasseled cap indices. Builds time series for phenology, deforestation alerts, and burn severity mapping.

- ⬜ **Protected area overlay** _(stubbed)_: Queries WDPA, OECMs, KBA, and Natura 2000 polygons against project footprints. Computes coverage, gap analysis against Aichi/30x30 targets, and management category breakdowns.

- ⬜ **Red List status checker** _(stubbed)_: Looks up IUCN Red List category, criteria, population trend, and threat codes via the Red List API for a species list. Cross-checks national lists (USFWS ESA, EU Habitats Directive, COSEWIC, CITES appendices) and flags taxonomic synonyms.

- ⬜ **Conservation prioritization solver** _(stubbed)_: Runs Marxan and Zonation spatial prioritization with species, cost, and connectivity layers under area or representation targets. Outputs selection frequency maps, irreplaceability scores, and reserve network solutions.

- ⬜ **Species traits database** _(stubbed)_: Queries TRY, EltonTraits, AmphiBIO, FishBase, and BIEN for morphological, life-history, and functional traits. Computes community-weighted means, functional diversity (FRic, FEve, FDis), and trait imputation for missing values.

- ⬜ **eDNA metabarcoding analyzer** _(stubbed)_: Processes amplicon sequencing reads through DADA2/QIIME2 pipelines with BOLD, MIDORI, and SILVA reference databases. Outputs OTU/ASV tables, taxonomy assignments, and detection probabilities for target taxa.

- ⬜ **Phenology and climate tracker** _(stubbed)_: Aggregates NPN, PEP725, and herbarium phenophase records against gridded temperature and growing-degree-day series. Detects shifts in first-bloom, leaf-out, and migration arrival, and projects under CMIP6 scenarios.


#### Geology, GIS, and earth sciences

- ⬜ **Map Viewer** _(stubbed)_: Pannable, zoomable slippy-map canvas that overlays raster tiles (OSM, satellite, custom XYZ) with vector layers (GeoJSON, KML, GPX, Shapefile). Supports per-layer styling, opacity, attribute filtering, and on-map measurement of distance, bearing, and area.

- ⬜ **DEM Analyzer** _(stubbed)_: Loads digital elevation models (GeoTIFF, ASCII grid, HGT) and derives hillshade, slope, aspect, curvature, and TPI rasters. Includes an interactive elevation profile tool that samples along arbitrary polylines.

- ⬜ **Projection Converter** _(stubbed)_: Reprojects coordinates and full datasets between CRSes via PROJ strings, EPSG codes, or WKT. Handles single points, coordinate lists, bounding boxes, and whole vector/raster files with datum-shift awareness.

- ⬜ **Geocoder** _(stubbed)_: Forward and reverse geocoding against a local Nominatim-style index plus optional online providers, returning ranked candidates with confidence and place hierarchy. Batch mode resolves CSV columns to lat/lon in one pass.

- ⬜ **Mineral Identifier** _(stubbed)_: Identifies likely minerals from observed properties (hardness, streak, luster, cleavage, density, optical signs, crystal system) against a vendored mineral database. AI-integrated reasoning over fuzzy or partial observations to rank candidates and suggest the next diagnostic test.

- ⬜ **Geologic Time Scale** _(stubbed)_: Interactive ICS chronostratigraphic chart with zoomable eons, eras, periods, epochs, and ages, plus GSSP markers and major bio/geo events. Click any unit to get duration, key fossils, climate state, and references.

- ⬜ **Seismic Trace Viewer** _(stubbed)_: Reads SEG-Y, miniSEED, and SAC files and renders waveforms, wiggle traces, or variable-density displays with bandpass/notch filtering, picking, and FFT spectra. Supports event windowing, STA/LTA detection, and arrival-time picking with first-motion polarity.

- ⬜ **Watershed Analyzer** _(stubbed)_: Performs pit filling, D8/D-infinity flow direction, flow accumulation, stream network extraction, and basin delineation from a DEM. Computes runoff curves (SCS-CN) and time-of-concentration for a user-placed pour point.

- ⬜ **Stereonet** _(stubbed)_: Equal-area and equal-angle lower-hemisphere projections for planes, lines, and fold axes, with contour plots of pole density (Kamb, Schmidt). Computes intersections, rotations, and best-fit fold-axis solutions from field measurements.

- ⬜ **Cross-Section Builder** _(stubbed)_: Draws a section line on a map and extrudes a geological cross-section from a DEM plus mapped contacts, dips, and unit polygons. Supports vertical exaggeration, fault offsets, and apparent-dip correction along oblique transects.

- ⬜ **Geochemistry Plotter** _(stubbed)_: Standard rock-geochemistry diagrams: TAS, AFM, QAPF, Harker, Pearce, spider/REE-normalized multielement plots, and ternary classifiers. Accepts whole-rock CSV input with normalization to chondrite, primitive mantle, or NMORB.

- ⬜ **CIPW Norm Calculator** _(stubbed)_: Computes the CIPW normative mineralogy of an igneous rock from a major-element whole-rock analysis, with iron oxidation handled by user ratio or empirical adjustment. Reports normative percentages and a derived rock classification.

- ⬜ **Earthquake Feed** _(stubbed)_: Queries USGS, EMSC, and IRIS catalogs for events filtered by magnitude, depth, time window, and polygon. Plots epicenters on the map with focal-mechanism beachballs and links to per-event waveform pulls.

- ⬜ **Viewshed and Line-of-Sight** _(stubbed)_: Computes single-observer and cumulative viewsheds across a DEM, accounting for observer height, target height, earth curvature, and atmospheric refraction. Includes a point-to-point LOS profile with obstruction highlighting.

- ⬜ **LiDAR Point Cloud Viewer** _(stubbed)_: Streams LAS/LAZ tiles into a 3D viewer with classification-based coloring, intensity ramps, and ground/non-ground filtering. Supports cropping by polygon, decimation, profile extraction, and export to gridded DEM or DSM.

- ⬜ **Borehole Log Viewer** _(stubbed)_: Renders LAS well-log curves (GR, resistivity, density, neutron, sonic) alongside lithology columns and core photos at matching depth. Supports zonation, crossplots (e.g. density-neutron), and porosity/Vshale calculations.

- ⬜ **Plate Reconstruction** _(stubbed)_: Rotates present-day plates to their paleogeographic positions using GPlates-compatible rotation files and topology models. Animates plate motion through time and reprojects user features onto the reconstructed basemap.

- ⬜ **Gravity and Magnetic Grids** _(stubbed)_: Loads and visualizes potential-field grids (Bouguer gravity, free-air, total-field magnetics) with upward continuation, derivative, analytic-signal, and reduction-to-pole filters. Extracts profiles and overlays interpreted lineaments.

- ⬜ **Tile Downloader** _(stubbed)_: Caches map tile pyramids (XYZ, WMTS, WMS) for an area of interest and zoom range into a local MBTiles or directory store for offline use. Throttles requests, resumes interrupted jobs, and reports tile coverage stats.

- ⬜ **Field Notebook** _(stubbed)_: Geotagged structured note capture for outcrops, samples, and measurements (strike/dip, lithology, contacts, photos) with offline sync and map pin rendering. AI-integrated extraction of structured fields from free-text descriptions and photo OCR of sample labels.


#### Agriculture and horticulture

- ⬜ **Plant identification** _(stubbed)_: image-based species identification from leaf/flower/bark/fruit photos (PlantNet/iNaturalist-style flow), regional flora filter, look-alike disambiguation, edibility and toxicity tagging, care guidance per species.

- ⬜ **Planting Calendar** _(stubbed)_: Generates per-crop sow, transplant, and harvest dates from local frost dates, day length, and hardiness zone. Supports succession planting, indoor starts, and ICS export to a calendar app.

- ⬜ **Soil Test Interpreter** _(stubbed)_: Parses lab reports (Mehlich-3, Bray, Olsen, Morgan, saturated paste) into normalized nutrient, CEC, base saturation, and pH metrics, then flags deficiencies and toxicities against crop-specific targets with AI-integrated narrative summaries of what the numbers actually mean.

- ⬜ **Fertilizer Blend Calculator** _(stubbed)_: Solves NPK and micronutrient targets across arbitrary input materials (urea, MAP, KCl, gypsum, fish emulsion, compost analyses) using linear programming to minimize cost or salt index. Outputs per-bed application rates in lb/ac, g/m2, or tsp/gal.

- ⬜ **Irrigation Scheduler** _(stubbed)_: Computes daily and weekly water needs from reference ET (Penman-Monteith or Hargreaves), crop coefficient (Kc) curves, root depth, soil water holding capacity, and rainfall. Outputs run times per zone for drip, micro-spray, or overhead emitters.

- ⬜ **Drip Layout Designer** _(stubbed)_: Lays out drip lines, emitters, and submains over a plot drawing with hydraulic calculations for pressure loss, emitter flow uniformity (EU), and lateral length limits. Generates a parts list keyed to standard fittings.

- ⬜ **Garden Plot Designer** _(stubbed)_: 2D bed and row editor with scale, sun exposure overlay, path widths, and plant spacing footprints from a built-in cultivar database. Snaps to companion planting and rotation constraints.

- ⬜ **Crop Rotation Planner** _(stubbed)_: Plans multi-year rotations across beds using family grouping, nitrogen demand, allelopathy, disease carryover windows, and cover crop slots. AI-integrated suggestion of a rotation that satisfies the user's beds, crops, and constraints.

- ⬜ **Companion Matrix** _(stubbed)_: Interactive grid of beneficial, neutral, and antagonistic plant pairings sourced from published trial data and traditional polyculture references, with provenance flags so folk wisdom and replicated studies are distinguishable.

- ⬜ **Growing Degree Day Tracker** _(stubbed)_: Accumulates GDD against base and ceiling temperatures per crop or pest from local station or gridded forecast data. Predicts bloom, ripening, and insect emergence windows (e.g. codling moth, corn earworm).

- ⬜ **Frost Risk Predictor** _(stubbed)_: Combines forecast lows, dew point, wind, and microclimate offsets to score nightly frost probability for each bed or row. Recommends row cover, sprinkler frost protection, or harvest-now triggers.

- ⬜ **Pest and Disease Identifier** _(stubbed)_: AI-integrated photo classifier for common crop pests, diseases, and nutrient deficiencies, with confidence scores, scouting checklist, lifecycle notes, and ranked IPM responses (cultural, biological, chemical) keyed to regional pesticide labels.

- ⬜ **Spray Mix Calculator** _(stubbed)_: Converts label rates (per acre, per 100 gal, ppm, or oz/gal) to tank-fill volumes for backpack, hand, or boom sprayers. Tracks REI, PHI, tank mixing compatibility, and surfactant or buffer additions.

- ⬜ **Compost Recipe Builder** _(stubbed)_: Balances feedstocks to target C:N (typically 25-30), moisture (~55%), and bulk density across browns, greens, manures, and biochar inputs. Estimates pile temperature curve and turn schedule.

- ⬜ **Hydroponic Solution Mixer** _(stubbed)_: Builds A/B stock tanks for hydroponics, fertigation, or foliar feeds from mineral salts (calcium nitrate, MKP, MgSO4, micros) to hit target ppm and EC with pH adjusted via acid or base. Validates ion balance and precipitation risk.

- ⬜ **Crop Yield Model** _(stubbed)_: Estimates yield per crop using degree-day accumulation, light interception (LAI, PAR), water stress index, and nutrient sufficiency. Calibrates against entered historical harvest weights to localize predictions.

- ⬜ **Sun and Shade Analyzer** _(stubbed)_: Computes hour-by-hour solar exposure for any garden location given latitude, date range, and user-drawn obstructions (trees, fences, structures). Outputs daily light integral (DLI) heatmaps for siting beds, greenhouses, or PV-shaded agrivoltaics.

- ⬜ **Greenhouse Climate Simulator** _(stubbed)_: Models heat load, ventilation, transpiration, and supplemental light for a hoop house or glasshouse given glazing, orientation, and local TMY weather. Sizes heaters, fans, shade cloth, and HAF circulation.

- ⬜ **Rainwater Catchment Calculator** _(stubbed)_: Sizes cisterns and gutters from roof area, local precipitation records, demand schedule, and first-flush losses. Plots reliability curves and overflow frequency for candidate tank volumes.

- ⬜ **Cover Crop Selector** _(stubbed)_: Recommends cover crop species or mixes for a target window based on biomass goal, N fixation, scavenging, weed suppression, soil compaction relief, and termination method. Computes seeding rates in lb/ac or g/m2.

- ⬜ **Harvest and Yield Log** _(stubbed)_: Tracks per-bed harvest weights, dates, and quality notes; rolls them up into pounds per square foot, dollars per bed, and labor hours per crop. Exports CSV and feeds back into the yield model for next season.


#### Image and video

- ✅ **Image recognition and description** _(in testing)_: runs a multimodal vision model against any image (drag into the AI pane or call with a path) and returns a textual description grounded in the actual pixels.

- ✅ **Image editor** _(in testing)_: MS-Paint-style canvas with brush, eraser, line, rectangle, and select tools, color picker, brush size, zoom (Ctrl+wheel and toolbar), 30-step undo/redo, and copy/cut/paste that talks to the OS clipboard. View/edit toggle on the image tab.

- ⬜ **Advanced raster image editor (Photoshop / GIMP class)** _(stubbed)_: layers, masks, blend modes, smart selections (lasso, magic wand, color range), healing/clone tools, content-aware fill, adjustment layers, color grading, photo retouching, compositing, plug-in/script architecture. Goes beyond the MS-Paint-style image editor for real photo work.

- ⬜ **Vector graphics editor (Illustrator / Inkscape class)** _(stubbed)_: paths and beziers, boolean operations, gradients, typography (text on path, OpenType features), live filters, layered SVG export, vector effects. For logos, schematic illustrations, icons, layout artwork.

- ⬜ **Raster editor** _(stubbed)_: Layer-based bitmap editor with brushes, masks, blend modes, non-destructive adjustment layers, and selection tools. AI-integrated inpainting, outpainting, and generative fill operate on the active selection.

- ⬜ **Vector editor** _(stubbed)_: Bezier-curve illustration surface with paths, boolean operations, gradient meshes, and SVG import/export. Supports AI-integrated raster-to-vector tracing and prompt-driven shape generation.

- ⬜ **Image generator** _(stubbed)_: Local text-to-image and image-to-image diffusion runner with controlnet, LoRA, IP-Adapter, and regional prompting. Routes prompts through the interpretation pipeline before sampling.

- ⬜ **Inpainter** _(stubbed)_: Mask-driven local regeneration over an existing image with edge-aware blending and seed control. Reuses the active generator and prompt context.

- ⬜ **Upscaler** _(stubbed)_: Tiled super-resolution using ESRGAN/Real-ESRGAN/SwinIR families with face-restore (GFPGAN/CodeFormer) and configurable denoise strength. Handles batch jobs and arbitrary integer or fractional factors.

- ⬜ **Background remover** _(stubbed)_: Foreground extraction via segmentation models (RMBG, BiRefNet, U2Net) with alpha refinement around hair and translucent edges. Supports batch processing and replacement backgrounds.

- ⬜ **Segmenter** _(stubbed)_: Click, box, and text-prompt segmentation built on SAM/SAM2 and Grounding-DINO. Exports masks, cutouts, and polygons for use elsewhere in the workbench.

- ⬜ **Pose and depth extractor** _(stubbed)_: Runs OpenPose, DWPose, MiDaS, ZoeDepth, and normal map estimators to produce ControlNet conditioning maps. Visualizes overlays and saves the maps as reusable assets.

- ⬜ **Tagger and captioner** _(stubbed)_: Image-to-text via WD14, BLIP, BLIP2, and a local VLM, producing booru tags, natural captions, and dense region descriptions. Writes results to sidecar files and the asset metadata store.

- ⬜ **Visual search** _(stubbed)_: CLIP/SigLIP embedding index over the local image library with text and image queries, duplicate detection, and near-duplicate clustering. Returns ranked thumbnails with similarity scores.

- ⬜ **OCR** _(stubbed)_: Text recognition over images and PDFs using PaddleOCR/Tesseract/TrOCR with layout analysis and language autodetection. Outputs searchable text, hOCR, and editable overlay layers.

- ⬜ **Color tools** _(stubbed)_: Palette extraction, harmony generation, LUT authoring, gamut checking, and color-blindness simulation. Includes AI-integrated palette suggestion conditioned on a reference image or mood prompt.

- ⬜ **Format converter** _(stubbed)_: Batch transcoder across PNG, JPEG, WebP, AVIF, HEIC, TIFF, JPEG-XL, DDS, and PSD with color-profile awareness and chroma subsampling control. Reports size and quality deltas per file.

- ⬜ **RAW developer** _(stubbed)_: Demosaicing and tone-mapping pipeline over CR2/NEF/ARW/DNG using libraw with white-balance, highlight recovery, and lens-profile correction. Exposes a node graph for non-destructive edits.

- ⬜ **HDR and tonemap** _(stubbed)_: Multi-exposure merging, tonemapping operators (Reinhard, Drago, Mantiuk), and EXR/HDR I/O. Supports deghosting of handheld brackets.

- ⬜ **Panorama stitcher** _(stubbed)_: Feature-matched alignment, blending, and projection via Hugin/OpenCV with manual control-point editing. Handles spherical, cylindrical, and planar outputs.

- ⬜ **Photogrammetry** _(stubbed)_: Structure-from-motion and multi-view stereo over image sets using COLMAP/OpenMVS to produce point clouds, meshes, and textured models. Optional Gaussian-splat export.

- ⬜ **Video editor** _(stubbed)_: Non-linear timeline with multi-track audio/video, trimming, transitions, keyframed effects, and proxy editing. Renders via the transcoder with project-level color management.

- ⬜ **Transcoder** _(stubbed)_: FFmpeg-driven batch encoder/decoder with presets for H.264, H.265, AV1, VP9, ProRes, DNxHR, and HAP. Exposes hardware-accelerated paths (NVENC/QSV/VAAPI) and two-pass settings.

- ⬜ **Frame interpolator** _(stubbed)_: Optical-flow and learned interpolation (RIFE, FILM, IFRNet) for slow motion and frame-rate upconversion. Supports scene-cut detection to avoid blending across cuts.

- ⬜ **Video upscaler** _(stubbed)_: Temporally-coherent super-resolution using Real-ESRGAN-Video, BasicVSR++, and Topaz-style pipelines. Streams frames through tiled inference with optional codec passthrough.

- ⬜ **Video matting** _(stubbed)_: Per-frame alpha extraction via RVM/MODNet/BiRefNet with temporal smoothing and trimap refinement. Outputs alpha video, mattes, and per-shot masks.

- ⬜ **Text-to-video** _(stubbed)_: Local video generation using AnimateDiff, CogVideoX, Mochi, LTX-Video, and Wan-class models with ControlNet, motion LoRAs, and reference-image conditioning. Reuses prompts and seeds from the image generator.

- ⬜ **Lipsync and talking head** _(stubbed)_: Audio-driven facial animation using SadTalker, Wav2Lip, and LivePortrait over a still portrait or driving video. Exposes head-pose and expression strength controls.

- ⬜ **Speech and caption** _(stubbed)_: Whisper-based transcription, forced alignment, and burned-in or sidecar subtitle generation (SRT/VTT/ASS) with speaker diarization. Translates captions via the local LLM.

- ⬜ **Object tracker** _(stubbed)_: Multi-object tracking (ByteTrack, OC-SORT) and mask propagation (SAM2, XMem, Cutie) across video for rotoscoping and effect attachment. Exports per-frame masks and bounding boxes.

- ⬜ **Stabilizer** _(stubbed)_: Camera-path smoothing and rolling-shutter correction using gyroflow-style and learned methods. Visualizes motion before commit and supports crop-aware output framing.

- ⬜ **Motion graphics** _(stubbed)_: Keyframed 2D animation and lower-third compositor with Lottie/Bodymovin import and SVG-on-timeline support. Exports to MP4, transparent WebM, GIF, and APNG.

- ⬜ **Compositor** _(stubbed)_: Node-based image and video compositing with EXR multi-channel support, deep compositing, keyers, and 2.5D camera. Inspired by Natron/Nuke conventions.

- ⬜ **Color grading** _(stubbed)_: Scene-referred grading with primary wheels, curves, qualifiers, and CDL/ACES workflows. Includes AI-integrated shot-match against a reference frame.

- ⬜ **GIF and sprite toolkit** _(stubbed)_: Lossy GIF optimization, palette quantization, sprite-sheet packing, and APNG/WebP-animated output. Includes onion-skin frame review.

- ⬜ **Screenshot and capture** _(stubbed)_: Region, window, and scrolling-page capture with annotation, redaction, and screen recording to MP4/WebM. Pipes captures directly into the editor or generator.

- ⬜ **Image diff** _(stubbed)_: Pixel, perceptual (SSIM/LPIPS/DISTS), and structural diff with side-by-side and onion-skin views. Useful for regression checks on generated assets.

- ⬜ **Trainer** _(stubbed)_: Local LoRA, DreamBooth, textual-inversion, and ControlNet training over diffusion backbones with mixed-precision and gradient-checkpointing. Live loss/sample previews during runs.

- ⬜ **Model hub** _(stubbed)_: Local registry of checkpoints, LoRAs, VAEs, embeddings, and ControlNets with hashes, sources, license tags, and per-model usage notes. Surfaces share-alike and restricted licenses per release-licensing intent.

- ⬜ **Prompt library** _(stubbed)_: Versioned prompt and negative-prompt snippets with tags, sample grids, and seed history. Integrates with the interpretation pipeline so prompts can be replayed or remixed.

- ⬜ **XYZ plot and grid** _(stubbed)_: Parameter-sweep renderer that varies prompts, seeds, CFG, samplers, and LoRA weights to produce comparison grids. Annotates each cell with its parameters.

- ⬜ **Style transfer** _(stubbed)_: Reference-style restyling via IP-Adapter, InstantStyle, StyleAlign, and neural style transfer with content/style weight controls. Supports both image and video inputs.

- ⬜ **Face tools** _(stubbed)_: Detection, landmarking, recognition, swap (InsightFace/InSwapper/Roop-style), and identity-preserving re-render. Includes consent/provenance flags in metadata per release-licensing intent.

- ⬜ **NSFW and safety classifier** _(stubbed)_: Optional local content classification with adjustable thresholds and per-tag reporting. Off by default since abliterated models are in scope.

- ⬜ **Watermark and provenance** _(stubbed)_: Visible watermarking plus invisible signatures (StegaStamp, Trustmark) and C2PA manifest read/write. Verifies and strips watermarks where licensing allows.

- ⬜ **Steganography** _(stubbed)_: LSB and DCT hide/extract for text and small payloads inside images and audio-of-video tracks. Reports capacity and visual detectability estimates.

- ⬜ **3D viewer and converter** _(stubbed)_: Loads OBJ, GLTF, FBX, STL, USDZ, and PLY (including Gaussian splats) with turntable preview and format conversion. Bakes textures and renders thumbnails for the asset library.

- ⬜ **Texture and material tools** _(stubbed)_: PBR texture authoring with seamless tiling, normal/height/roughness map extraction, and AI-integrated material synthesis from a single photo. Outputs glTF-ready material sets.


#### Photography and cinematography

- ⬜ **Exposure Calculator** _(stubbed)_: Computes depth of field, hyperfocal distance, circle of confusion, equivalent exposure across ISO/aperture/shutter, ND filter stacking, long-exposure times, and film reciprocity failure for a chosen stock. Cross-links results to the focal length and sensor format selected in the lens browser.

- ⬜ **Lens Database Browser** _(stubbed)_: Searchable catalog of lens specifications with focal length, max aperture, mount, weight, optical formula, MTF curves, vignetting, distortion, and field-of-view crop simulator. Imports manufacturer data sheets and user-measured calibrations.

- ⬜ **Sun and Moon Planner** _(stubbed)_: Ephemeris that plots sun and moon azimuth/altitude, sunrise/sunset, civil/nautical/astronomical twilight, golden and blue hour windows, and Milky Way core position for a given latitude, longitude, and date. Overlays the path on a map with terrain shadowing.

- ⬜ **Astrophotography Planner** _(stubbed)_: Computes optimal exposure for stars from focal length and declination via the NPF rule, predicts star trail arcs, charts deep-sky object visibility and transit times, schedules ISS passes, and outputs polar alignment offsets for an equatorial mount.

- ⬜ **Shot List Builder** _(stubbed)_: Structured planner for scenes, shots, takes, and setups with fields for lens, filter, lighting, action, dialogue, and coverage. Exports to CSV, PDF, and EDL-compatible formats and links each row to storyboard frames.

- ⬜ **Storyboard Composer** _(stubbed)_: Frame-by-frame storyboard editor with aspect-ratio guides, figure and prop placement, camera position and lens indicator, and arrow annotations for movement. AI-integrated rough sketch generation from a shot description and reference image.

- ⬜ **Lighting Diagram Designer** _(stubbed)_: Top-down 2D lighting plot with subjects, key/fill/back/rim sources, modifiers, flags, power levels, color temperatures, and gel selections. Renders a quick BRDF preview of the resulting key-to-fill ratio on a standard head model.

- ⬜ **Camera Move Planner** _(stubbed)_: Designs dolly, crane, slider, and gimbal paths with keyframed position, rotation, focus, and zoom; simulates motion blur and parallax against a depth-mapped scene plate, and exports MoCo-compatible motion data.

- ⬜ **Focus Stacker** _(stubbed)_: Aligns a bracketed focus series with subpixel registration, fuses by maximum local contrast or depth-map weighting, retouches halos and motion artifacts, and outputs a single deep-focus frame with optional depth pass.

- ⬜ **HDR Merger** _(stubbed)_: Aligns and combines bracketed exposures into a 32-bit float radiance map, deghosts moving elements, and tone-maps via global (Reinhard, Drago) or local (Mantiuk, Durand) operators with controllable contrast preservation.

- ⬜ **Color Grading Suite** _(stubbed)_: Node-based grading with lift/gamma/gain wheels, log/linear curves, qualifier-driven secondaries, scopes (waveform, vectorscope, parade, histogram), and ACES/Rec.709/Rec.2020 working spaces. Renders to ProRes and DNxHR.

- ⬜ **LUT Designer** _(stubbed)_: Authors 1D and 3D LUTs from before/after frame pairs or from explicit gain/contrast/sat operations, converts between .cube, .3dl, and .lut formats at arbitrary cube sizes, and validates round-trip accuracy with a ColorChecker reference.

- ⬜ **Film Stock Emulator** _(stubbed)_: Models grain structure, spectral sensitivity, characteristic curves, halation, and reciprocity behavior for catalogued color and B&W stocks (Portra, Velvia, Tri-X, Cinestill, and others), applied as a post-process pass on linear input.

- ⬜ **Time-Lapse Builder** _(stubbed)_: Calculates intervals for a target output duration, ramps exposure across day-to-night transitions, deflickers the sequence, applies motion-blur synthesis between frames, and renders to ProRes or H.265 with optional dolly-zoom interpolation.

- ⬜ **Composition Overlay** _(stubbed)_: Live and still-image overlay grid with rule of thirds, golden ratio and spiral, diagonal method, dynamic symmetry, action and title safe areas, and arbitrary aspect ratio masking for cropping decisions.

- ⬜ **Photo Catalog** _(stubbed)_: Local-first DAM with content-addressed storage, tag and rating hierarchies, smart collections, face and scene clustering, similarity search via image embeddings, and non-destructive edit history per asset.

- ⬜ **Print Soft Proofer** _(stubbed)_: Previews an image under a paper and printer ICC profile with selectable rendering intent, marks out-of-gamut pixels, simulates D50 viewing illuminant, and predicts ink coverage for fine-art and commercial print workflows.

- ⬜ **MTF Chart Analyzer** _(stubbed)_: Measures lens resolution from photographs of slanted-edge or Siemens-star targets, computes MTF50 across the frame, plots field curvature and astigmatism, and compares results against the lens database reference values.


#### Animation, VFX, and 3D rendering

- ⬜ **Keyframe Animator** _(stubbed)_: Dope-sheet and curve-editor based keyframe authoring with TCB, Bezier, and stepped tangents, onion skinning, and per-channel easing. AI-integrated suggestion of in-between keys and natural easing curves from a sparse pose set.

- ⬜ **Rigging Studio** _(stubbed)_: Skeleton, IK/FK, spline, and ribbon rig builder with weight painting, corrective shape keys, and constraint graphs. Generates control rigs from a mesh plus joint hints, with auto-skinning via heat or biharmonic methods.

- ⬜ **Mocap Cleanup** _(stubbed)_: Imports BVH, FBX, and C3D capture data, fixes marker swaps, gap fills, and foot-slide; retargets between skeletons of differing proportions. AI-assisted denoise and gap interpolation trained on locomotion priors.

- ⬜ **Particle and Fluid Sim** _(stubbed)_: GPU-backed solver for SPH/FLIP fluids, smoke, fire, sand, and instanced particles with collision against signed-distance fields. Caches to OpenVDB and exports velocity fields for motion blur.

- ⬜ **Node Compositor** _(stubbed)_: Nuke-style node graph compositor supporting EXR multilayer, deep compositing, premultiplication-correct merges, color management via OCIO, and animated reads. Renders out tiles in parallel with disk-backed caches.

- ⬜ **Rotoscope Studio** _(stubbed)_: Frame-by-frame matte authoring with planar tracker assisted Bezier splines, motion-blur aware edges, and per-feather animation. AI mask propagation between sparse keyframes using a local segmentation model.

- ⬜ **Render Farm Dispatcher** _(stubbed)_: Local-first job scheduler that distributes Cycles, LuxCore, Arnold-CLI, Mantra, or custom render jobs across machines on the LAN, with per-frame retries, tile assembly, and live thumbnail preview.

- ⬜ **Asset Library** _(stubbed)_: Tagged, versioned browser for models, materials, HDRIs, brushes, rigs, and FX setups with USD-based references, thumbnail bake, and dependency tracking. AI-tagged from preview renders to enable natural-language search.

- ⬜ **PBR Material Lab** _(stubbed)_: Layer-stack material editor producing metallic-roughness or specular-glossiness sets, with substance-like procedural noises, generators, and bake-to-texture from a high-poly source. Exports glTF, MaterialX, and OSL.

- ⬜ **UV Toolkit** _(stubbed)_: Automatic and manual UV unwrap with angle-based flattening, packed tile (UDIM) layout, seam painter, and distortion heatmap. Suggests seam placement from mesh curvature and symmetry analysis.

- ⬜ **Sculpt and Retopo** _(stubbed)_: Multiresolution dynamic-topology sculpting with masks, layers, and remeshing; coupled with quad-dominant retopology for animation-ready meshes. AI-integrated auto-retopo respecting edge loops at deformation regions.

- ⬜ **Motion Tracker** _(stubbed)_: 2D point tracker, planar tracker, and full 3D camera solve from monocular footage with lens-distortion calibration and survey-point constraints. Exports solved cameras and tracked geometry to USD or Alembic.

- ⬜ **Camera and Lens Sim** _(stubbed)_: Physically based camera with focal length, aperture, shutter, focus, ND, and lens distortion presets matched to real-world bodies. Includes anamorphic bokeh, chromatic aberration, and rolling-shutter playback.

- ⬜ **Path Tracer Preview** _(stubbed)_: Embedded interactive path tracer with denoiser, light groups, and AOV passes for IPR while editing the scene. Renders to OpenColorIO display transforms with VR/stereo support.

- ⬜ **Shader Graph** _(stubbed)_: OSL and MaterialX node editor with live-compiled GPU preview, BRDF inspector, and energy-conservation checks. Imports and round-trips to Blender, USD MaterialX, and glTF KHR extensions.

- ⬜ **Cloth and Hair Sim** _(stubbed)_: XPBD-based cloth, hair card and strand grooming, with collisions, wind fields, and pinning maps; cached to Alembic and previewable in the viewport with self-collision toggles.

- ⬜ **Crowd Director** _(stubbed)_: Agent-based crowd system with state-machine behaviors, navmesh pathing, animation-clip stitching, and per-agent variation. AI-integrated locomotion blending so feet stay planted across speeds.

- ⬜ **USD Stage Editor** _(stubbed)_: Hierarchical USD composition browser with layer stacks, variants, references, payloads, and reference resolution. Provides a diff view between layers and an integrated USD-python script console.

- ⬜ **Audio and Lip Sync** _(stubbed)_: Waveform-aligned dope-sheet for dialogue, with phoneme detection driving viseme blendshapes and procedural facial micro-expressions. AI phoneme alignment runs locally from an audio file and a transcript.

- ⬜ **Edit and Color Suite** _(stubbed)_: Non-linear editor with EDL/XML/AAF round-trip, OCIO-managed timelines, primary and secondary color correction, and scopes. Conforms VFX shot ranges from an offline cut and tracks plate handles per shot.


#### Music composition and theory

- ⬜ **Chord and Scale Explorer** _(stubbed)_: Interactive dictionary of chords, scales, and modes with on-keyboard and on-fretboard visualization, interval spelling, mode rotations, and inversion browsing. Plays each selection through a built-in sampler or WebMIDI device.

- ⬜ **Harmonic Analyzer** _(stubbed)_: Parses a score or chord list and produces Roman numeral analysis, functional labels (T/PD/D), secondary dominants, modal interchange tags, and pivot-chord modulation points. Highlights ambiguous regions and offers AI-integrated alternate readings with rationale.

- ⬜ **Score Notation Editor** _(stubbed)_: WYSIWYG engraver for multi-staff scores with proper beaming, slurs, dynamics, articulations, and lyrics. Imports and exports MusicXML, MEI, MIDI, ABC, and LilyPond, and renders engraver-quality PDF and SVG.

- ⬜ **Lead Sheet Generator** _(stubbed)_: Builds a single-staff melody plus chord-symbol chart from a chord progression, melody fragment, or AI-integrated prompt. Supports jazz, pop, and Nashville-number conventions and exports to iReal Pro, MusicXML, and PDF.

- ⬜ **Voice Leading Checker** _(stubbed)_: Validates four-part and chorale-style writing against configurable rules: parallel fifths and octaves, hidden parallels, doubled leading tone, voice crossing, spacing, and resolution of dissonances. Suggests minimum-move corrections.

- ⬜ **Counterpoint Tutor** _(stubbed)_: Species counterpoint editor (Fux/Gradus) covering first through fifth species in two to four voices, with live error highlighting, cantus firmus library, and step-by-step Schenkerian-style reduction view.

- ⬜ **Ear Training Drills** _(stubbed)_: Adaptive trainers for intervals, chord qualities, inversions, scales, modes, cadences, melodic and rhythmic dictation, and functional bass. Tracks per-skill mastery and spaces review using SRS.

- ⬜ **Reharmonization Assistant** _(stubbed)_: Generates substitution options for a given progression including tritone subs, secondary dominants, modal interchange, line-cliche fills, and negative-harmony mirrors, with AI-integrated ranking against a target style or reference track.

- ⬜ **Voicing Optimizer** _(stubbed)_: Computes idiomatic chord voicings (drop-2, drop-3, shell, rootless, quartal, spread) for piano, guitar, or arbitrary ensembles, optimizing voice leading across a progression and respecting instrument range and playability.

- ⬜ **Key and Tempo Detector** _(stubbed)_: Analyzes audio or MIDI to estimate key, mode, tonic confidence, tempo, downbeat positions, time signature, and tempo map. Outputs a click track and beat grid aligned to the source.

- ⬜ **Pitch and Chord Transcriber** _(stubbed)_: Polyphonic audio-to-MIDI with chord-symbol estimation, instrument-aware source separation, and editable piano-roll output. Useful for transcribing recordings into editable lead sheets or stems.

- ⬜ **Rhythm and Groove Builder** _(stubbed)_: Step sequencer plus pattern designer for drums and percussion, with swing, micro-timing, polyrhythm and polymeter layering, Euclidean rhythm generation, and a library of genre-tagged grooves.

- ⬜ **Melody Generator and Harmonizer** _(stubbed)_: Generates melodic lines constrained by scale, contour, range, motivic seed, and target harmony, or harmonizes an existing melody with multiple voicing styles. AI-integrated continuation when given a short motif.

- ⬜ **Form and Structure Analyzer** _(stubbed)_: Segments a score or audio file into sections (intro, verse, chorus, bridge, coda), detects repetition and variation, and produces a labeled timeline with phrase and period grouping.

- ⬜ **Set Theory Workbench** _(stubbed)_: Pitch-class set calculator producing prime form, normal form, interval vector, Forte number, complement, Z-relations, and transformations (Tn, TnI). Visualizes set webs for 20th-century and post-tonal analysis.

- ⬜ **Tuning and Temperament Lab** _(stubbed)_: Compares equal temperament, just intonation, meantone, well-tempered, and custom Scala scales. Plays intervals and chords in each tuning and visualizes cents deviation and harmonic-entropy curves.

- ⬜ **Modulation Planner** _(stubbed)_: Maps routes between any two keys via pivot chords, common-tone modulation, chromatic mediants, enharmonic reinterpretation, and direct shift, with audible previews and difficulty ranking by smoothness.

- ⬜ **Instrument Range and Orchestration Checker** _(stubbed)_: Validates parts against per-instrument range, transposition, idiomatic figuration, and standard doublings. Flags awkward passages and suggests reassignments across a configurable ensemble.

- ⬜ **Style Corpus Search** _(stubbed)_: Searches a local corpus of MIDI, MusicXML, and audio (Bach chorales, jazz standards, folk tunes, pop charts) by melodic contour, harmonic pattern, rhythmic motif, or text query, and reports matches with citations.

- ⬜ **Prosody and Lyric Setter** _(stubbed)_: Aligns lyrics to a melody by syllable count, stress pattern, and phrase shape, flagging mismatches between linguistic stress and metric accent. Offers AI-integrated rewrites that preserve meaning while fixing prosody.


#### Signal processing and audio

- ⬜ **Multitrack audio editor** _(stubbed)_: Non-destructive timeline editor with clip-based editing, crossfades, ripple/slip/roll edits, comping lanes, time-stretching, and pitch-shifting. AI-integrated source separation, transient detection, silence trimming, and natural-language edit commands ("tighten the vocal between 0:42 and 1:10").

- ⬜ **Audio recorder** _(stubbed)_: Multi-input recorder with loopback, monitor mixing, punch-in/out, pre-roll buffer, takes management, and per-channel metering. Supports ASIO/JACK/PipeWire/CoreAudio backends and arbitrary sample rates and bit depths.

- ⬜ **Spectrogram analyzer** _(stubbed)_: STFT, CQT, mel, and reassigned spectrograms with adjustable window, hop, and color mapping; supports linear/log frequency axes and per-bin readout. AI-integrated event labeling and anomaly highlighting on spectrogram regions.

- ⬜ **FFT and spectral inspector** _(stubbed)_: Interactive single-shot and rolling FFT view with windowing options (Hann, Hamming, Blackman-Harris, Kaiser, flat-top), peak picking, harmonic markers, and THD/SNR/SINAD/SFDR readouts.

- ⬜ **Bode and pole-zero plotter** _(stubbed)_: Plots magnitude, phase, group delay, step, and impulse responses for arbitrary transfer functions; supports symbolic entry, s-domain to z-domain conversion, and interactive pole-zero dragging.

- ⬜ **Convolution and impulse-response toolkit** _(stubbed)_: Captures, deconvolves (sine-sweep, MLS), trims, and applies impulse responses for room/cab/speaker simulation; partitioned convolution for low-latency runtime use.

- ⬜ **Synthesizer rack** _(stubbed)_: Modular soft-synth host running subtractive, FM, wavetable, additive, granular, and physical-modeling engines with polyphony, unison, and modulation matrices. AI-integrated patch generation from text descriptions ("warm analog pad, slow attack").

- ⬜ **Effects chain** _(stubbed)_: Insert/send routing for EQ, dynamics, reverb, delay, modulation, distortion, pitch, and spectral effects; supports LV2/VST3/CLAP plugins and per-effect A/B and undo history.

- ⬜ **Parametric and graphic EQ** _(stubbed)_: Linear-phase and minimum-phase EQ with dynamic bands, mid/side processing, spectrum overlay, and match-EQ from reference tracks. AI-integrated EQ-curve suggestion from a target tonal reference.

- ⬜ **Dynamics processor suite** _(stubbed)_: Compressor, limiter, expander, gate, multiband compressor, and transient shaper with sidechain, lookahead, and gain-reduction visualization.

- ⬜ **Loudness and metering suite** _(stubbed)_: True-peak, RMS, LUFS (integrated/short-term/momentary), LRA, K-system, VU, PPM, correlation, vectorscope, and goniometer meters compliant with EBU R128 and ITU-R BS.1770.

- ⬜ **Source separation studio** _(stubbed)_: AI-integrated stem separation (vocals, drums, bass, guitar, piano, other) using Demucs/MDX/Spleeter-class models; per-stem export, residual extraction, and karaoke creation.

- ⬜ **Noise reduction and restoration** _(stubbed)_: Spectral subtraction, Wiener filtering, RNNoise-style learned denoising, declicking, decrackle, dehum, and dereverb; manual noise-profile capture and AI-integrated automatic profiling.

- ⬜ **Pitch and time tools** _(stubbed)_: Phase-vocoder, PSOLA, and elastique-style time-stretch and pitch-shift; melodic pitch correction with note-grid editing similar to Melodyne/Auto-Tune workflows.

- ⬜ **Sample library manager** _(stubbed)_: Tagged, searchable browser for one-shots, loops, and multisamples with waveform/spectrogram previews, BPM/key detection, similarity search, and drag-to-track loading. AI-integrated content-based tagging and natural-language sample search.

- ⬜ **MIDI editor** _(stubbed)_: Piano-roll, drum-grid, step-sequencer, and event-list editors with velocity/CC lanes, quantize, humanize, swing, arpeggiator, and chord tools. AI-integrated melody/harmony/drum-pattern generation and continuation.

- ⬜ **MIDI router and monitor** _(stubbed)_: Routes and filters MIDI between hardware/virtual ports, transforms channels/CCs, monitors raw bytes and SysEx, and records MIDI streams with timestamps.

- ⬜ **Music notation editor** _(stubbed)_: Score editor with staff layout, transposition, chord symbols, lyrics, articulations, MusicXML/LilyPond import/export, and bidirectional MIDI<->score conversion. AI-integrated automatic transcription from audio to notation.

- ⬜ **Audio-to-MIDI transcription** _(stubbed)_: AI-integrated monophonic and polyphonic transcription of recordings to MIDI with onset/offset, pitch, and velocity estimation; drum hit detection and groove extraction.

- ⬜ **Beat, tempo, and key detection** _(stubbed)_: Detects BPM, downbeats, beat grid, time signature, and musical key/scale; supports variable-tempo mapping and warp-marker editing.

- ⬜ **Mixing console** _(stubbed)_: Channel-strip mixer with faders, pans, mutes, solos, busses, sends, VCA groups, automation lanes, and snapshot/scene recall.

- ⬜ **Mastering workbench** _(stubbed)_: Reference-track A/B, loudness normalization, dithering, oversampling, codec preview, and ISP-aware limiting; batch export to streaming-platform targets. AI-integrated mastering profile matched to a reference.

- ⬜ **Audio format converter** _(stubbed)_: Batch converter between WAV/FLAC/AIFF/MP3/Opus/Vorbis/AAC/ALAC with sample-rate, bit-depth, channel-layout, and metadata transforms; SoX/FFmpeg-backed.

- ⬜ **Bode/network analyzer driver** _(stubbed)_: Drives sound-card and lab-instrument loopback measurements to characterize analog/digital signal chains: frequency response, THD+N, IMD, crosstalk, and phase. AI-integrated interpretation of measurement plots.

- ⬜ **Signal generator** _(stubbed)_: Generates sine, square, triangle, sawtooth, white/pink/brown noise, sweeps (linear/log), MLS, chirps, multitones, and arbitrary user-defined waveforms with precise amplitude, phase, and DC offset control.

- ⬜ **Oscilloscope and logic-style viewer** _(stubbed)_: Time-domain viewer with triggering, persistence, XY mode, math channels, and cursor measurements; reads from audio interfaces or imported captures.

- ⬜ **Room acoustics analyzer** _(stubbed)_: Measures RT60, EDT, C50/C80, STI, modal response, and waterfall decay from sine-sweep or MLS captures; suggests treatment placement. AI-integrated treatment recommendations from measured response.

- ⬜ **Loudspeaker and headphone EQ tuner** _(stubbed)_: Captures system response with a measurement mic, computes correction FIR/IIR filters, and exports to convolution plugins or hardware DSPs.

- ⬜ **Plugin host and scanner** _(stubbed)_: Hosts LV2/VST3/CLAP/AU plugins in-process or sandboxed, scans and catalogs installed plugins, exposes parameters for automation, and bridges 32-bit/legacy formats.

- ⬜ **Vocoder and talkbox** _(stubbed)_: Channel and phase vocoders with carrier/modulator routing, formant shifting, and cross-synthesis between arbitrary signals.

- ⬜ **Granular and concatenative synthesis lab** _(stubbed)_: Grain-cloud synthesizer with corpus-based concatenative mode driven by target-audio descriptors (centroid, MFCC, loudness).

- ⬜ **Speech toolkit** _(stubbed)_: AI-integrated speech-to-text (Whisper-class) with diarization, forced alignment, phoneme labeling, and TTS voices; supports timestamped subtitle export and voice cloning workflows.

- ⬜ **Audio fingerprinting and similarity** _(stubbed)_: Computes Chromaprint/audio-hashing fingerprints and embedding-based similarity to deduplicate, identify, and cluster recordings across a library.

- ⬜ **DSP code playground** _(stubbed)_: Live-coded DSP environment with NumPy/SciPy/Faust/SOUL-style cells, real-time audio I/O, and instant plot/scope feedback for prototyping algorithms. AI-integrated code generation from natural-language DSP specs.

- ⬜ **Wavetable and sample editor** _(stubbed)_: Single-cycle and multi-frame wavetable editor with harmonic drawing, FFT resynthesis, morphing, and export to common synth wavetable formats; loop-point and zero-crossing tools for sampled instruments.

- ⬜ **Drum machine and step sequencer** _(stubbed)_: Pattern-based sequencer with per-step probability, ratchets, micro-timing, parameter locks, and song-mode chaining; samples or synthesized voices per track.

- ⬜ **Tuner and intonation tools** _(stubbed)_: Chromatic and strobe tuner, temperament selector (equal, just, meantone, Pythagorean, custom Scala/.scl), and intonation analyzer for ensembles and recordings.

- ⬜ **Podcast and dialogue production** _(stubbed)_: Multi-speaker leveling, dialogue ducking, mouth-click removal, silence-cut, and chapter-marker tools; AI-integrated transcript-driven editing where deleting text deletes audio.

- ⬜ **Project versioning and stem archive** _(stubbed)_: Snapshots sessions, freezes tracks to stems, deduplicates audio assets, and stores reproducible render manifests for collaboration and recall.


#### Textiles, sewing, soft crafts

- ⬜ **Pattern Drafter** _(stubbed)_: Parametric 2D garment pattern drafting with measurement tables, slopers, darts, seam allowances, notches, and grain lines. Exports tiled PDF, plotter HPGL, DXF-AAMA, and SVG. AI-integrated drafting from a natural-language brief plus body measurements that proposes a starting block and ease.

- ⬜ **Sloper Block Library** _(stubbed)_: Parametric bodice, sleeve, skirt, trouser, and torso blocks driven by a measurement profile. Stores personal fit blocks with version history and exposes them as the base for any drafted pattern.

- ⬜ **Garment Grader** _(stubbed)_: Applies grade rules across a size range with proportional, vector, or industry-standard grading. Visualizes nested sizes, edits grade points per landmark, and validates dart truing across the run.

- ⬜ **Marker Nester** _(stubbed)_: Auto-nests pattern pieces onto fabric width with nap, stripe match, plaid match, and grain constraints to minimize yardage. Reports efficiency percent and lay length and exports cut files for manual or plotter cutting.

- ⬜ **Fabric Yardage Calculator** _(stubbed)_: Estimates required yardage from selected pattern, fabric width, repeat, nap, and shrinkage. Computes cost from price per yard, accounts for fussy-cut waste, and stores fabric-specific defaults.

- ⬜ **Drape Simulator** _(stubbed)_: 3D cloth simulation of a flat pattern on a parametric avatar with adjustable stretch, bending, and friction per fabric. Highlights strain and pressure to flag tight zones before cutting.

- ⬜ **Embroidery Digitizer** _(stubbed)_: Converts raster or vector art into machine-stitch files (DST, PES, EXP, JEF, VP3) with auto-underlay, density, push-pull compensation, stitch direction, and color stops. AI-integrated auto-digitize that proposes fills, satins, and run paths from an input image.

- ⬜ **Cross-Stitch Charter** _(stubbed)_: Converts photos and artwork into counted cross-stitch charts with floss palette mapping to DMC, Anchor, Madeira, and Sulky. Controls aida count, chart size, color count, dithering, and backstitch outlines, and exports printable symbol charts.

- ⬜ **Knit Chart Editor** _(stubbed)_: Designs colorwork, lace, and cable charts with standard stitch symbols, repeat boxes, and no-stitch cells. Computes row and stitch counts from a gauge swatch and exports written instructions alongside the chart.

- ⬜ **Crochet Chart Editor** _(stubbed)_: Symbol-based charts for amigurumi, granny squares, filet, Tunisian, and freeform crochet on rectangular and radial grids. Generates written pattern text from the chart with standard US or UK terminology.

- ⬜ **Weaving Draft Editor** _(stubbed)_: Edits threading, tie-up, and treadling with live drawdown, network drafting, profile drafts, and color-and-weave previews. Supports multi-shaft looms up to 32 shafts and exports WIF for loom software.

- ⬜ **Beading Pattern Layout** _(stubbed)_: Grid-correct layouts for peyote, brick, herringbone, loom, right-angle weave, and bead crochet ropes. Maintains bead palettes keyed to Miyuki, Toho, and Czech codes and counts beads per color for the bill of materials.

- ⬜ **Quilt Block Designer** _(stubbed)_: Block library and layout canvas with fabric swatches, sashing, borders, and on-point sets. Auto-computes piecing yardage, cutting list, and half-square-triangle math, and renders a fabric-colored preview.

- ⬜ **Dye Recipe Tracker** _(stubbed)_: Logs immersion, low-water, acid, fiber-reactive, and natural dye recipes with weight-of-fiber percentages, depth of shade, mordants, water hardness, and temperature curves. Reproduces a target color from a recorded swatch and predicts recipe scaling.

- ⬜ **Yarn Substitution Calculator** _(stubbed)_: Matches a target yarn by gauge, wraps per inch, fiber blend, twist, and weight category against a personal stash database. Recomputes ball quantity needed for a given pattern when substituting.

- ⬜ **Stitch Library** _(stubbed)_: Searchable reference of embroidery, knit, crochet, sashiko, and tatting stitches with diagrams, video loops, and tension notes. Insertable into patterns as named stitches with parameters.

- ⬜ **Colorway Palette Builder** _(stubbed)_: Extracts palettes from reference images and matches each color to thread, floss, yarn, and bead inventories. Tests palettes against simulated daylight and tungsten white points and exports a colorway sheet.

- ⬜ **Tufting Rug Designer** _(stubbed)_: Designs cut-pile and loop-pile rugs with pile-direction arrows, color blocks, and backing dimensions. Computes yarn poundage per color and generates a projector-ready outline for the tufting cloth.

- ⬜ **Smocking Pleat Calculator** _(stubbed)_: Computes pleater rod counts, dot grids, and fabric width for English, North American, and counterchange smocking. Generates dot transfer sheets and predicts finished panel width from gather ratio.

- ⬜ **Stash And Project Ledger** _(stubbed)_: Tracks fabric, yarn, floss, and bead inventory with photos, fiber content, dye lot, and remaining quantity, and links materials to active projects. Deducts consumption automatically from finished patterns and warns when a project would overdraw stash.


#### Cooking, brewing, fermentation, food science

- ⬜ **Recipe Scaler** _(stubbed)_: Scales ingredients with non-linear adjustments for spices, leaveners, and salt; recomputes pan dimensions and bake times when geometry changes. AI suggests safe scaling bounds where a straight multiply would break the recipe.

- ⬜ **Nutrition Calculator** _(stubbed)_: Builds per-serving macro and micronutrient panels from USDA FDC and custom ingredients, with optional FDA-format Nutrition Facts label rendering and allergen flagging.

- ⬜ **Baker's Percentage Workbench** _(stubbed)_: Edits formulas as baker's percentages with total dough weight, hydration, salt, and pre-ferment targets; back-solves any field from any other and validates against style ranges.

- ⬜ **Sous-Vide Pasteurization Table** _(stubbed)_: Computes time-to-pasteurization by core thickness and target log reduction for poultry, beef, pork, fish, and egg using published D and z values, with come-up time correction.

- ⬜ **Equilibrium Cure & Brine Calculator** _(stubbed)_: Solves salt, sugar, and nitrite (Prague #1 and #2) percentages by total system mass for wet, dry, and EQ cures; enforces safe ppm bounds and estimates dwell time by thickness.

- ⬜ **Brewing Recipe Builder** _(stubbed)_: Designs all-grain, extract, and BIAB recipes with grain bill, mash schedule, hop schedule, and yeast; outputs OG, FG, ABV, SRM, and IBU under selectable Tinseth, Rager, and Garetz models.

- ⬜ **Mash Water Chemistry Designer** _(stubbed)_: Models source water plus salt and acid additions against style-appropriate sulfate, chloride, and calcium targets; predicts mash pH from grist color and acidulated malt percentage.

- ⬜ **Yeast Pitch & Starter Designer** _(stubbed)_: Computes pitch rate by gravity and volume for ale, lager, and Brettanomyces strains; designs stepped starters using stir-plate vs intermittent-shaking growth factors and viability decay.

- ⬜ **Gravity Log & Curve Fitter** _(stubbed)_: Logs hydrometer and refractometer readings with temperature and wort-correction-factor adjustment; fits Gompertz or logistic fermentation curves and forecasts terminal gravity and attenuation.

- ⬜ **Wine Must Adjustment Bench** _(stubbed)_: Solves chaptalization, acid additions, and SO2 dosing as a function of pH for free vs molecular SO2; tracks malolactic conversion, fining, and oak regimens across a vintage.

- ⬜ **Sake Sandan-Jikomi Planner** _(stubbed)_: Schedules three-step moromi additions with rice, koji, mizu, and shubo masses; computes seimaibuai, expected SMV (nihonshudo), and acidity targets across the moto and main mash.

- ⬜ **Lacto-Ferment Workbench** _(stubbed)_: Solves salt percentage and brine ratios for vegetable ferments, kimchi, and pickles; tracks pH, temperature, and CO2 vent timeline with safety thresholds for Listeria and Clostridium.

- ⬜ **Sourdough Schedule Builder** _(stubbed)_: Plans starter feedings, levain build, autolyse, bulk, and cold retard against ambient temperature and starter activity; estimates dough doubling time and final crumb hydration.

- ⬜ **Cheesemaking Worksheet** _(stubbed)_: Templates fresh, bloomy, washed-rind, and aged cheese makes with target pH and temperature curves, rennet and culture dosing, draining schedule, and affinage parameters.

- ⬜ **Modernist Hydrocolloid Calculator** _(stubbed)_: Computes agar, gellan (high and low acyl), xanthan, methylcellulose, and alginate doses for gels, fluid gels, foams, and spherification by texture target and syneresis tolerance.

- ⬜ **Flavor & Aroma Pairing Explorer** _(stubbed)_: Maps shared volatile aroma compounds across ingredients with overlap scoring. AI-integrated suggestion of novel pairings constrained by cuisine, season, and dietary rules.

- ⬜ **Smoke, Roast & Stall Modeler** _(stubbed)_: Predicts internal temperature curves for low-and-slow cooks including the evaporative stall; recommends wood, rub, and wrap timing by protein cut and target bark.

- ⬜ **Spirit Proofing & Cuts Workbench** _(stubbed)_: Handles ABV and temperature correction, Pearson square dilution and blending, and heads/hearts/tails cut planning by congener volatility where legally applicable.

- ⬜ **Coffee & Tea Extraction Bench** _(stubbed)_: Computes TDS, extraction yield, brew ratio, and grind setting for pour-over, espresso, immersion, and tea; logs refractometer reads against brew curves and roast profile.

- ⬜ **Shelf-Life & Spoilage Modeler** _(stubbed)_: Predicts shelf life from water activity, pH, salt, and storage temperature using Q10 kinetics and published pathogen growth boundaries, with HACCP-style critical control point review.

- ⬜ **Sourdough Starter Logger** _(stubbed)_: Tracks feedings, hydration ratio, peak times, and rise heights across multiple starters. Plots ambient temperature against doubling time to predict readiness windows.

- ⬜ **Kombucha SCOBY Tracker** _(stubbed)_: Logs brew batches with starting tea pH, sugar concentration, and daily acidity readings. Estimates F1 finish dates from temperature curves and flags vinegar-tier overshoot risk.

- ⬜ **Koji and Miso Schedule** _(stubbed)_: Schedules koji incubation, salt-cure timing, and miso aging milestones for shiro, aka, and hatcho styles. Adjusts moromi turning intervals based on logged temperature and humidity readings.

- ⬜ **Lacto-Fermentation Acidity Tracker** _(stubbed)_: Records brine salinity, pH, and titratable acidity for kimchi, sauerkraut, kosher dills, and giardiniera. Computes lactic acid percentage from titration volumes and warns when pH stalls outside safe descent curves.

- ⬜ **Hot Sauce Capsaicin Calculator** _(stubbed)_: Estimates Scoville heat units from pepper varietal mass ratios and dilution factors in the final sauce. Accepts HPLC-style capsaicinoid input when available and back-solves blend ratios to hit a target SHU.

- ⬜ **Dehydration and Curing Scheduler** _(stubbed)_: Plans drying schedules for jerky, biltong, charcuterie, and herbs with target water activity and equilibrium humidity. Models weight loss curves and flags hazard windows where surface case hardening or mold growth is likely.

- ⬜ **Modified Atmosphere Packaging Reference** _(stubbed)_: Looks up recommended gas mixtures (CO2, N2, O2 residuals) for cheese, cured meats, produce, and coffee. Calculates headspace volume, gas flush durations, and OTR/MVTR film selection for a given shelf-life target.

- ⬜ **Water-Bath and Pressure Canning Guide** _(stubbed)_: Provides validated process times for high-acid and low-acid recipes indexed by jar size, altitude, and product density. Cross-references USDA and NCHFP guidance and refuses to generate schedules outside tested parameter envelopes.

- ⬜ **Brewing and Mead Hydrometer Lab** _(stubbed)_: Tracks original and final gravity, temperature-corrected SG, ABV, attenuation, and FAN for beer, wine, mead, and cider. Estimates yeast pitch rates and predicts fermentation finish from logged Brix and gravity curves.

- ⬜ **Vinegar and Acetification Monitor** _(stubbed)_: Logs surface vs submerged acetification batches with alcohol-by-volume input, ambient O2 access, and titrated acidity output. Predicts mother-of-vinegar maturation timing and flags stuck conversions.

- ⬜ **Cheese Make Planner** _(stubbed)_: Walks through milk pasteurization, culture inoculation, rennet set, curd cut, cook, drain, press, brine, and affinage stages. Logs target pH and titratable acidity at each step and adjusts hold times when readings drift.

- ⬜ **Cold-Smoke and Hot-Smoke Calculator** _(stubbed)_: Computes wood-to-protein ratios, smoke residency time, and equivalent surface drying for cold smoking, hot smoking, and sous vide finish. Highlights nitrite/nitrate cure levels per Cure #1 and Cure #2 reference tables.

- ⬜ **Salt and Brine Chemistry Solver** _(stubbed)_: Converts between percent salt by weight, baume, salometer degrees, and equilibrium brine for a given product mass. Includes nitrite ppm targets, sugar offsets, and water hardness adjustments.

- ⬜ **Tempeh and Natto Incubator Log** _(stubbed)_: Tracks substrate hydration, acidulation, inoculation temperature, and mycelium or bacillus growth across multi-day incubations. AI summarizes failure modes from photo logs when batches go ammoniated or patchy.

- ⬜ **Ingredient Substitution and Flavor Pairing Assistant** _(stubbed)_: Maps ingredients by dominant volatile compounds, fermentation behavior, and acid/sugar/salt ratios. AI suggests substitutions that preserve safety-critical parameters (Aw, pH, salt percent) rather than only flavor.

- ⬜ **Recipe Scaling and Yield Tracker** _(stubbed)_: Scales fermentation and curing recipes by ingredient, batch volume, or vessel headspace while preserving ratio-critical parameters. Records actual yield, loss, and shrinkage against predicted values across runs.

- ⬜ **Pantry and Culture Inventory** _(stubbed)_: Tracks live cultures (starters, mothers, koji, kefir grains), spice freshness, oil rancidity windows, and freezer cure stock. Issues refresh and feed reminders based on observed inactivity intervals.


#### Coffee, tea, wine, spirits, beverages

- ⬜ **RoastProfileStudio** _(stubbed)_: Live roast logger capturing bean temp, environmental temp, and Rate of Rise with timestamped events (charge, turning point, yellow, first crack, drop). Replays curves, overlays batches, and flags crash/flick artifacts.

- ⬜ **EspressoShotLog** _(stubbed)_: Per-shot record of dose, yield, time, grind setting, basket, water temp, pressure profile, TDS, and extraction yield. Trends results per bean over time and surfaces drift in grinder or beans.

- ⬜ **ExtractionCalculator** _(stubbed)_: Solves brew ratio, TDS, and extraction yield for pour-over, immersion, and espresso given any two known variables. Includes refractometer correction factors per method.

- ⬜ **WaterChemistryBuilder** _(stubbed)_: Targets coffee and brewing water recipes (SCA, Hendon, Rao, Burton, Pilsen) by computing CaCl2, MgSO4, NaHCO3, and CaSO4 additions from a source water report. Outputs mash, sparge, and dilution blends.

- ⬜ **TeaSessionPlanner** _(stubbed)_: Gongfu and western brewing parameters per cultivar with temperature, leaf-to-water ratio, and infusion ladder. Logs infusion-by-infusion sensory notes and adjusts steep times against observed strength.

- ⬜ **CocktailWorkbench** _(stubbed)_: Spec builder enforcing sugar, acid, and ABV balance with stir, shake, and build dilution models. Generates build cards, scales to batch size, and tracks ingredient substitutions.

- ⬜ **DilutionCalculator** _(stubbed)_: Solves final ABV, sugar, and acid given ice melt, top-up, water cuts, or proofing additions. Supports cask strength reductions and barrel-proof bottling targets.

- ⬜ **WineTastingNotes** _(stubbed)_: Structured WSET SAT capture covering appearance, nose, palate, and conclusions, with grape, region, and producer taxonomy. Supports blind tasting modes and computes calibration accuracy over time.

- ⬜ **CellarInventory** _(stubbed)_: Tracks bottle location, vintage, provenance, purchase price, storage conditions, and projected drink window. Alerts on bottles drifting past peak and supports cellar valuation snapshots.

- ⬜ **DistillationCalculator** _(stubbed)_: Logs wash ABV, parrot readings, vapor and liquid temperatures, and cut points across heads, hearts, and tails. Computes proofing-down, angel's share, and yield per fermentable; flags congener thresholds.

- ⬜ **BeerRecipeDesigner** _(stubbed)_: Mash schedule, strike water, hop IBU via Tinseth, Rager, and Garetz, yeast pitch rate, and attenuation forecast in one recipe. Exports BeerXML and compares predicted versus measured gravities.

- ⬜ **CarbonationCalculator** _(stubbed)_: Computes CO2 volumes from temperature and pressure for force carbonation, priming sugar dosage, or natural conditioning. Includes nitro blend ratios and bottle pressure safety checks.

- ⬜ **FermentationLog** _(stubbed)_: Gravity, Brix, pH, and temperature curves for beer, wine, mead, cider, and kombucha. Detects stuck fermentation, predicts terminal gravity, and prompts dry hop or rack windows.

- ⬜ **CuppingSession** _(stubbed)_: SCA cupping form with the ten scored attributes plus Q-grade overlays, triangle and duo-trio panels, and blind randomization. Aggregates panel scores and flags outlier graders.

- ⬜ **FoodPairingEngine** _(stubbed)_: AI-assisted pairing across coffee, tea, wine, beer, and spirits given a dish, cooking technique, and dominant flavor compounds. Cross-references aromatic congruence and contrast strategies with cited tasting precedents.

- ⬜ **SensoryWheel** _(stubbed)_: Tags tastings against SCA, WSET, BJCP, and spirits flavor wheels with shared aroma compound mappings. Cross-references descriptors across modalities and tracks personal vocabulary calibration.


#### Traditional crafts: horology, blacksmithing, jewelry, glassblowing, pottery, leatherwork, cobbling

- ⬜ **Cobbling and shoe repair** _(stubbed)_: sole/welt construction reference (Goodyear, Blake, Norwegian), last sizing, last-grading by foot type, leather and lasting/closing materials database with sourcing, heel-height pattern adjustment, repair logbook with photo before/after.

- ⬜ **Gear Train Calculator** _(stubbed)_: Computes wheel and pinion tooth counts, ratios, and beats-per-hour for clock and watch movements. Solves for missing members given a target frequency, mainspring torque, and going-train depth.

- ⬜ **Escapement Designer** _(stubbed)_: Lays out lever, cylinder, deadbeat, and detent escapement geometries with pallet angles, drop, lock, and impulse arcs. Renders interactive SVG and exports DXF for jig cutting.

- ⬜ **Pendulum and Balance Tuner** _(stubbed)_: Calculates pendulum length, period, and temperature compensation for gridiron and invar rods; for balance wheels, derives moment of inertia, hairspring stiffness, and isochronism error from amplitude.

- ⬜ **Mainspring and Power Reserve Solver** _(stubbed)_: Sizes mainspring thickness, height, length, and barrel diameter for a target torque curve and runtime. Plots residual power across the unwinding cycle and flags fusee compensation needs.

- ⬜ **Alloy Composer** _(stubbed)_: Selects ferrous, copper, silver, and gold alloys by hardness, color, melting range, workability, and cost. Predicts liquidus and solidus from composition, suggests deoxidizers, and surfaces hallmark thresholds (sterling, 14k, 18k, britannia, shibuichi, shakudo).

- ⬜ **Forge Heat and Color Reference** _(stubbed)_: Maps visible incandescent color to temperature for low-light shop conditions across carbon steels, wrought iron, bronze, and silver. Cross-references forging, normalizing, hardening, and tempering windows for the selected alloy.

- ⬜ **Heat Treatment Planner** _(stubbed)_: Builds quench and temper schedules for tool steels, spring steels, and pattern-welded billets with soak times derived from cross-section thickness. Suggests quenchant, interrupted cycles, and cryo treatment for retained austenite.

- ⬜ **Damascus Layer Calculator** _(stubbed)_: Plans pattern-welded billet stacks: starting layer count, folds, target layer count, drawn length, and finished thickness. Previews canister, ladder, raindrop, and mosaic patterns and estimates etch contrast from alloy nickel content.

- ⬜ **Ring and Chain Sizer** _(stubbed)_: Converts ring sizes across US, UK, EU, JP, and CN scales; computes blank length from finger size, profile, and metal stretch. For chains, derives wire gauge, link aspect ratio, and weave density for byzantine, jens pind, foxtail, and european weaves.

- ⬜ **Gemstone Setting Reference** _(stubbed)_: Catalogs prong, bezel, channel, pave, flush, and tension settings with seat geometry, undercut angles, and bur sizes for cushion, round, oval, marquise, and fancy cuts. Warns on cleavage planes, heat sensitivity, and acid intolerance per species.

- ⬜ **Lapidary Facet Layout** _(stubbed)_: Designs and previews faceted gem cuts with critical-angle checks per refractive index, dispersion modeling, and yield estimation from rough dimensions. Exports cutting prescriptions in GemCAD-compatible angle and index notation.

- ⬜ **Glaze Chemistry Calculator** _(stubbed)_: Computes Seger and unity molecular formulae from raw material recipes, solves for limit-formula targets, and substitutes materials by available analyses. Flags eutectics, COE mismatch with the clay body, and food-safe boundaries for lead, barium, lithium, and cadmium.

- ⬜ **Kiln Firing Schedule Builder** _(stubbed)_: Constructs multi-segment ramp-hold-cool programs for bisque, glaze, lustre, raku, and crystalline firings. Adjusts for kiln mass, element load, and cone deformation; predicts quartz inversion dwells and exports to Bartlett, Orton, and Skutt controller syntax.

- ⬜ **Clay Body Formulator** _(stubbed)_: Blends earthenware, stoneware, and porcelain bodies from feldspar, silica, kaolin, ball clay, and grog by target shrinkage, absorption, plasticity, and maturing cone. Predicts thermal expansion for glaze fit and warns on dunting and crawling risks.

- ⬜ **Glass COE and Color Match** _(stubbed)_: Compatibility checker for soft glass, borosilicate, quartz, and lead crystal across vendors (Bullseye 90, System 96, Effetre 104, Boro 33). Stores striking and reduction color libraries with working temperature ranges and suggests anneal soak times by section thickness.

- ⬜ **Annealing and Lehr Planner** _(stubbed)_: Derives anneal point, strain point, hold, and cool-rate schedules for glass pieces by thickness and geometry; for metals, plans recrystallization anneals for sterling, copper, brass, gold, and titanium with pickle and oxide-mitigation steps.

- ⬜ **Leather Pattern Drafter** _(stubbed)_: Unfolds 3D shapes into flat patterns for bags, sheaths, holsters, saddle skirts, and bookbinding cases with seam, turn, and skive allowances. Computes stitch line offsets, edge bevel, and burnish length per leather weight and tannage.

- ⬜ **Saddle Stitch and Punch Spacing** _(stubbed)_: Calculates pricking iron pitch, thread diameter, awl angle, and SPI for a given leather thickness and seam strength target. Generates printable punch templates and corner radii to keep stitch count even around curves.

- ⬜ **Patina and Finish Recipe Book** _(stubbed)_: Catalogs liver-of-sulfur, cupric nitrate, ferric, fume, heat, and electrochemical patinas for copper, bronze, brass, silver, and steel; for leather, vinegaroon, oak gall, and oil finishes. Tracks dilutions, dwell, neutralization, and seal coats with reproducibility notes.

- ⬜ **Hallmark and Assay Reference** _(stubbed)_: Searchable database of maker, assay-office, date, and standard marks across UK, France, Russia, Sweden, and Japan, with touchstone and acid-test fineness procedures. Includes XRF interpretation hints and minimum legal finenesses by jurisdiction.

- ⬜ **Engraving and Chasing Pattern Studio** _(stubbed)_: Vector design surface for hand engraving, chasing, repousse, and bulino with bright-cut shading, scroll layout aids, and mirror-image preview for transfer. AI-assisted style transfer adapts a sketch into period-appropriate acanthus, arabesque, or mokume motifs while preserving cut-friendly line weights.

- ⬜ **Wax and Lost-Wax Casting Planner** _(stubbed)_: Sizes sprues, gates, vents, and risers for centrifugal and vacuum casting by alloy and pattern volume; computes investment water ratio, burnout schedule, and flask preheat. Estimates metal weight from wax weight per alloy and flags shrinkage porosity hotspots.

- ⬜ **Forge and Kiln Build Calculator** _(stubbed)_: Sizes propane and coal forges, gas kilns, and glass furnaces: chamber volume, burner BTU, refractory thickness, flue draft, and insulating firebrick layer counts. Estimates fuel burn rate and reaches-temperature time for a target peak.

- ⬜ **Project Costing and Stock Ledger** _(stubbed)_: Tracks raw stock (bar, sheet, wire, sprue, ingot, clay, glass rod, hide) with weight, dimensions, and per-unit cost; computes piece cost from kerf, scrap, fuel, electricity, and labor. Generates client quotes and bench-time invoices.


#### Visual arts: calligraphy, painting, sculpture

- ⬜ **Palette extractor** _(stubbed)_: Extracts dominant colors from a reference image using k-means or median-cut quantization, returns swatches with hex/RGB/OKLCH values, coverage percentages, and optional weighting by saliency.

- ⬜ **Color harmony generator** _(stubbed)_: Builds complementary, split-complementary, triadic, tetradic, square, and analogous schemes around a base hue, with editing in OKLCH or HSLuv so lightness stays perceptually even across the wheel.

- ⬜ **Gradient designer** _(stubbed)_: Composes multi-stop gradients with per-stop easing and interpolation chosen across sRGB, linear RGB, OKLab, OKLCH, or LCh, and exports CSS, SVG, GIMP .ggr, and Krita .svg presets.

- ⬜ **Color space converter** _(stubbed)_: Round-trips colors between sRGB, Adobe RGB, Display P3, Rec.2020, CIE XYZ, CIELAB, OKLab, HSL, HSV, and CMYK with explicit white point and gamma handling, and flags out-of-gamut values.

- ⬜ **Color vision simulator** _(stubbed)_: Renders an image or palette as seen under protanopia, deuteranopia, tritanopia, achromatopsia, and low-contrast vision, and scores adjacent swatch pairs for confusability.

- ⬜ **Pigment mixer** _(stubbed)_: Simulates subtractive paint mixing using a Kubelka-Munk two-constant model over a built-in library of watercolor, oil, acrylic, and gouache pigments with lightfastness, granulation, and staining metadata.

- ⬜ **Value and notan studio** _(stubbed)_: Posterizes a reference image to 2 to 9 value steps, generates thumbnail variants, and overlays a notan mask so light/dark composition can be judged independently of hue.

- ⬜ **Perspective grid overlay** _(stubbed)_: Generates 1-, 2-, 3-, 4-, and 5-point perspective grids plus fisheye and curvilinear projections, with adjustable horizon, station point, cone of vision, and exportable transparent overlays for tracing.

- ⬜ **Calligraphy nib simulator** _(stubbed)_: Simulates broad-edge, pointed-pen, and brush-pen strokes with pressure- and angle-driven contrast, supports historical hands (Italic, Copperplate, Uncial, Kufic, Sōsho), and produces guide sheets at requested x-height and slant.

- ⬜ **Brushwork trainer** _(stubbed)_: Plays stepped exercises for pressure control, hairline-to-swell transitions, ovals, and ductus drills, with AI-integrated stroke analysis that scores the user trace against the target path for smoothness and angle.

- ⬜ **Anatomy reference viewer** _(stubbed)_: Displays a layered écorché of skeleton, deep muscle, superficial muscle, fascia, and skin with per-layer opacity, sex/build presets, and labeled origins and insertions.

- ⬜ **Pose mannequin** _(stubbed)_: Poses a 3D figure via IK handles or imported BVH/Mixamo clips, controls camera, lighting, and lens length, and exports turnaround sheets or single-frame references with adjustable line-art shading.

- ⬜ **Gesture session timer** _(stubbed)_: Runs timed life-drawing sessions over a local pose library or user folder, with configurable 30s/1m/2m/5m/10m intervals, warmup ramps, and grayscale or silhouette modes.

- ⬜ **Kiln schedule planner** _(stubbed)_: Builds segmented ramp/hold/cool firing schedules by cone and atmosphere (oxidation, reduction, salt, raku), estimates work-heat via Orton cone tables, and exports controller programs for Bartlett, Skutt, and Genesis profiles.

- ⬜ **Clay and shrinkage calculator** _(stubbed)_: Tracks wet, leather-hard, bone-dry, bisque, and glaze-fired dimensions for a clay body, computes total linear and volumetric shrinkage, and back-solves the wet size needed to hit a target finished dimension.

- ⬜ **Maquette viewer** _(stubbed)_: Loads STL, OBJ, PLY, and 3MF maquettes with PBR or matcap shading, measures distances/angles/cross-sections, slices for armature planning, and snapshots reference views at standard sculptor angles.

- ⬜ **Papercraft unfolder** _(stubbed)_: Unfolds a 3D mesh into flat polygon strips with user-controlled cut and glue-tab seams, optimizes for sheet count and tab visibility, and exports printable PDF/SVG with fold-direction line styles.

- ⬜ **Tessellation and pattern designer** _(stubbed)_: Constructs repeating motifs across all 17 wallpaper groups, 7 frieze groups, and Penrose/quasiperiodic tilings, with live symmetry constraints and seamless tile export for textiles, mosaic, or stained-glass cartoons.


#### Numismatics, philately, collecting, antiques

- ⬜ **Catalog Reference Browser** _(stubbed)_: Local mirror of standard catalogs (Krause/SCWC for coins, Scott for stamps, Pick for banknotes) with cross-indexed entries by year, country, denomination, and KM/Scott/Pick number. Supports offline lookup and side-by-side variant comparison.

- ⬜ **Attribution Assistant** _(stubbed)_: Identifies an unknown coin, stamp, or note from a photo or rubbing using on-device vision plus catalog metadata. Returns ranked candidate matches with KM/Scott numbers, mint years, and diagnostic features (legends, dates, watermarks, perforations).

- ⬜ **Grading Guide** _(stubbed)_: Side-by-side reference plates for major grading standards (Sheldon 1-70 for coins, PSE/PF for stamps, PMG for paper money) with annotated wear points. Guides user through a grading interview and produces a defensible grade with notes.

- ⬜ **Mint Mark and Variety Tracker** _(stubbed)_: Database of mint marks, die varieties, double dies, RPMs, overdates, plate flaws, and printing varieties keyed to catalog numbers. Flags collectible varieties versus errors versus PMD on a submitted specimen.

- ⬜ **Hallmark and Purity Decoder** _(stubbed)_: Reference for silver, gold, platinum, and pewter hallmarks across jurisdictions (Britannia, Sheffield assay, French poincon, Russian zolotnik, US sterling). Decodes maker, assay office, date letter, and fineness from struck marks.

- ⬜ **Auction Price Comparator** _(stubbed)_: Aggregates realized prices from Heritage, Stack's Bowers, Spink, Kunker, Cherrystone, and other house archives, normalized by grade and population. Plots price-by-grade curves and flags outlier results worth investigating.

- ⬜ **Population and Census Tracker** _(stubbed)_: Local snapshot of PCGS, NGC, CAC, PSE, and PMG population reports per issue and grade. Computes rarity ratios, condition census standings, and registry-set scoring against user holdings.

- ⬜ **Provenance and Pedigree Ledger** _(stubbed)_: Chain-of-custody log linking each item to prior owners, named collections (Eliasberg, Norweb, Pittman, Ferrari), auction lot numbers, and dated invoices. Generates a provenance dossier with citations for sale or insurance.

- ⬜ **Condition and Conservation Log** _(stubbed)_: Per-item record of toning, planchet flaws, stamp gum state, hinge marks, fold lines, foxing, and prior interventions (cleaning, dipping, pressing, restoration). Tracks change over time with dated macro photos and notes.

- ⬜ **Die Marriage and Plate Mapper** _(stubbed)_: Tool for early US coppers, bust coinage, ancient Greek and Roman issues, and classic stamp plates: aligns specimen photos to known die or plate diagnostics (Sheldon, Overton, Bolender, Newcomb; Mayer, Crawford, RIC; Stanley Gibbons plating). Returns most likely die marriage or plate position.

- ⬜ **Bullion and Melt Calculator** _(stubbed)_: Real-time and historical spot price feeds for Au, Ag, Pt, Pd, Cu, Ni applied to coin weights and finenesses by KM number. Computes intrinsic melt floor versus numismatic premium and break-even spreads.

- ⬜ **Counterfeit and Alteration Detector** _(stubbed)_: Checks weight, diameter, edge, magnetic response, specific gravity, and surface texture against catalog specs; pattern-matches against known fake dies (Omega, Henning, Micro O, Chinese counterfeit families) and altered-date diagnostics. Flags suspect pieces with reasoning.

- ⬜ **Watermark and Perforation Analyzer** _(stubbed)_: For stamps and banknotes: detects watermarks via fluid-free image enhancement, measures perforation gauge to 0.1, and identifies paper type, gum, and printing method (engraved, typographed, photogravure, offset). Outputs Scott/SG-compatible attribution data.

- ⬜ **Hoard and Find Atlas** _(stubbed)_: Geo-tagged archive of documented coin hoards, shipwreck recoveries (Atocha, SS Central America, El Cazador), and stamp finds with composition tables and date ranges. Useful for sourcing provenance hints and dating unprovenanced lots.

- ⬜ **Antiques Provenance and Appraisal Workbench** _(stubbed)_: General antiques module covering furniture, silver hollowware, ceramics marks (Meissen, Sevres, Wedgwood, Imari), and clock movements. Captures dimensions, marks, condition, repairs, and comparable sales to produce fair market value, replacement value, and forced-sale estimates.

- ⬜ **Inventory and Insurance Manifest** _(stubbed)_: Holdings ledger with cost basis, acquisition source, storage location (safe, SDB, album page), insured value, and depreciation schedule. Exports schedule-of-items PDFs for riders, estate planning, and customs declarations.

- ⬜ **Reference Library and OCR Shelf** _(stubbed)_: Searchable local corpus of out-of-copyright and licensed numismatic and philatelic literature (ANS journals, Numismatic Chronicle, Linn's, Coin World back issues, Spink Standard Catalogues). Full-text OCR with citation export and AI-assisted question answering grounded in the indexed sources.


#### Body art: tattoo and body modification

- ⬜ **Stencil Transfer Designer** _(stubbed)_: Vectorizes line art and converts raster references into clean single-line stencils sized to print on thermal or hectograph paper. Supports mirroring, registration marks, and multi-page tiling for large back pieces.

- ⬜ **Skin Tone Preview** _(stubbed)_: Renders the design over a calibrated swatch of Fitzpatrick types I through VI, simulating ink saturation, healing fade, and contrast loss on each tone. AI color-correction suggests adjusted palettes for darker skin.

- ⬜ **Body Placement Mockup** _(stubbed)_: Warps the design onto an uploaded body photo using pose-aware mesh fitting so the art follows curvature of limbs, ribs, and spine. Includes flexion poses to check stretch behavior.

- ⬜ **Needle and Cartridge Reference** _(stubbed)_: Searchable database of liner, shader, magnum, and curved configurations with diameter, taper, and recommended voltage ranges. Cross-references brands and equivalents across cartridge systems.

- ⬜ **Ink Library and Mix Calculator** _(stubbed)_: Tracks pigment brands, batch numbers, and MSDS data, and computes mix ratios for custom greywash dilutions or color blends. Flags inks containing reportable allergens or banned EU REACH pigments.

- ⬜ **Aftercare Protocol Generator** _(stubbed)_: Produces client-specific aftercare instructions based on placement, size, technique, and skin type, with day-by-day timelines. AI adapts wording for reading level and translates to client's preferred language.

- ⬜ **Jewelry Sizing Calculator** _(stubbed)_: Converts gauge, diameter, and length across metric and imperial standards for barbells, rings, labrets, and plugs. Accounts for swelling allowances and anatomy-specific minimums (septum, daith, surface).

- ⬜ **Healing Log** _(stubbed)_: Per-client journal capturing daily photos, exudate notes, pain scores, and intervention events with automatic timeline view. Flags deviations from expected healing curves and suggests when to escalate to a physician.

- ⬜ **Design Library** _(stubbed)_: Tagged archive of flash, custom commissions, and reference images with style, motif, body zone, and client provenance metadata. Semantic search finds visually similar pieces from natural-language prompts.

- ⬜ **Anatomy and Risk Atlas** _(stubbed)_: Interactive map of nerve, vessel, and lymphatic structures relevant to piercing and scarification, with depth and danger-zone overlays. Cites primary anatomical sources per region.

- ⬜ **Consent and Intake Forms** _(stubbed)_: Builds compliant consent, medical history, and minor-guardian forms with e-signature and ID capture, scoped by jurisdiction. Stores encrypted records with retention timers matching local health code requirements.

- ⬜ **Sterilization and Bloodborne Log** _(stubbed)_: Tracks autoclave cycles, spore test results, sharps disposal, and surface barrier changes against OSHA and local health department schedules. Generates inspection-ready reports.

- ⬜ **Style Reference Generator** _(stubbed)_: AI image synthesis tuned for tattoo idioms: traditional, neo-trad, blackwork, irezumi, tribal, single-needle, biomech, ornamental. Outputs are watermarked as reference-only and never used as final stencils without artist line work.

- ⬜ **Pricing and Session Planner** _(stubbed)_: Estimates session count and pricing from design dimensions, detail density, and placement difficulty, with hourly versus piece-rate modes. Schedules touch-up windows aligned to expected healing time.

- ⬜ **Removal and Cover-Up Planner** _(stubbed)_: Predicts laser fade response per ink color and Fitzpatrick type, recommends session counts and wavelengths, and overlays candidate cover-up designs on the existing piece. Models residual ghosting for darker cover artwork.


#### Performing arts: theater, sound, lighting, stage

- ⬜ **Cue Stack Programmer** _(stubbed)_: Builds ordered cue lists with audio, video, MIDI, OSC, and DMX cues. Supports fades, follow cues, wait times, loops, devamps, and continue actions in the QLab/SCS lineage.

- ⬜ **Lighting Plot Drafter** _(stubbed)_: Drafts a 2D plot and section view with positions, pipes, booms, and fixtures pulled from a profile library; tracks channel, address, dimmer, circuit, color, gobo, accessory, and focus note per unit.

- ⬜ **Console Syntax Editor** _(stubbed)_: Offers an Eos/grandMA-style command line over groups, palettes, presets, and cue lists. Edits intensity, focus, color, beam, and effect parameters with keypad-style syntax and tracking semantics.

- ⬜ **Previz Renderer** _(stubbed)_: Renders fixtures in a 3D venue model driven by live sACN/Art-Net/DMX input. Shows beams, shutter cuts, gobo rotation, prism break-up, and CMY/RGB color mixing for offline programming and dry tech.

- ⬜ **Magic Sheet Builder** _(stubbed)_: Lays out clickable channel buttons, group thumbnails, and palette tiles over a stage diagram. The designer pokes a position to recall a focus palette or to see at a glance what is live.

- ⬜ **Photometric Calculator** _(stubbed)_: Computes throw distance, beam and field angles, footcandle/lux at the deck, edge falloff, and pool diameter from each fixture's photometric data and hang height.

- ⬜ **Audio Patch Designer** _(stubbed)_: Documents the full signal flow: mics, DIs, stage box channels, console inputs, inserts, buses, matrix sends, amps, and speaker zones. Tracks snake counts, cable lengths, and rack U positions.

- ⬜ **Speaker Coverage Predictor** _(stubbed)_: Maps SPL coverage, direct-to-reverberant ratio, and STI across the seating plan from speaker placement, aim angles, splay, and a venue acoustic model in the MAPP/EASE lineage.

- ⬜ **Acoustic Measurement Suite** _(stubbed)_: Performs dual-channel FFT transfer function, RTA, impulse response, RT60, and coherence measurement against a reference signal; used to tune a PA or characterize a room.

- ⬜ **RF Coordination Planner** _(stubbed)_: Assigns wireless mic and IEM frequencies inside a license-free band, avoiding intermodulation products through third and fifth order, local broadcast TV channels, and on-site bleed.

- ⬜ **Set Drafting Workspace** _(stubbed)_: Drafts ground plan, section, and elevation views at scale with line weights, hatches, dimensions, and a title block. Exports DWG/PDF for the scene shop and load-in crew.

- ⬜ **Blocking Notation Pad** _(stubbed)_: Captures actor positions on a top-down stage diagram beat by beat or page by page. Scrubs through the script timeline to see entrances, crosses, exits, and stage picture changes.

- ⬜ **Script Breakdown Grid** _(stubbed)_: Parses the script into French scenes and produces a character/scene presence grid with tagged props, costumes, sound effects, and lighting moments. AI-integrated parsing extracts entrances, exits, and implicit cues from the dramatic text.

- ⬜ **Rehearsal Scheduler** _(stubbed)_: Solves rehearsal calls against actor availability, French-scene presence, room and director conflicts, and Equity break rules. Emits daily call sheets and conflict reports.

- ⬜ **Costume and Wig Tracker** _(stubbed)_: Builds the change plot per actor per scene, flags quick changes shorter than the available stage time, and tracks fittings, dressers assigned, laundry runs, and wig dressing appointments.

- ⬜ **Calling Script Formatter** _(stubbed)_: Annotates the prompt book with standby, warn, go, and follow markers, plus channel and spot callouts, inline against the script PDF for the stage manager to call the show from.

- ⬜ **Paperwork Generator** _(stubbed)_: Generates channel hookup, instrument schedule, magic sheet, dimmer/circuit report, gel cut list, gobo order, and focus chart from the plot data in the Lightwright lineage.

- ⬜ **Show Control Router** _(stubbed)_: Monitors and routes LTC, MTC, MIDI, OSC, Art-Net, and sACN between devices. Logs traffic, generates and chases timecode, and triggers cues for paperless tech rehearsal.

- ⬜ **Rigging Load Calculator** _(stubbed)_: Computes pipe point loads, batten weight balance, and counterweight requirements for hung scenery and electrics. Flags overloads against rated capacity and reports per-pickup tension.


#### Magic, juggling, stage illusion, performance

- ⬜ **Card Trick Notation** _(stubbed)_: Plain-text format for recording card routines with deck state, controls, forces, switches, and reveals. Renders to step diagrams and replays the full deck sequence at any point in the routine.

- ⬜ **Sleight Video Annotator** _(stubbed)_: Frame-accurate tagging of double lifts, passes, palms, steals, and loads on recorded sessions. AI-assisted hand and object tracking builds a searchable sleight library across years of practice footage.

- ⬜ **Siteswap Editor** _(stubbed)_: Live editor for vanilla, synchronous, and multiplex siteswap with 3D ball animation, beat clock, and validity checking. Exports to Juggling Lab, prechac, and passing patterns for two or more jugglers.

- ⬜ **Stack and Setup Manager** _(stubbed)_: Tracks memorized deck orders such as Mnemonica, Aronson, and Si Stebbins with key card lookup, stay-stack analysis after cuts and faros, and printable reference indices for the wallet.

- ⬜ **Props Inventory** _(stubbed)_: Catalog of cards, coins, clubs, balls, silks, gimmicks, and consumables with condition tracking, retire-by dates, repair logs, travel case assignments, and per-routine load lists.

- ⬜ **Stage Plot Designer** _(stubbed)_: Top-down stage diagrams with prop positions, blocking marks, light angles, audience risers, and sightline cones. Computes blind spots and exposure risk from any seat in the house.

- ⬜ **Rehearsal Log** _(stubbed)_: Per-routine practice journal capturing drop counts, run times, tempo, video diary entries, and skill-acquisition curves. Flags regressions and surfaces overdue review items based on a forgetting curve.

- ⬜ **Audience Beat Sheet** _(stubbed)_: Timeline editor for buildup, misdirection, climax, and reveal beats with tension markers, applause windows, and patter sync. Calibrates pacing against recorded performances and live applause traces.

- ⬜ **Illusion Blueprint Workshop** _(stubbed)_: CAD-lite drafting for stage illusions with mirror angles, hinge geometry, load capacities, and bills of materials. Ships with a reference library of classic principles (Pepper's Ghost, black art, sub trunk, Zig Zag).

- ⬜ **Patter Script Editor** _(stubbed)_: Scripted dialog with embedded sleight cues, timing marks, ad-lib branches, and translation slots for touring. LLM assists in tightening lines, generating themed variants, and matching word counts to handling beats.

- ⬜ **Multiple Outs Planner** _(stubbed)_: Decision tree builder for equivoque, magician's choice, dual reality, and multi-prediction routines. Verifies every branch lands on a clean reveal and highlights paths with weak outs or audible giveaways.

- ⬜ **Misdirection Heatmap** _(stubbed)_: Simulated audience attention overlay derived from performer gesture, gaze, motion, and visual contrast in recorded footage. AI predicts where eyes go frame by frame and rates each sleight against ambient distraction.

- ⬜ **Lighting and Sound Cue Sheet** _(stubbed)_: DMX, MIDI, and OSC cue table synced to the beat sheet, with practice playback locally. Exports to QLab, ETC EOS, and Ableton sessions, and round-trips changes back from the console.

- ⬜ **Gig and Tour Manager** _(stubbed)_: Venue specs, tech riders, contracts, travel manifests, hotel blocks, and customs paperwork for prop transport including blades, livestock, and pyrotechnics. Generates load-in run sheets per date.

- ⬜ **Reveal Detector** _(stubbed)_: Scans performance video for tells: card flashes, palm knuckling, angle leaks, mic-bleed clicks, and shadow betrayals. AI flags vulnerable moments per camera position and suggests blocking or grip adjustments.

- ⬜ **Mentalism Toolkit** _(stubbed)_: Pre-show work tracker, billet management, peek device inventory, swami and nail-writer consumables, and a dual reality log to keep stooged versus organic outcomes straight across a tour.


#### Tabletop RPG and world building

- ⬜ **Character Sheet Forge** _(stubbed)_: Multi-system character sheet manager supporting D&D 5e, Pathfinder 1e/2e, OSR, and custom rulesets. Tracks stats, inventory, spells, conditions, and resource pools with live derived-stat recalculation and per-system rule validation.

- ⬜ **Dice Roller With Macros** _(stubbed)_: Parser for full dice notation (advantage, exploding, keep-highest, fudge, success-counting pools) with named macros, roll history, and probability previews. Roll templates bind to character sheet fields for one-click attacks, saves, and skill checks.

- ⬜ **Encounter Builder** _(stubbed)_: Assembles combat encounters from the monster compendium with CR/XP budgeting, party-level balance heuristics, terrain tags, and difficulty estimation across multiple systems. Outputs initiative-ready stat blocks and loot rolls.

- ⬜ **Initiative Tracker** _(stubbed)_: Round/turn tracker with HP, conditions, concentration, durations, lair actions, and legendary action slots. Supports hidden GM rolls, surprise rounds, delayed/readied actions, and reaction tagging; auto-decrements timed effects on turn transitions.

- ⬜ **World Bible** _(stubbed)_: Linked knowledge graph of places, factions, NPCs, deities, items, and events with bidirectional references, timeline pinning, and tag-based filtering. AI assists with consistency checks (contradictions, dangling references) and on-demand expansion of stub entries.

- ⬜ **Random Table Generator** _(stubbed)_: Authoring and rolling environment for nested weighted tables with recursion, conditional branches, and inline dice expressions. Imports common formats (Perchance, RollTables, Foundry) and exposes tables as macros to other modules.

- ⬜ **Hex And Grid Mapper** _(stubbed)_: Virtual tabletop canvas with square, hex (pointy/flat), and gridless modes; supports tokens, fog of war, dynamic line-of-sight, elevation layers, and measured templates. Exports battle maps to print-tiled PDFs or image atlases.

- ⬜ **Regional Map Sculptor** _(stubbed)_: Procedural overworld generator producing biomes, rivers, coastlines, settlements, and trade routes with editable splines and tectonic/climate simulation. Outputs labeled atlas pages and player-facing fog-redacted variants.

- ⬜ **Campaign Log** _(stubbed)_: Session-keyed journal capturing recap notes, quests, loot, XP awards, and player decisions with auto-cross-linking to World Bible entities. AI drafts session summaries from raw notes and produces player-facing recaps with spoilers redacted.

- ⬜ **GM Screen Reference** _(stubbed)_: Configurable reference panel of conditions, actions-in-combat, cover/cone rules, travel pace, downtime activities, and ruling cheatsheets per system. User-pinnable cards and house-rule overlays sit beside official text.

- ⬜ **Monster Compendium** _(stubbed)_: Searchable bestiary with full stat blocks, CR/level, tags, environments, lore, and reaction/legendary actions across SRD-licensed sources. Includes a stat-block editor with CR estimator and homebrew validation.

- ⬜ **Spell And Item Library** _(stubbed)_: Filterable catalog of spells, magic items, mundane gear, and crafting recipes with school/level/rarity/source filters and per-system attunement and slot rules. Items link to character sheets and loot tables for one-click assignment.

- ⬜ **NPC Generator** _(stubbed)_: Produces NPCs with names, appearances, mannerisms, secrets, voice notes, and full stat blocks tuned to a culture/region defined in the World Bible. AI generates dialogue snippets and motive arcs grounded in the linked faction and location context.

- ⬜ **Calendar And Timeline** _(stubbed)_: Custom-calendar engine supporting arbitrary months, weeks, moons, planar conjunctions, and holidays, with event scheduling and travel-time math. Renders chronological and parallel-track timelines for campaigns, factions, and PC backstories.

- ⬜ **Language And Script Lab** _(stubbed)_: Conlang workbench with phoneme inventories, sound-change rules, lexicon, romanization, and glyph/script designers for in-world writing systems. Renders inscriptions and produces player-handout images with optional cipher-reveal layers.

- ⬜ **Handout And Prop Studio** _(stubbed)_: Layout tool for letters, maps-in-the-fiction, ransom notes, wanted posters, and tavern menus with parchment/ink presets and aging effects. Exports print-ready PDFs and player-screen variants; AI drafts in-character prose from bullet briefs.

- ⬜ **Voice And Ambience Mixer** _(stubbed)_: Local audio board for combat tracks, biome ambience, weather layers, and stinger cues with crossfade, ducking, and scene presets that follow the active map or encounter. Drives playback from initiative events (round start, low HP, boss phase).

- ⬜ **Rules Oracle** _(stubbed)_: Retrieval-augmented Q&A over loaded rulebooks, errata, and house rules with citation-anchored answers and a ruling-history log. Answers route through the active system profile so cross-edition rules do not leak into responses.


#### Game playing assistants

- ⬜ **UCI engine driver** _(stubbed)_: Spawns and communicates with UCI-protocol chess engines (Stockfish, Lc0, Berserk), exposing position evaluation, multi-PV search, and ponder mode. Streams depth/score/PV lines back as structured data for charting and annotation.

- ⬜ **PGN repertoire workbench** _(stubbed)_: Parses, merges, and queries PGN libraries into a unified opening tree with transposition detection, move-frequency stats, and ECO classification. Supports tagging variations as 'main line', 'sideline', or 'avoid' and exports Lichess-study-compatible PGN.

- ⬜ **Lichess/chess.com explorer** _(stubbed)_: Queries the Lichess opening explorer and master games database for any FEN, returning move frequency, win percentages by rating band, and example games. Caches results locally for offline repertoire work.

- ⬜ **Syzygy tablebase probe** _(stubbed)_: Resolves 7-piece endgame positions against local or remote Syzygy tablebases, returning WDL and DTZ values plus the principal variation. Used to verify engine evaluations in technical endgames.

- ⬜ **KataGo analysis bridge** _(stubbed)_: Drives a local KataGo binary via the analysis engine JSON protocol, requesting policy/value/ownership maps and score-lead estimates for SGF positions. Surfaces winrate graphs, blunder detection, and move suggestions with visit counts.

- ⬜ **SGF library indexer** _(stubbed)_: Ingests SGF collections (pro games, OGS/KGS archives), normalizes metadata, and builds a searchable index by player, rank, opening pattern, and board-region motif. Exposes joseki and fuseki frequency tables.

- ⬜ **Poker equity solver** _(stubbed)_: Computes hand-vs-hand and range-vs-range equity via exhaustive enumeration or Monte Carlo, supporting Hold'em, Omaha, and Stud variants. Returns equity distribution, board-texture breakdowns, and combo-by-combo grids.

- ⬜ **GTO range explorer** _(stubbed)_: Loads precomputed solver outputs (PioSolver, GTO+, Wizard) and lets the user query node-by-node strategy frequencies, EV by combo, and exploitative deviations against custom opponent ranges. AI-integrated explanation translates solver output into plain-English heuristics.

- ⬜ **Hand history parser** _(stubbed)_: Parses PokerStars, GGPoker, and Hand2Note hand-history formats into a normalized schema with per-street action, pot odds, and showdown equities. Feeds leak-finder queries (e.g., '3-bet fold from BB vs CO').

- ⬜ **TCG deck builder** _(stubbed)_: Builds Magic, Hearthstone, and Pokemon TCG decklists with mana-curve analysis, color-source counts, legality checking per format, and sideboard guides. Imports/exports MTGA, MTGO, and Cockatrice formats and resolves cards against a local Scryfall mirror.

- ⬜ **Meta snapshot scraper** _(stubbed)_: Pulls tournament results and tier lists from MTGTop8, Untapped.gg, 17lands, and Smogon usage stats, normalizing them into a single 'archetype share + winrate' table. Tracks deltas week-over-week to flag emerging archetypes.

- ⬜ **Frame data lookup** _(stubbed)_: Queries fighting-game frame-data sets (Street Fighter, Tekken, Guilty Gear, Smash Ultimate) by character and move, returning startup, active, recovery, on-block, and cancel-routes. Supports punish-window queries ('what punishes Ryu cr.MK on block?').

- ⬜ **Emulator RAM watcher** _(stubbed)_: Attaches to RetroArch, BizHawk, or Dolphin via their memory APIs, exposing live RAM watches, save-state diffing, and Lua hook injection. Used for randomizer routing, RNG manipulation research, and TAS scripting.

- ⬜ **Speedrun split timer** _(stubbed)_: LiveSplit-compatible timer with autosplitters, gold-split tracking, sum-of-best, and run history. Imports splits.io files and exports run attempts as JSON for pacing analysis.

- ⬜ **Walkthrough/wiki indexer** _(stubbed)_: Builds a local full-text index over GameFAQs guides, Fandom wikis, speedrun.com resources, and StrategyWiki pages for a given game. Returns ranked passages with citations and supports follow-up questions grounded in the indexed corpus.

- ⬜ **Replay annotator** _(stubbed)_: Ingests game replays (chess PGN, StarCraft .rep, Dota .dem, fighting-game match video) and produces a turn/tick-aligned annotation track with engine evaluation, key-moment markers, and AI-generated commentary. Exports to a timeline UI or video overlay.

- ⬜ **Randomizer seed analyzer** _(stubbed)_: Loads randomizer logic files (OoT, ALttP, Pokemon) for a given seed and computes item placement, required tricks, and shortest-path routes through dungeons. Surfaces logic-locked items and beatability checks.

- ⬜ **ICCF/NAG annotator** _(stubbed)_: Normalizes chess move annotations (!, ?, !?, $14, $44) and Go/poker equivalents across PGN, SGF, and hand-history formats, mapping them to a unified severity/quality scale. Feeds study-card generation for spaced-repetition review of blunders and missed lines.


#### Esports, speedrunning, competitive game analytics

- ⬜ **Replay Scrubber** _(stubbed)_: Frame-accurate replay viewer with per-frame scrubbing, variable playback speed, A/B loop markers, and side-by-side run comparison. Supports common replay containers (Quake DEM, Source DEM, StarCraft .rep, Dolphin/RetroArch states) plus raw video with OCR-derived timestamps.

- ⬜ **Input Overlay** _(stubbed)_: Live and recorded input-display overlay that renders controller/keyboard state per frame, with configurable skins for arcade sticks, gamepads, and KB+M. Reads from local capture devices, TAS movie files, or replay-extracted input streams.

- ⬜ **Splits Timer** _(stubbed)_: LiveSplit-compatible segmented timer with auto-splitting via memory hooks, image recognition, and load-removal. Stores splits, gold segments, and sum-of-best per route, and writes splits.io and Splits XML.

- ⬜ **Route Notebook** _(stubbed)_: Markdown notebook for route notes with embedded splits, video clips, and per-segment tricks; supports branching variants and conditional sections per RNG seed or category. AI summarizer drafts route changelogs by diffing two notebook versions.

- ⬜ **Leaderboard Scraper** _(stubbed)_: Polite scheduled scraper for Speedrun.com, Cyberscore, Twitch VODs, and game-specific ladders. Normalizes runs into a local SQLite leaderboard with verification metadata and ELO/percentile views.

- ⬜ **Draft Analyzer** _(stubbed)_: MOBA/TCG draft assistant that scores picks and bans against historical winrates, lane synergies, and tournament metas. Runs locally against vendored patch data and exposes a hot-seat captain mode for scrim drafting.

- ⬜ **Patch Diff** _(stubbed)_: Patch-note differ that ingests official changelogs, datamines, and balance spreadsheets, then produces a structured diff of stats, abilities, and items. AI explainer translates raw stat deltas into projected meta impact with cited prior patches.

- ⬜ **Meta Tracker** _(stubbed)_: Time-series tracker for character/deck/strategy pick and win rates across regions and ranks. Detects emergent strategies via change-point analysis and tags them to specific patches or tournament events.

- ⬜ **Scenario Builder** _(stubbed)_: Training-room scenario authoring with state snapshots, scripted bot behaviors, and pass/fail conditions for combos, kill confirms, or boss patterns. Exports to in-game training mode formats where supported and to a local repeatable harness otherwise.

- ⬜ **Frame Data Inspector** _(stubbed)_: Per-move frame-data browser with startup, active, recovery, hitbox/hurtbox visualizations, and on-block/on-hit advantage tables. Cross-references community frame-data sets with in-engine extracted values.

- ⬜ **Memory Watcher** _(stubbed)_: Lightweight RAM watcher and pointer-path resolver for emulators and PC processes, with named addresses, struct overlays, and tick-aligned logging. Used as the data source for auto-splitters and route validators.

- ⬜ **RNG Manipulator** _(stubbed)_: RNG state tracker and manipulation planner that models per-frame PRNG advancement for games with exposed seeds. Generates input sequences that hit target seeds and verifies them against captured runs.

- ⬜ **VOD Indexer** _(stubbed)_: Local index over Twitch, YouTube, and downloaded VODs with chat ingest, scene detection, and OCR of in-game HUDs. AI tagger marks deaths, objective takes, and notable plays for instant jump-to.

- ⬜ **Highlight Cutter** _(stubbed)_: Clip extractor that takes timestamps from the VOD indexer or chat spikes and produces vertical/horizontal cuts with captions, splits overlay, and watermark. Renders via local ffmpeg with deterministic encode profiles.

- ⬜ **Tournament Bracket** _(stubbed)_: Bracket builder and tracker covering single/double elimination, Swiss, round-robin, and GSL formats. Pulls live results from Challonge, Start.gg, and Liquipedia, and produces seedings, tiebreak reports, and prize splits.

- ⬜ **Scrim Scheduler** _(stubbed)_: Roster, availability, and scrim-block scheduler with timezone-aware slots, opponent contact log, and post-scrim review checklists. Links scrim sessions to VOD index entries and draft logs.

- ⬜ **Coach Telestrator** _(stubbed)_: Drawing and annotation layer over paused VOD frames or live spectator feeds, with arrow, zone, and timing-marker tools. Exports annotated frame sequences as study packets for players.

- ⬜ **Input Lab** _(stubbed)_: Latency and polling-rate measurement bench for controllers, monitors, and capture cards, using a USB microcontroller probe or audio-flash sync. Stores per-device profiles and warns when latency drifts.

- ⬜ **Hitbox Viewer** _(stubbed)_: Geometry viewer for collision, hit, and hurt volumes extracted from game files or live memory, rendered against the active animation. Supports fighting games, platformers, and FPS hitreg debugging.

- ⬜ **Practice Logger** _(stubbed)_: Structured log of practice sessions with drills attempted, success rates, and self-rated focus. Surfaces weak spots over time and suggests next-session drills based on miss patterns.


#### Hobbies: gardening, beekeeping, aquariums, pets

- ⬜ **Companion planting matrix** _(stubbed)_: Tabulates pairwise plant compatibility across allelopathy, shared pests, pollinator overlap, and nutrient guilds. Flags conflicts and suggests beneficial neighbors when laying out beds.

- ⬜ **Plant and pest ID** _(stubbed)_: Identifies plants, weeds, insects, and disease lesions from photos using a local visual model, returning species, confidence, and a short treatment note with source citation.

- ⬜ **Hive inspection log** _(stubbed)_: Structured capture of brood pattern, frame counts, queen sighting, temperament, stores, supers, and treatments. Rolls up colony health timelines per yard and per queen.

- ⬜ **Varroa load tracker** _(stubbed)_: Records alcohol-wash and sugar-roll counts, computes mites per 100 bees, and recommends a treatment window (oxalic dribble/vapor, formic, thymol) against a configurable threshold and brood status.

- ⬜ **Swarm and queen lineage tracker** _(stubbed)_: Logs queen marking color, source, mating date, supersedure, and pedigree across splits. Predicts swarm risk from frame congestion, drone brood, and seasonal cues.

- ⬜ **Pollinator phenology log** _(stubbed)_: Time-series record of bloom onset, pollinator species sightings (iNaturalist-style taxonomy), and forage availability. Correlates with hive scale weight gain and weather to surface dearth periods.

- ⬜ **Nitrogen cycle tracker** _(stubbed)_: Plots ammonia, nitrite, nitrate, and pH over the cycling period for a new tank. Flags stalls and estimates time-to-cycled from the curve shape using a fish-in or fishless protocol.

- ⬜ **Water chemistry calculator** _(stubbed)_: Converts and reconciles GH, KH, TDS, conductivity, salinity, and specific gravity. Computes RO remineralization recipes for soft-water biotopes, blackwater tannins, or reef salt mixing targets.

- ⬜ **Stocking and bioload calculator** _(stubbed)_: Estimates bioload from species, adult size, and activity profile against tank volume and filtration turnover. Warns on aggression, schooling minimums, and pH/temperature/hardness compatibility across the proposed list.

- ⬜ **Dosing calculator** _(stubbed)_: Computes dose volumes for Estimative Index (planted), two-part and Balling (reef), and trace supplementation given measured Ca, Alk, Mg, NO3, PO4, and Fe consumption rates.

- ⬜ **PAR and PPFD planner** _(stubbed)_: Models light intensity at depth from fixture spectrum, mounting height, and tank or terrarium geometry. Flags photoperiod and PUR mismatches for coral, planted, or UVB-dependent reptile keepers.

- ⬜ **Species care database** _(stubbed)_: Local-first species sheet for fish, coral, herp, amphibian, and invertebrate species with temperature, pH, humidity, UVB, diet, social structure, and source citations; queryable by parameter overlap.

- ⬜ **Terrarium climate logger** _(stubbed)_: Ingests temp, humidity, and UVB sensor streams (Solarmeter, BLE hygrometers, MQTT probes). Computes basking-to-cool-side gradient adherence, night drop compliance, and shed-cycle humidity bumps.

- ⬜ **Pet training log** _(stubbed)_: Records sessions per cue with criteria, reinforcement schedule, latency, and success rate. Tracks shaping progress and proofs against distraction levels in a Karen Pryor / clicker style ladder.

- ⬜ **Pet health and medication log** _(stubbed)_: Tracks weight, body-condition score, medications, vaccination dates, fecals, and vet notes. Generates weight-scaled dose reminders for spot-ons, oral meds, and parasite preventatives.

- ⬜ **Pet diet planner** _(stubbed)_: Calculates RER and MER caloric needs, macronutrient targets, and BARF or PMR balance from species, weight, and activity. Flags taurine, calcium-phosphorus ratio, and thiaminase issues in raw diets.


#### Home and DIY (plumbing, residential electrical, HVAC, landscaping)

- ⬜ **NEC wire and conduit sizer** _(stubbed)_: Sizes branch and feeder conductors per NEC 310 ampacity tables with derating for ambient temperature, raceway fill, and continuous-load factor; cross-checks voltage drop over the run and conduit fill per Chapter 9 tables 4 and 5.

- ⬜ **Service panel load calculator** _(stubbed)_: Runs NEC Article 220 standard and optional-method dwelling load calculations, including 240V appliance branches, EV charger, heat pump, and PV backfeed; sizes the service entrance conductor and main breaker.

- ⬜ **Motor and HVAC circuit sizer** _(stubbed)_: Computes MCA, MOCP, disconnect, and overload sizing for compressors and motors per NEC 430/440, using nameplate FLA/RLA/LRA and the appropriate code factors.

- ⬜ **Plumbing fixture-unit pipe sizer** _(stubbed)_: Sizes water supply mains and branches by WSFU per IPC/UPC Hunter curves, with simultaneous Hazen-Williams pressure-loss check across fittings and elevation; sizes DWV by DFU and slope.

- ⬜ **Gas line sizer** _(stubbed)_: Sizes natural gas and LP piping per NFPA 54 longest-length and branch-length methods, given appliance BTU loads, pipe material (black iron, CSST, PE), inlet pressure, and allowable drop.

- ⬜ **Manual J load calculator** _(stubbed)_: Performs ACCA Manual J8 room-by-room heating and cooling load calculations from envelope assemblies, orientation, infiltration (ACH50 to natural), and occupant/internal gains; outputs sensible, latent, and design CFM per room.

- ⬜ **Manual D duct designer** _(stubbed)_: Sizes supply and return trunks and branches using equal-friction or static-regain methods, given the available static pressure, fitting equivalent lengths, and target room CFM from the Manual J output.

- ⬜ **Psychrometric tool** _(stubbed)_: Plots state points and processes (mixing, sensible heat, latent, cooling coil bypass) on a psychrometric chart for a given elevation; solves for SHR, enthalpy, dew point, and required reheat.

- ⬜ **Refrigerant charge analyzer** _(stubbed)_: Computes superheat and subcooling targets for fixed-orifice and TXV systems from suction/liquid pressures and temperatures, with refrigerant property tables for R-410A, R-32, R-454B, and R-22; flags over/undercharge.

- ⬜ **Heat pump balance point planner** _(stubbed)_: Overlays equipment capacity curves against the building heat-loss line versus outdoor temperature, locating the thermal and economic balance points and sizing auxiliary strip heat or backup fuel.

- ⬜ **Envelope heat-loss stack** _(stubbed)_: Builds U-value assemblies layer by layer (framing fraction, cavity insulation, sheathing, cladding) with parallel-path and isothermal-planes methods; rolls up whole-building UA and design heat loss/gain.

- ⬜ **Blower-door and infiltration estimator** _(stubbed)_: Converts blower-door CFM50 to ACH50, ACHnat, and equivalent leakage area using the LBL model with shielding and stack-height coefficients; suggests target tightness for the climate zone.

- ⬜ **Energy retrofit payback ranker** _(stubbed)_: Ranks insulation, window, air-sealing, and equipment upgrades by simple payback and lifecycle NPV against local fuel prices and HDD/CDD data, with AI-integrated suggestion of bundles that hit a target HERS or utility rebate tier.

- ⬜ **Solar-path sun and shade overlay** _(stubbed)_: Renders the sun path for a given latitude and date range over a site plan or yard photo, computing hours of direct sun per zone with tree-canopy and structure masks; exports a shade map for plant siting and PV placement.

- ⬜ **Irrigation zone designer** _(stubbed)_: Lays out drip and rotor zones from a parcel polygon and water-pressure/flow inputs, grouping heads by precipitation rate and hydrozone; computes friction loss, valve sizing, and a controller schedule by ET and soil type.

- ⬜ **Planting plan and hardiness advisor** _(stubbed)_: Combines USDA hardiness zone, AHS heat zone, soil pH, and the shade overlay to suggest species lists with spacing, mature size, and bloom calendar; AI-integrated companion-planting and pest-pairing notes.

- ⬜ **Grading and drainage planner** _(stubbed)_: Computes runoff via the rational method and TR-55 from contour or LiDAR DEM input, sizes swales, French drains, and dry wells, and checks 2% slope-away from foundations and ADA walkway grades.

- ⬜ **Deck and fence material estimator** _(stubbed)_: From a deck or fence footprint and code inputs (joist span tables, footing depth, post spacing), generates a cut list of joists, beams, posts, decking, and fasteners with waste factor and a board-foot cost roll-up.

- ⬜ **Lumber cut optimizer** _(stubbed)_: Solves 1D bin-packing across a cut list against available stock lengths to minimize offcut waste and board count, accounting for kerf and grain-direction constraints.

- ⬜ **Paint and primer coverage calculator** _(stubbed)_: Estimates gallons of primer and topcoat from wall, ceiling, and trim surface areas net of openings, given the product spread rate, substrate porosity, and number of coats; outputs a buy list rounded to can sizes.

- ⬜ **Concrete and aggregate volume calculator** _(stubbed)_: Computes yards of concrete, rebar weight, and base aggregate for slabs, footings, piers, and stairs from dimensions and reinforcement spacing; adds waste factor and converts to bag count for small pours.

- ⬜ **Roof pitch and surface estimator** _(stubbed)_: Derives roof surface area, ridge/hip/valley lengths, and shingle/underlayment counts from footprint and pitch (or from a tilted photo with vanishing-point calibration); estimates ice-and-water shield and drip-edge runs.

- ⬜ **Code and permit reference lookup** _(stubbed)_: Searches adopted local amendments to IRC, IPC, IMC, NEC, and IECC by jurisdiction and year, surfacing the controlling clause for a given calculation; AI-integrated paraphrase plus the verbatim quoted section.


#### Hunting, fishing, trapping, wildlife tracking

- ⬜ **Solunar Tables** _(stubbed)_: Generates sunrise, sunset, moonrise, moonset, and twilight times for any coordinate, with major and minor feeding windows scored against barometric pressure and recent weather. Plots multi-day activity forecasts and exports ICS alarms for stand departure.

- ⬜ **Species Season Calendar** _(stubbed)_: Tracks legal open and close dates for game and fish species by state, zone, weapon, and method. Color-coded annual view with overlap detection for multi-species trips and reminder hooks for application periods.

- ⬜ **Regulation Lookup** _(stubbed)_: Searches current statutes, bag limits, slot limits, weapon restrictions, and county exceptions across US, Canadian, and selected international jurisdictions. AI parses the official regs PDF and cites the rule paragraph; manual overrides flagged when local ordinances diverge.

- ⬜ **Hunt and Catch Log** _(stubbed)_: Records each outing with GPS waypoint, species, weight, length, sex, weapon or tackle, conditions, and geotagged photos. Supports voice memos in the field, EXIF backfill, and CSV or GPX export for season summaries.

- ⬜ **Trail-Cam Image Manager** _(stubbed)_: Ingests SD card dumps and deduplicates burst sequences, then runs on-device vision to tag species, sex, antler points, and individual deer by tine pattern. Time-of-day heatmaps and travel-corridor inference per camera location.

- ⬜ **Fish ID** _(stubbed)_: Identifies freshwater and saltwater species from a phone photo using a local vision model, with confidence scores and lookalike disambiguation (e.g., spotted vs largemouth bass). Links each ID to applicable size and bag rules from Regulation Lookup.

- ⬜ **Lure and Bait Reference** _(stubbed)_: Cross-references species, water temperature, clarity, depth, structure, and season against a tackle library to recommend presentations. Includes retrieval cadence notes, knot pairing, and a personal confidence-bait ranking learned from your catch log.

- ⬜ **Taxidermy Notebook** _(stubbed)_: Captures skinning measurements, eye-set references, pose sketches, form part numbers, tan recipes, and mount progress photos per specimen. Supplier order ledger and drying-time scheduler with humidity log integration.

- ⬜ **Topo and Property Atlas** _(stubbed)_: Offline topo, satellite, and contour layers with public land, WMA, and parcel-line overlays sourced from county GIS. Marks stands, blinds, scrapes, food plots, boat ramps, and access trails with custom icon sets.

- ⬜ **Wind and Scent Planner** _(stubbed)_: Models prevailing and forecast wind at each stand against terrain-driven thermals to recommend morning vs evening sits. Visualizes scent cones over the atlas and flags entry and exit routes that stay downwind of bedding.

- ⬜ **Ballistics and Arrow Calculator** _(stubbed)_: Computes bullet drop, wind drift, energy, and point-blank range for cataloged loads, and arrow trajectory with FOC, spine, and broadhead drag for compound and traditional setups. Range-card export sized for stock and quiver labels.

- ⬜ **Track and Sign ID** _(stubbed)_: Identifies mammal and bird tracks, scat, rubs, scrapes, and feathers from photos with a measurement overlay for scale. Gait analysis suggests species, direction, and time-since with confidence rationale.

- ⬜ **Tide and Current Charts** _(stubbed)_: Provides tide height, slack, and current vectors for NOAA and international stations with bite-window heuristics by target species. Pairs lunar phase, water temperature, and wind to score coastal and estuary trips.

- ⬜ **Game Call Library** _(stubbed)_: Audio reference of elk bugles, turkey yelps, whitetail grunts, predator distress, and waterfowl calls with cadence diagrams and best-use season notes. Includes a sequence builder that practices call-and-response drills against playback.

- ⬜ **Field Dressing and Yield Planner** _(stubbed)_: Step-by-step diagrams for gutting, skinning, quartering, and butchering big game, small game, upland, waterfowl, and common fish. Estimates boneless yield by cut, generates a freezer label sheet, and tracks cooler ice budget for warm-weather hauls.

- ⬜ **Trap Line Manager** _(stubbed)_: Maps sets along a line with target species, lure, bait, anchor type, and last-check timestamp, and enforces statutory check intervals per jurisdiction. Pelt prep workflow tracks fleshing, drying, and grading per fur.


#### Outdoor recreation: camping, climbing, skiing, paragliding, geocaching

- ⬜ **Topo Route Viewer** _(stubbed)_: Renders crag and big-wall topos from local PDFs, JPEGs, or imported guidebook data with pitch overlays, bolt counts, and beta annotations. AI vision tags features (cruxes, belays, fixed gear) from uploaded photos and aligns them to the route line.

- ⬜ **GPX Track Workbench** _(stubbed)_: Loads, splits, merges, simplifies, and re-times GPX/TCX/FIT tracks with elevation smoothing and pause detection. Includes waypoint snapping, segment statistics, and export to common watch and GPS formats.

- ⬜ **Avalanche Forecast Overlay** _(stubbed)_: Pulls regional avalanche center bulletins (CAIC, UAC, Avalanche Canada, EAWS) and overlays danger rose, problem types, and elevation bands onto the map. Caches forecasts offline for backcountry trips.

- ⬜ **Slope Angle & Aspect Map** _(stubbed)_: Renders slope angle shading (27, 30, 35, 38, 45 degree bands) and aspect coloring from local DEM tiles. Highlights terrain traps, runout zones, and ATES classifications along a planned ski tour line.

- ⬜ **Climb Grade Converter** _(stubbed)_: Converts between YDS, French, UIAA, British trad (adjectival + technical), Australian, Hueco, Font, and ice/mixed (WI, M) scales. Handles route grades, boulder grades, ice grades, and aid grades with confidence bands where systems do not map cleanly.

- ⬜ **Knot Reference Library** _(stubbed)_: Interactive 3D knot reference covering climbing, sailing, rescue, and fishing knots with step-by-step tying animations, strength ratings, and use-case tags. Searchable by function (load-bearing, slip, bend, hitch) and by rope diameter.

- ⬜ **Gear List Manager** _(stubbed)_: Builds packing lists per activity template (alpine climb, ski tour, paragliding XC, multi-day backpack) with weight totals, base/consumable splits, and redundancy flags. Tracks owned inventory and suggests swaps to hit a target pack weight.

- ⬜ **Weather Window Planner** _(stubbed)_: Aggregates multi-model forecasts (GFS, ECMWF, HRRR, ICON, MeteoBlue) for a target coordinate and stacks precipitation, freezing level, wind aloft, and CAPE across the next 10 days. AI summarizes the best send/launch/tour windows with caveats.

- ⬜ **Paraglider Site Database** _(stubbed)_: Local database of launch and LZ sites with takeoff aspect, wind direction windows, hazards, regulations, and XC history. Cross-references current wind aloft, thermal index, and convergence forecasts to score flyability.

- ⬜ **Geocache Hunter** _(stubbed)_: Manages GC waypoints, hint decoding (ROT13), puzzle solver notes, and field log offline. Filters by terrain/difficulty, cache type, and date placed; syncs found logs when reconnected.

- ⬜ **Summit Log & Peak Bagger** _(stubbed)_: Tracks ascents against curated lists (14ers, Munros, Bulgers, Volcanic Seven, county highpoints, local 100-prominence) with date, partners, route, and conditions. Imports GPX summit waypoints and generates progress maps.

- ⬜ **Anchor & Force Calculator** _(stubbed)_: Computes load distribution on multi-piece anchors (cordelette, equalette, quad), vector forces on top belays, fall factors, and pulley/Z-drag mechanical advantage. Flags configurations exceeding gear or sling MBS.

- ⬜ **Avalanche Beacon Drill Simulator** _(stubbed)_: Practice mode for transceiver search patterns (signal, coarse, fine, pinpoint) with timed scenarios, multiple burials, and spike-strike strategies. Logs search times and probe-strike accuracy across sessions.

- ⬜ **Trip Journal & Photo Geotagger** _(stubbed)_: Bundles GPX tracks, photos, weather snapshots, and free-form notes into per-trip records with automatic photo geotagging from track timestamps. AI drafts trip reports in classic American Alpine Journal or guidebook prose styles.

- ⬜ **Wilderness Medicine Reference** _(stubbed)_: Offline quick reference for WFR-level assessments, drug dosing, fracture/dislocation reduction, environmental injuries (HACE, HAPE, hypothermia, lightning), and evacuation decision trees. Includes patient SOAP note templates exportable as PDF.


#### Sports analytics, fitness, nutrition, coaching

- ⬜ **Periodization scheduler** _(stubbed)_: Lays out macro, meso, and microcycles with block, linear, conjugate, or DUP templates; supports RPE-based and percent-of-1RM progressions, automatic deload weeks, and taper curves into a target date.

- ⬜ **1RM and load calculator** _(stubbed)_: Estimates one-rep max via Epley, Brzycki, Lombardi, and Lander; renders %1RM tables and velocity-based training zones from bar-speed input.

- ⬜ **Macro and calorie ledger** _(stubbed)_: Logs foods against USDA FoodData Central and Open Food Facts (barcode lookup), rolling up daily protein/carb/fat/fiber/micros against targets with weekly trend smoothing.

- ⬜ **Recipe builder** _(stubbed)_: Composes recipes from ingredient lists with per-serving nutrient roll-up, batch scaling, leftover-aware meal planning, and grocery list export.

- ⬜ **Gait and form video analyzer** _(stubbed)_: Runs MediaPipe/MoveNet pose estimation on uploaded video to extract joint angles, stride length, cadence, ground-contact time, and left/right asymmetry; renders side-by-side overlays against a reference clip.

- ⬜ **HRV analyzer** _(stubbed)_: Parses raw RR-interval files into time-domain (RMSSD, SDNN, pNN50) and frequency-domain (LF, HF, LF/HF) metrics, with rolling baseline and readiness scoring.

- ⬜ **Sleep dashboard** _(stubbed)_: Ingests stage data from Oura, Whoop, Apple Health, or Polar exports and plots hypnogram, sleep efficiency, latency, and SWS/REM ratios; includes a chronotype and jet-lag shifter.

- ⬜ **Training load PMC** _(stubbed)_: Computes TSS/TRIMP, chronic training load (CTL), acute load (ATL), and form (TSB) on the Banister impulse-response model, with acute:chronic workload ratio flags and ramp-rate warnings.

- ⬜ **Critical power and W-prime tracker** _(stubbed)_: Fits the CP/W' model from 3-minute all-out or multi-effort test data, then simulates W' balance during interval workouts to predict failure points.

- ⬜ **Race pacing planner** _(stubbed)_: Computes Daniels VDOT zones and Riegel equivalent times across distances; overlays target splits on a GPX course profile with grade-adjusted pace.

- ⬜ **Wearable file importer** _(stubbed)_: Parses .fit, .tcx, .gpx, and CSV exports from Garmin, Strava, Wahoo, Polar, Apple Health, and Whoop into a normalized activity/stream schema with deduplication.

- ⬜ **Play-by-play loader** _(stubbed)_: Pulls play-by-play and tracking data from nflfastR, pybaseball (Statcast), nba_api, hockey-reference, and StatsBomb open data into tidy long-format frames keyed by game/possession/event.

- ⬜ **Expected-value model fitter** _(stubbed)_: Trains xG (soccer), xT/EPV (basketball), wOBA/xwOBA (baseball), EPA/WP (football) models on shot or play features; outputs calibration plots, SHAP importances, and per-player residuals.

- ⬜ **Scouting notebook** _(stubbed)_: Bookmarks timestamped clips against a tag taxonomy (player, action, outcome) with hotkey tagging; AI-integrated summarization rolls per-player tendencies and weaknesses from accumulated notes.

- ⬜ **Rating system fitter** _(stubbed)_: Fits Elo, Glicko-2, TrueSkill, Massey, and Colley ratings on head-to-head results with margin-of-victory and home-field adjustment; exposes a prediction API for upcoming fixtures.

- ⬜ **Season Monte Carlo simulator** _(stubbed)_: Walks a remaining schedule N times from team strength priors to produce win-total distributions, playoff seeding odds, and championship probabilities with confidence bands.

- ⬜ **Fantasy draft optimizer** _(stubbed)_: Computes VBD/VORP from projections and replacement levels, scrapes ADP, and solves lineup construction under salary cap and positional constraints for DFS and season-long formats.

- ⬜ **Body composition tracker** _(stubbed)_: Logs weight, skinfolds, bioimpedance, and DEXA scans; estimates body fat via Navy, Jackson-Pollock, and Durnin-Womersley with Kalman-smoothed lean-mass trajectory.

- ⬜ **Drill and play diagrammer** _(stubbed)_: Edits SVG court, field, ice, or pitch diagrams with route trees, screens, and defensive coverages; animates plays frame-by-frame and exports to GIF or video for film sessions.

- ⬜ **Statcast pitch and shot renderer** _(stubbed)_: Plots pitch movement, release-point density, spin-axis tilt, and pitch-tunnel comparisons from Statcast or Trackman data; renders shot charts and expected-shooting heatmaps for basketball and hockey.


#### Sailing, boating, navigation

- ⬜ **ENC chart renderer** _(stubbed)_: Parses S-57 ENC cells and S-63 encrypted exchange sets, applies S-52 presentation library symbology with day/dusk/night palettes, and renders configurable safety contours, scamin filters, and CATZOC quality overlays. Supports M_QUAL accuracy diamonds and depth area shading at a chosen safety depth.

- ⬜ **Raster chart georeferencer** _(stubbed)_: Imports BSB/KAP, GeoTIFF, and scanned pilot chart images, fits ground control points to WGS-84, and emits a calibrated tile pyramid. AI-assisted GCP suggestion reads printed graticule ticks and place names when metadata is missing.

- ⬜ **Tidal harmonic predictor** _(stubbed)_: Computes tide height and slack/flood/ebb times from harmonic constituents (M2, S2, N2, K1, O1, etc.) using the standard 37- or 67-constituent set, with nodal corrections. Loads NOAA, UKHO TotalTide, and IHO TWL station files and can solve constituents from a logged water level series.

- ⬜ **Tidal stream atlas** _(stubbed)_: Overlays vector current fields from HF-radar, ROMS, and tidal-diamond data on the chart at a chosen time, with hour-by-hour scrubbing relative to a reference port. Computes set-and-drift along a planned leg and flags tidal gates that close within a chosen window.

- ⬜ **GRIB weather router** _(stubbed)_: Reads GRIB1/GRIB2 wind, gust, pressure, wave, and current fields and computes isochrone-optimal routes against a polar with configurable comfort, gust, and squall-line penalties. Supports ensemble runs across GFS, ECMWF, ICON, and Arpege and reports route robustness across members.

- ⬜ **Polar diagram workbench** _(stubbed)_: Edits, interpolates, and validates boat polars in ORC, PCX, and OpenCPN formats, fits VPP surfaces from logged true-wind/SOG/heel triples, and exports target boatspeed, target heel, and target VMG curves. Highlights gaps in the wind-angle/wind-speed coverage of the underlying log data.

- ⬜ **AIS decoder and viewer** _(stubbed)_: Decodes NMEA AIVDM/AIVDO sentences for message types 1-5, 18, 19, 21, 24, and 27, reconstructs vessel tracks, and computes CPA/TCPA against own ship. Filters by class, MMSI, navigation status, and bounding box, and tags targets with risk-of-collision flags per the configured CPA limit.

- ⬜ **NMEA 0183/2000 multiplexer** _(stubbed)_: Reads, validates, and replays NMEA 0183 sentences and NMEA 2000 PGNs (including 129025 position, 129026 COG/SOG, 129029 GNSS, 130306 wind, 128267 depth), with checksum and PGN-fastpacket reassembly. Bridges to SignalK delta streams and exposes a tap for instrument debugging.

- ⬜ **SignalK schema bridge** _(stubbed)_: Mounts a SignalK server endpoint, maps incoming deltas to and from local instrument feeds, and lets the workbench subscribe to or publish self/vessels paths. Validates against the SignalK JSON schema and surfaces unit and reference-frame mismatches.

- ⬜ **Anchor watch** _(stubbed)_: Sets a swing-circle alarm from anchor drop position, rode length, depth, and tide range, with separate alarms for drag, depth excursion, and wind shift beyond a threshold. Logs the swing pattern through the night and AI-suggests a likely drag event versus a normal tide-driven swing.

- ⬜ **Celestial sight reducer** _(stubbed)_: Reduces sun, star, moon, and planet sights using the NA almanac and the Bowditch/HO 229 intercept method, builds a fix from multiple LOPs with a least-squares cocked-hat solution, and propagates DR between sights. Handles index error, dip, refraction, parallax, and limb corrections.

- ⬜ **Magnetic model** _(stubbed)_: Evaluates WMM and IGRF coefficients for variation, inclination, and field strength at a given lat/lon/date, applies deviation from a per-heading swing card, and converts between true, magnetic, and compass headings. Fits a new deviation card from a logged compass swing.

- ⬜ **Route and passage planner** _(stubbed)_: Builds multi-leg routes with great-circle and rhumb-line segments, snaps to ENC safety contours and TSS lanes, and computes ETA bands from polar plus weather. Generates a printable passage plan with waypoints, leg bearings, tidal windows, port-of-refuge fallbacks, and a SOLAS-style berth-to-berth checklist.

- ⬜ **Sail trim assistant** _(stubbed)_: Compares live wind, heel, rudder angle, and boatspeed against polar targets and suggests sheet, traveller, halyard, backstay, and outhaul adjustments per sail. AI-integrated recommendation explains the dominant loss (twist, depth, angle of attack) and proposes one change at a time.

- ⬜ **Sail and rig inventory** _(stubbed)_: Tracks each sail with cloth, year, hours under load, wind-range envelope, and last inspection, and the rig with standing wire, terminals, and tension log. Flags overdue inspections and AI-summarises wear patterns from uploaded photos of leech, luff, and clew.

- ⬜ **Logbook and track recorder** _(stubbed)_: Records position, COG, SOG, heading, wind, baro, sea state, sail config, crew on watch, and freeform notes at a configurable cadence, with manual event entries. Exports GPX, CSV, and PDF voyage logs and emits a customs-ready crew and vessel declaration.

- ⬜ **COLREGs and lights advisor** _(stubbed)_: Identifies a target's likely vessel type and status from observed light arcs, day shapes, or sound signals, and states the applicable steering and sailing rules with stand-on/give-way assignment. References specific rule numbers (5, 7, 13-18, 19) and quotes the rule text.

- ⬜ **MOB and SAR pattern generator** _(stubbed)_: Drops a MOB datum, computes Williamson, Anderson, and Scharnow turn solutions for the current speed and rudder rate, and generates expanding-square, sector, and parallel-track search patterns sized from leeway and elapsed-time uncertainty. Outputs waypoints ready to push to the autopilot.

- ⬜ **Fuel, range, and provisioning calculator** _(stubbed)_: Computes range and endurance from tank capacity, engine fuel curve, hotel load, and current/wind drag, with a reserve policy. Sizes water, food, gas, and battery provisioning per crew per day and warns when a planned passage exceeds any single resource.

- ⬜ **Engine and systems maintenance log** _(stubbed)_: Tracks engine hours, impeller, oil, filter, anode, and rig service intervals against manufacturer schedules, and records observed RPM, EGT, oil pressure, and alternator output curves. AI-flags drift in those curves that precedes a known failure mode.

- ⬜ **Pilot book ingestor** _(stubbed)_: Ingests cruising guides, harbour pilot pages, and notices to mariners as PDF, HTML, or scans, extracts anchorages, approaches, hazards, contacts, and VHF channels, and links each entry to a chart position. Cross-references with current NtMs to flag stale information.


#### Survival, emergency preparedness, off-grid

- ⬜ **Water filter sizing planner** _(stubbed)_: Computes required filter throughput and replacement cadence from household size, source-water contaminant profile, and target log-reduction values for bacteria, protozoa, and viruses. Compares ceramic, hollow-fiber, carbon block, and RO elements against the demand curve.

- ⬜ **Water treatment dose calculator** _(stubbed)_: Calculates chlorine, iodine, and UV doses for batch disinfection given volume, turbidity, and water temperature, and adjusts boil times for altitude. Includes SODIS exposure tables and pool-shock dilution for emergency potable conversion.

- ⬜ **Rainwater catchment sizer** _(stubbed)_: Sizes roof catchment area, first-flush diverter, and storage tank volume from regional precipitation data and projected demand. Models multi-month dry-period reserve and overflow routing for swales or cisterns.

- ⬜ **Off-grid PV and battery sizer** _(stubbed)_: Sizes solar array, charge controller, battery bank, and inverter from a daily load profile, site insolation, panel tilt, and battery chemistry (lead-acid, LiFePO4, NMC). Reports autonomy days, depth-of-discharge headroom, and amortized cost per kWh over cycle life.

- ⬜ **Electrical load profile builder** _(stubbed)_: Builds a 24-hour load curve by itemizing appliances, run-time duty cycles, startup surge factors, and seasonal variation. Exports daily watt-hour totals and peak demand for use by the PV sizer or a generator runtime calculator.

- ⬜ **Food storage rotation tracker** _(stubbed)_: Tracks pantry inventory with purchase date, best-by, and rehydrated calorie/macro yield per item, and generates a FIFO rotation schedule. Flags items approaching expiry and suggests substitutions that keep the stored ration on its macro target.

- ⬜ **Ration and caloric needs calculator** _(stubbed)_: Computes daily caloric and macronutrient needs from body mass, activity tier, ambient temperature, and altitude, then assembles a ration plan from on-hand storage items. Outputs grams-per-day per item and total pack weight for bug-out or sheltered scenarios.

- ⬜ **Garden and permaculture planner** _(stubbed)_: Lays out beds, succession plantings, and companion groupings against local frost dates and hardiness zone, with caloric-yield-per-square-meter rankings for staple crops. Schedules seed-starting, transplant, and harvest windows and tracks seed-bank viability.

- ⬜ **Foraging field guide** _(stubbed)_: Regional, season-aware reference for wild edibles with photographs, key ID features, look-alike warnings, and preparation notes. AI-assisted photo identification scores candidates and flags toxic confusions before any positive ID is returned.

- ⬜ **Mushroom ID helper** _(stubbed)_: Photo-based mushroom identification with conservative scoring that hard-flags deadly look-alikes (Amanita, Galerina, Cortinarius) and refuses positive ID where spore print or microscopy is required. Cross-references regional fruiting calendars and substrate.

- ⬜ **First-aid decision tree** _(stubbed)_: Triage and treatment walkthroughs for trauma, hemorrhage, hypothermia, hyperthermia, anaphylaxis, snakebite, and burns, with tourniquet timing, TXA window, and improvised splint guidance. Adapts to the kit on hand and projected time-to-evac.

- ⬜ **Field pharmacy reference** _(stubbed)_: Drug reference covering shelf life beyond labeled expiry, vet and aquarium-grade substitutions with caveats, common antibiotic regimens by indication, and interaction warnings. Includes a dosing calculator for pediatric and weight-based adjustment.

- ⬜ **Ham antenna designer** _(stubbed)_: Designs dipole, end-fed half-wave, vertical, loop, and NVIS antennas with element lengths, feed impedance, height-above-ground modeling, and SWR projection across the band. Outputs cut lengths in copper or steel wire for field construction.

- ⬜ **HF propagation forecaster** _(stubbed)_: Predicts band openings and maximum usable frequency for a given path using current solar flux, K-index, and ionospheric models. Recommends band and mode windows for NVIS, regional, and DX work over the next 24 hours.

- ⬜ **Radio link planner** _(stubbed)_: Plots line-of-sight and Fresnel-zone clearance across terrain between two coordinates for VHF/UHF simplex, repeater, or Meshtastic/LoRa links. Estimates path loss, fade margin, and antenna height needed for a target link budget.

- ⬜ **Celestial navigation aid** _(stubbed)_: Identifies stars and planets, reduces sextant sights to a line of position, and derives latitude from Polaris altitude or noon-sun observations. Includes a pre-computed almanac for sun, moon, and navigational stars at any date and location.

- ⬜ **Land navigation toolkit** _(stubbed)_: Converts between lat/lon, UTM, and MGRS, applies local magnetic declination, computes pace count over slope, and walks dead-reckoning legs with cumulative error estimates. Reads topo contours for slope angle, viewshed, and water-flow direction.

- ⬜ **Knot and lashing library** _(stubbed)_: Cataloged knots and lashings with use cases, breaking-strength derating versus rope material and diameter, and step-by-step diagrams. Recommends an appropriate knot from the load type (life-safety, hauling, hitching, joining, securing).


#### Detailed genealogy and family history

- ⬜ **GEDCOM Tree Editor** _(stubbed)_: Round-trip GEDCOM 5.5.1 / 7.0 editor with individual, family, source, and event records. Supports custom tags, multimedia links, and diff/merge between two trees on shared INDI keys.

- ⬜ **Autosomal Match Cluster Board** _(stubbed)_: Imports raw match lists from AncestryDNA, 23andMe, MyHeritage, and GEDmatch, then runs Leeds-method clustering plus configurable shared-cM thresholds. Renders the resulting groups as a colored matrix with drag-to-merge cluster assignment.

- ⬜ **Segment Triangulator** _(stubbed)_: Loads chromosome browser CSVs and finds three-way triangulated segments above a user-set cM floor. Annotates each segment with candidate most-recent-common-ancestor couples pulled from linked trees.

- ⬜ **Y-DNA and mtDNA Haplogroup Explorer** _(stubbed)_: Browses ISOGG / YFull haplotrees offline, places STR and SNP results on the tree, and estimates TMRCA between kits using infinite-alleles and Bayesian (BATWING-style) models.

- ⬜ **Oral History Studio** _(stubbed)_: Multi-track recorder with local Whisper transcription, speaker diarization, and timestamped name/place tagging. Exports interview transcripts as linked sources attached to the relevant INDI records in the tree.

- ⬜ **Paleography OCR** _(stubbed)_: Handwriting recognition tuned for parish registers, ship manifests, and probate scripts (Secretary hand, Kurrent, Sutterlin, Spencerian). Outputs editable text with per-token confidence so a researcher can correct ambiguous letters in place.

- ⬜ **Census Record Harvester** _(stubbed)_: Parses US, UK, Canadian, and Irish census images or HTML into structured household rows. Auto-links suggested INDI matches in the open tree using fuzzy name, age, and birthplace scoring.

- ⬜ **Immigration and Naturalization Search** _(stubbed)_: Cross-queries Ellis Island, Castle Garden, Hamburg, Bremen (reconstructed), and Canadian Border Crossings indices. Reconciles passenger manifests with later naturalization petitions and declarations of intent.

- ⬜ **Military Service Reconstructor** _(stubbed)_: Pulls pension files, muster rolls, draft cards (WWI / WWII), and unit histories. Plots the soldier's movements on a campaign map and flags pension widows for spouse confirmation.

- ⬜ **Probate and Land Parser** _(stubbed)_: Extracts heirs, executors, witnesses, bequests, and metes-and-bounds descriptions from wills and deeds. Produces an inferred kinship graph and a polygon plat for land records that include surveyor calls.

- ⬜ **Surname Etymology and Patronymic Resolver** _(stubbed)_: Looks up surname origins across British Isles, Scandinavian, Iberian, Slavic, Arabic, and East Asian traditions. Handles patronymic chain resolution (Petersen -> Peter's son) and known anglicization / Ellis Island mis-transcriptions.

- ⬜ **Historical Jurisdiction Atlas** _(stubbed)_: Time-sliced map overlay of county, parish, diocese, and country boundaries from roughly 1500 to present. Given an event date and modern place, returns the correct jurisdiction that would hold the original record.

- ⬜ **Endogamy and Pedigree Collapse Detector** _(stubbed)_: Scans the tree for shared ancestors within a configurable generational window and computes inbreeding coefficients. Warns when DNA cM predictions should be widened due to endogamous populations (Ashkenazi, Acadian, isolated island, colonial Virginia).

- ⬜ **Newspaper and Obituary Miner** _(stubbed)_: Full-text search across Chronicling America, BNA, Trove, and user-supplied OCR'd local papers. Uses an LLM pass to pull names, relationships, residences, and dates out of obituaries and social columns into structured event records.

- ⬜ **Cemetery and Burial Mapper** _(stubbed)_: Imports BillionGraves / Find a Grave exports plus user GPS walks, and stores headstone photos with plot coordinates. Renders cemetery maps showing family clustering and unassigned adjacent plots worth investigating.

- ⬜ **Source Citation Manager** _(stubbed)_: Generates citations in Evidence Explained, NGSQ, and Chicago formats with templates per record type (census, vital, church, probate). Tracks evidence quality (original / derivative, primary / secondary) and surfaces conflicting source claims on the same fact.

- ⬜ **Brick Wall Research Planner** _(stubbed)_: Given a stuck ancestor, the LLM proposes prioritized next steps (specific microfilm rolls, FHL catalog numbers, FAN-club candidates, DNA targets) with rationale. Tracks completed and pending tasks per research question in a kanban-style log.

- ⬜ **Photo Restoration and Face Matcher** _(stubbed)_: Local diffusion-based deblur, colorize, and tear-fill for daguerreotypes, tintypes, and early prints. Runs face embeddings across the photo library to suggest the same individual across decades, and can age-progress or age-regress between known reference shots.

- ⬜ **Migration and Timeline Visualizer** _(stubbed)_: Animated timeline of an individual's or family's known events on a base map, with optional historical context layers (epidemics, famines, land openings, conscriptions). Useful for spotting unexplained gaps that imply a missing record.


#### Sleep science, wellness, mental health

- ⬜ **Sleep Stage Logger** _(stubbed)_: Records sleep periods with manual stage entry (N1/N2/N3/REM/Wake) or timestamped wake-after-sleep-onset events. Computes total sleep time, sleep efficiency, and stage percentages from the log.

- ⬜ **Wearable Data Importer** _(stubbed)_: Ingests CSV, FIT, JSON, and Apple Health XML exports from Oura, Whoop, Fitbit, Garmin, and Withings. Normalizes nightly summaries and per-epoch stage arrays into a single local schema.

- ⬜ **Hypnogram Viewer** _(stubbed)_: Renders the night as a stepped stage plot over time with overlays for HR, HRV, SpO2, movement, and ambient temperature. Supports zoom, multi-night stacking, and side-by-side comparison.

- ⬜ **Dream Journal with Motif Tagger** _(stubbed)_: Free-text journal with timestamped entries, voice-note transcription, and lucidity flags. An on-device language model extracts recurring motifs, characters, settings, and emotional valence into a tag index.

- ⬜ **Dream Sign Catalog** _(stubbed)_: Catalogs personal dream signs (inner, action, form, context) with frequency and co-occurrence statistics across the journal. Surfaces the top-N signs to rehearse for prospective lucidity training.

- ⬜ **Lucid Technique Tracker** _(stubbed)_: Logs attempts and outcomes for MILD, WBTB, WILD, SSILD, FILD, and DEILD with protocol parameters (WBTB delay, mantra text, anchor duration). Computes per-technique success rate, dwell time before lucidity, and confounds.

- ⬜ **Reality Check Scheduler** _(stubbed)_: Fires configurable randomized prompts during waking hours for digit-glance, hand-check, light-switch, and breath-through-nose tests. Logs hit, miss, and false-positive outcomes for habit-strength tracking.

- ⬜ **Sleep Cycle Calculator** _(stubbed)_: Computes candidate bedtimes or wake times in 90-minute (or user-tuned) ultradian increments, factoring sleep-onset latency. Outputs alarm targets aligned to predicted REM exits.

- ⬜ **WBTB Alarm Composer** _(stubbed)_: Builds multi-stage alarm sequences for wake-back-to-bed protocols: primary wake, awake-window length, mantra rehearsal cue, and re-entry alarm. Adapts to last night's onset latency and the running cycle estimate.

- ⬜ **Chronotype Quiz** _(stubbed)_: Administers the Morningness-Eveningness Questionnaire (MEQ), Munich Chronotype Questionnaire (MCTQ), and AutoMEQ. Reports chronotype score, mid-sleep on free days corrected for sleep debt (MSFsc), and social jetlag.

- ⬜ **Circadian Phase Estimator** _(stubbed)_: Estimates dim-light melatonin onset (DLMO) and core body temperature minimum from logged sleep midpoints, light exposure, and (optionally) HRV troughs. Produces a phase-advance or phase-delay recommendation window.

- ⬜ **Light Exposure Logger** _(stubbed)_: Tracks bright-light dose (lux times minutes) and red-shifted evening exposure, either from manual entries or phone ambient-light sensor sampling. Flags deviations from a target circadian light schedule.

- ⬜ **Sleep Debt Ledger** _(stubbed)_: Maintains a rolling acute and chronic sleep debt against a configurable need (default 7.5 to 8.5 h), with weighted recency. Projects recovery sleep required to return to baseline.

- ⬜ **Nap Planner** _(stubbed)_: Schedules power (20 min), caffeine (20 min after 100 to 200 mg), and long (90 min) naps relative to current debt, circadian phase, and next obligation. Predicts sleep inertia severity at wake.

- ⬜ **Jet Lag Schedule Builder** _(stubbed)_: Generates a per-day light, darkness, melatonin, and meal schedule for a planned timezone shift using direction-of-travel and chronotype. Exports to ICS calendar.

- ⬜ **Snore and Apnea Audio Analyzer** _(stubbed)_: Records overnight audio locally and runs on-device detection for snoring, apnea-suggestive silences, and arousals. Outputs an event timeline aligned with the hypnogram and a rough AHI proxy with clear non-diagnostic disclaimers.

- ⬜ **Caffeine and Stimulant Tracker** _(stubbed)_: Logs caffeine, theanine, nicotine, and other stimulant doses with timing and computes residual concentration curves using configurable half-lives. Flags doses likely to suppress adenosine load at target bedtime.

- ⬜ **Supplement and Pharmacology Log** _(stubbed)_: Tracks melatonin, magnesium, glycine, apigenin, and (gated, with safety notes) galantamine or choline use for REM augmentation. Correlates dose and timing with subsequent sleep architecture and lucidity outcomes.

- ⬜ **Sleep Environment Audit** _(stubbed)_: Checklist plus optional sensor capture for bedroom temperature, humidity, CO2, noise floor, and lux at eye level. Scores the environment against evidence-based targets and recommends single-variable fixes.

- ⬜ **Dream Incubation Planner** _(stubbed)_: Composes pre-sleep intention scripts targeting a chosen theme, person, or problem, with a brief rehearsal protocol and post-wake recall prompt. Uses an on-device model to draft and refine the intention text.

- ⬜ **Sleep Paralysis and False Awakening Journal** _(stubbed)_: Specialized log for SP episodes, hypnagogic and hypnopompic imagery, and false awakenings, with severity, duration, and induction-attempt fields. Cross-references entries with the main dream journal and stage data.

- ⬜ **Polyphasic Schedule Designer** _(stubbed)_: Models Everyman, Uberman, Dymaxion, Biphasic, and custom segmented schedules with core and nap placement on a 24-hour circadian curve. Simulates expected REM and SWS allocation and adaptation-period sleep deprivation.

- ⬜ **Mood log with valence-arousal grid** _(stubbed)_: Logs mood as a two-axis point (valence x arousal) plus tags, notes, and contextual fields (sleep, caffeine, social). Renders Daylio-style streaks and weekly heatmaps over the local store.

- ⬜ **CBT thought record** _(stubbed)_: Structured template with situation, automatic thought, evidence-for/against, cognitive-distortion tags (Burns' list), and reframe. AI-integrated distortion classifier suggests likely distortions from the thought text.

- ⬜ **Consensus sleep diary** _(stubbed)_: Implements the Carney et al. Consensus Sleep Diary fields: bedtime, sleep latency, WASO, final awakening, time out of bed. Computes sleep efficiency, total sleep time, and rolling 7- and 14-day averages.

- ⬜ **Meditation timer with interval bells** _(stubbed)_: Configurable session timer with warm-up, primary interval, and cool-down phases, plus repeating interval bells (Tibetan bowl, woodblock, or custom WAV). Logs session length, technique tag, and post-session rating.

- ⬜ **Breathing pacer** _(stubbed)_: Animated pacer for box breathing, 4-7-8, coherent (5.5 bpm) and physiological-sigh patterns, with optional inhale/exhale tones. Per-pattern timing is editable and sessions are logged with HRV deltas if a sensor stream is attached.

- ⬜ **Gratitude and habit tracker** _(stubbed)_: Daily gratitude entries plus a Loop-style habit grid with streaks, miss-once-no-twice rule, and target frequencies (e.g. 5x/week). Exports to CSV and renders calendar heatmaps.

- ⬜ **Encrypted therapy session notes** _(stubbed)_: Age- or libsodium-encrypted notes in SOAP or DAP format, with client/session metadata stored separately from body text. Local-only by default; supports offline passphrase-derived keys and a panic-lock shortcut.

- ⬜ **Validated-instrument battery** _(stubbed)_: Self-administered PHQ-9, GAD-7, PSQI, ISI, AUDIT-C, K10, and PCL-5 with correct scoring, severity bands, and longitudinal plots. Flags clinical thresholds and surfaces item-level changes between administrations.

- ⬜ **Stanley-Brown safety plan** _(stubbed)_: Guided builder for the six-step safety plan: warning signs, internal coping, social distractions, support contacts, professionals, and means restriction. Prints a wallet-card PDF and an on-device quick-access view.

- ⬜ **Chronotype and circadian estimator** _(stubbed)_: Administers the MEQ and MCTQ, estimates chronotype and social jetlag, and approximates DLMO from sleep midpoint. Outputs a recommended light, melatonin, and meal-timing window.

- ⬜ **Sleep restriction scheduler** _(stubbed)_: CBT-I sleep restriction therapy calendar: sets prescribed time-in-bed from diary sleep efficiency, advances or holds windows by the 85/90% rule, and tracks adherence and TST drift week over week.

- ⬜ **Behavioral activation planner** _(stubbed)_: Schedules mastery and pleasure activities on a weekly grid, with pre/post mood and engagement ratings (0-10). Suggests activities from the user's own historical high-rated entries (AI-integrated ranking).

- ⬜ **Exposure hierarchy builder** _(stubbed)_: Constructs a SUDS-rated fear hierarchy with steps, planned exposures, expectancy vs outcome ratings, and inhibitory-learning prompts. Logs trial-by-trial SUDS curves to visualize habituation.

- ⬜ **Reframing assistant** _(stubbed)_: Takes a hot thought plus distortion tags and drafts balanced alternative thoughts grounded in the user's stated evidence. AI-integrated, with deterministic citation of which evidence lines justify each reframe.

- ⬜ **Dream and imagery journal** _(stubbed)_: Free-text dream entries with lucidity flag, recurring-symbol tagger, and emotion vector. AI-integrated theme clustering over the corpus surfaces recurring motifs without exposing entries off-device.

- ⬜ **HRV and biofeedback viewer** _(stubbed)_: Ingests RR-interval CSV from Polar H10, EliteHRV, or Kubios exports and computes RMSSD, SDNN, LF/HF, and the resonant breathing frequency. Live mode renders a coherence score during paced-breathing sessions.

- ⬜ **Bilateral stimulation tool** _(stubbed)_: EMDR-style alternating audio (panned L/R) and visual (moving dot) stimulus with adjustable speed, set length, and set count. Logs SUDS and VOC ratings between sets per the standard protocol.

- ⬜ **Soundscape and binaural generator** _(stubbed)_: Generates pink/brown/violet noise, rain, and binaural or isochronic tones at chosen carrier and beat frequencies (delta through gamma). Sessions are mixable, loopable, and saveable as preset stacks.

- ⬜ **Symptom and medication tracker** _(stubbed)_: Bearable-style daily log of symptoms, medications (with doses and times), supplements, and factors (sleep, food, cycle). Computes lagged correlations between factors and symptoms over a configurable window.

- ⬜ **Body scan and yoga nidra player** _(stubbed)_: Plays scripted body-scan and iRest yoga-nidra sequences with adjustable pacing, region order, and background bell. Scripts are plain-text editable; TTS rendering uses an on-device voice for full local operation.


#### Radio-control modeling (planes, cars, trucks, boats, drones)

- ⬜ **RC transmitter and receiver pairing reference** _(stubbed)_: covers OpenTX/EdgeTX/Spektrum/Futaba/FrSky/ELRS protocols, model setup, dual rates, expo, mixes, telemetry.

- ⬜ **ESC, motor, prop matching** _(stubbed)_: kV vs. battery vs. prop diameter and pitch sizing for given target speed/thrust; thermal and current-draw checks; current sensor and ESC current limit verification.

- ⬜ **CG and balance calculator** _(stubbed)_: for fixed-wing planes, helis, multirotors: balance point from layout, neutral-point and static-margin calculation, payload effect.

- ⬜ **Battery cycle log** _(stubbed)_: tracks LiPo/Li-ion pack charge cycles, internal resistance trend, voltage sag under load, abuse events. Flags packs near end-of-life.

- ⬜ **FPV setup and video link** _(stubbed)_: VTX frequency selector (race-band awareness), goggle pairing, DVR/HD recording, latency profiling, antenna polarization mismatch detector.

- ⬜ **Race track timer and lap analyzer** _(stubbed)_: transponder integration, lap-time charts, sector splits, fastest-clean-lap, comparison across drivers/sessions.

- ⬜ **Model-specific build databases** _(stubbed)_: wiring diagrams, BOM, settings dumps for popular open-source RC projects (Apex, X-Class, F3F sailplanes, scale RC boats), pulled from RCGroups, GitHub repos, and community wikis.


#### Entertainment, trivia, pop culture

- ⬜ **Movie and TV trivia** _(stubbed)_: queries IMDb-class data offline, with cast/crew/episode/character details, behind-the-scenes notes, reception and review aggregation, soundtrack lookups.

- ⬜ **General trivia and quiz** _(stubbed)_: categorized trivia banks (history, geography, science, sports, etc.) with question-generation, difficulty scaling, multiplayer mode, party-game format, AI-generated questions from a topic seed.

- ⬜ **Music history and recognition** _(stubbed)_: Shazam-style audio fingerprinting, song/album metadata, lyrics, music-video matching, genre/era browsing, discography of an artist.

- ⬜ **Pop culture timeline** _(stubbed)_: year-by-year cultural reference: hit songs, top films, news events, fashion, technology releases. Useful for period-correct writing, escape-room design, themed parties, and quick context for historical research.

- ⬜ **Fandom and lore database** _(stubbed)_: structured data for well-documented fictional universes (Star Wars, Tolkien, Marvel, Star Trek, Pokemon, etc.) for AI-grounded answers on canon/character/timeline questions without making things up.

### Interface

- ✅ **Web app** _(in testing)_: self-contained C++ server on port 8080 with cpp-httplib. All UI assets are embedded into the binary; no Node or browser bundler in the build pipeline.

- ✅ **File tree** _(in testing)_: directory listing in the left pane with single-click open and drag-to-AI for image files. Polls for filesystem changes so external edits show up without manual refresh.

- ✅ **Multi-mode editor** _(in testing)_: tabbed editor shell with mode auto-detected from extension. Text modes are inline (markdown rendered by default with a toggle to source, code in a monospace textarea, prose with spellcheck); non-text modes render the appropriate tool into the tab (image editor, PDF reader, embedded browser, etc.). Per-file dirty state, Ctrl+S saves.

- ✅ **Terminal** _(in testing)_: single-pane keystroke-driven terminal per tab, with per-tab cwd that survives `cd` through the AI chat, command history, and the standard editing shortcuts.

- ✅ **AI chat** _(in testing)_: server-sent-event streaming of each pipeline layer's output into the chat log, with a collapsible "thinking" expander for the chain and a headline that shows the final handler's output. Markdown links in the chat render as clickable in-tool browser links with an inline download button.

- ✅ **Sessions** _(in testing)_: per-browser-tab session identified by a UUID in the URL fragment, backed by per-session SQLite for chat history and per-session JSON for UI state. Picker on cold start when multiple sessions exist; refresh restores UI and chat; sessions can be renamed, switched, or forgotten.

- ✅ **Settings** _(in testing)_: API credentials modal that stores keys (currently Mouser) in `settings/credentials.json` (gitignored) and reads them at tool-invocation time.

- ✅ **Folder picker** _(in testing)_: modal directory browser used to open the project root.

- ✅ **Status bar** _(in testing)_: shows pipeline-stage load progress in the menubar as the server starts up.

