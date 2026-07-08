# Porting KiCad into `tool` - feasibility &amp; effort

Date: 2026-07-05
KiCad rev analyzed: 10.0.4 (latest stable), git 3a2065e8 → tag f7414d419.

---

## TL;DR

Technically feasible. Big. And there's a licensing landmine.

- KiCad 10.0.4 is **~1.5 M lines of C++** across ~3.5 K files, plus ~42 K lines of Python bindings, ~398 K lines of wxFormBuilder XML (generated dialogs), and a handful of `.cmake`.
- The core is well-layered: geometry kernel + file parsers + engines (DRC, PNS router, netlist, ERC, plotters, STEP export) are separable from the wx GUI. `kicad-cli` already runs most engines headless.
- **License blocker:** KiCad is **GPLv3**. `tool` targets PD/Unlicense. Linking KiCad code into `tool` forces the whole `tool` binary to GPLv3. This is the single most important decision on the table; every effort estimate below is conditional on how you resolve it (see §1).
- `tool` already has slot dirs for `843_schematic_capture`, `844_pcb_layout`, `846_footprint_editor`, `849_spice_simulator`, `869_gerber_and_fab_output_viewer`, `408_pcb_silkscreen_reader`, `330_circuit_builder`, `1196_pcb_rf_stackup_helper` - all currently empty stub cpp/hpp. The scaffolding is ready.

---

## 1. Licensing (read this first)

KiCad's own source is **GPLv3-or-later** (main files) with some vendored libs under BSD/Apache/MIT/CC0/Boost. Directly linking any GPLv3 KiCad code into `tool`:

- **Contaminates the whole `tool` binary** - every module you link with becomes GPLv3, killing the PD/Unlicense goal noted in your memory (`project_release_licensing`).
- Even reading GPLv3 code and reimplementing it "from a fresh reading" is legally shaky; the safer path is **clean-room** (someone specs the behavior from public docs / file-format specs; someone else implements without ever reading GPLv3 KiCad source).

Options, from least to most costly:

| Option | License impact on `tool` | Effort |
|---|---|---|
| A. Ship KiCad as a **separate binary** (`tool` shells out to `kicad-cli` for DRC/gerber/step, and to `eeschema-cli`-equivalent) | `tool` stays PD; KiCad stays GPLv3, isolated in its own process | **Small** - days to wire up shell-outs and file exchange. Loses in-process integration and any UI you want to build inside `tool`. |
| B. Vendor KiCad as a **GPLv3 module inside `tool`** and re-license the whole project as GPLv3 | Kills PD goal | Small licensing effort, still ~M lines to keep building |
| C. Vendor only KiCad's **BSD/permissive sublibraries** (`libs/kimath` is under KiCad license = GPLv3 too - check each file's header; some upstream deps like Clipper2 and boost polygon are permissive) | Requires per-file license audit; mixed outcome | Medium |
| D. **Clean-room port** of the pieces you actually want (native s-expr file format is documented; DRC rules are documented; Gerber/Excellon are IPC standards) | `tool` stays PD | **Very large** - see §5. |
| E. Use an already-permissive EDA alternative as your kernel (Horizon EDA is GPLv3 too; LibrePCB is GPLv3; there is no serious permissively-licensed EDA suite) | N/A | N/A |

**Recommendation:** Option A for the MVP. `kicad-cli` already exposes `pcb drc`, `pcb export gerbers|drill|step|pos|pdf|svg`, `sch export netlist|bom|pdf|svg`, `sym export`, and `fp export`. `tool`'s AI pipeline can build boards by generating `.kicad_sch`/`.kicad_pcb` files (the format is a well-known s-expr) and driving `kicad-cli`. That gets you fabrication-grade output without vendoring GPLv3 code. Then, over time, clean-room-port only the pieces where in-process integration actually pays off (probably: schematic capture UI, live DRC feedback, footprint editor).

If PD is negotiable, Option B is by far the cheapest engineering path.

---

## 2. Tool project architecture (recap)

- C++17 monolith, single `tool` binary, glob-recursive CMake pulling every `.cpp` under the tree.
- Web UI: cpp-httplib server on 8080 (in `modules/010_interface/`), HTML/JS/CSS assets, xterm.js terminal, ToastUI editor, CodeJar, Prism.
- SQLite for context per session (vendored amalgamation).
- ~2200 module dirs; each named `NNN_<slug>` with matching `slug.cpp`/`slug.hpp`. Most are stubs exposing a `namespace::status()`.
- Pipeline: prompt cleanup → dictionary → stylize → expertise routing → context → disambiguate → knowledge → entities → tools → interface. Each stage is a module.
- Local LLM runtimes already integrated (coder / physics / chemistry models chunked into the repo).
- Existing self-contained ethos: no CDNs at runtime, everything vendored.

**Verdict on integration model:** KiCad's `KIWAY`/`KIFACE` DSO model isn't a natural fit for tool's single-binary glob build. Anything you pull in gets statically linked into `tool`. Either accept that (works fine, `tool` gets larger), or wrap KiCad as an out-of-process helper.

---

## 3. KiCad size and shape

Whole tree (v10.0.4, non-QA):

| Language | Files | Lines |
|---|---|---|
| C++ (.cpp/.cxx/.cc) | 3,060 | 1,383,147 |
| C/C++ headers (.h/.hpp) | 3,529 | 174,713 |
| Python | 202 | 42,116 |
| CMake | 79 | 9,548 |
| wxFormBuilder (.fbp XML) | 260 | 398,174 |
| JS/Lua | 4 | 382 |

Excluding tests (`qa/`, 130 K LoC) and vendored `libs/` (43 K). Adding QA back brings it to ~1.55 M LoC.

Per subsystem:

| Subsystem | Files | LoC | Purpose |
|---|---|---|---|
| pcbnew | 1,206 | 498,935 | PCB layout, footprints, DRC, PNS router, fab export |
| common | 773 | 313,099 | shared: settings, GAL, tool framework, plotters, io, dialogs |
| eeschema | 743 | 313,416 | schematic capture, ERC, netlist, SPICE integration |
| include | 510 | 90,165 | headers exposing common APIs |
| 3d-viewer | 204 | 55,294 | OpenGL + raytracer board view |
| libs (all) | 155 | 43,092 | kimath, kiplatform, core, sexpr, kinng |
| kicad (launcher) | 167 | 32,091 | top-level project frame + updater |
| gerbview | 93 | 25,516 | Gerber/Excellon viewer |
| pcb_calculator | 125 | 21,228 | transmission line, attenuator, E-series calcs |
| plugins (3d loaders) | 81 | 21,355 | OCE (STEP/IGES), VRML, IDF loaders |
| pagelayout_editor | 51 | 10,000 | drawing sheet designer |
| cvpcb | 36 | 6,829 | footprint assigner |
| bitmap2component | 13 | 2,767 | raster to symbol/footprint |

---

## 4. Key architectural findings

### 4a. wxWidgets coupling
- The **data-model headers** (`BOARD`, `FOOTPRINT`, `PAD`, `PCB_TRACK`, `ZONE`, `NETINFO_ITEM`, `SCH_ITEM`, `SCHEMATIC`, `SCH_SYMBOL`, `LIB_SYMBOL`, `SCH_PIN`, `SCH_SHEET`, `SCH_SCREEN`, `CONNECTION_GRAPH`) contain **zero** references to `wxWindow`/`wxFrame`/`wxDialog`.
- But they use `wxString`, `wxPoint`, `wxSize`, `wxFileName`, `wxLog*`, `wxT()`, `_HKI` everywhere. `libwx_base` (the non-GUI wxWidgets subset) is effectively non-optional.
- Practical: KiCad **cannot** be built without wxWidgets. A full de-wx port would touch essentially every source file. Estimate: several engineer-months of mechanical rewrites plus regression testing.

### 4b. Headless paths already exist
`kicad-cli` (`pcbnew_jobs_handler.cpp`, `eeschema_jobs_handler.cpp`) already runs the engines without any UI:
- `pcb drc | export gerbers | export drill | export step | export pos | export pdf | export svg | render`
- `sch export netlist | bom | pdf | svg`
- `sym export svg` / `fp export svg`

This is why Option A above is cheap. You're driving a battle-tested headless command surface.

### 4c. Geometry kernel is clean
`libs/kimath` (32 K LoC): `VECTOR2/3`, `BOX2`, `SHAPE_*` hierarchy, `SHAPE_POLY_SET` (with Clipper2), triangulation, convex hull, R-tree. **~165 wx references across the whole subtree**, all trivial. This is the single most reusable chunk of KiCad - it's essentially a general 2D CAD kernel with an s-expr file format bolted on. If any part of KiCad is worth clean-room-porting first, it's this.

### 4d. GAL headless mode
`common/gal/graphics_abstraction_layer.h` has an abstract `GAL` with three backends: Cairo (software vector), OpenGL, and `CALLBACK_GAL` (used for headless plotting/DRC harvesting). Cairo + CALLBACK_GAL are usable without a display.

### 4e. File formats
- `.kicad_pcb`, `.kicad_sch`, `.kicad_mod`, `.kicad_sym`, `.kicad_pro`, `.kicad_dru` are all documented s-expressions (KiCad docs + community wiki). Round-tripping them without KiCad code is a bounded, well-scoped project.
- Parsers use a shared `DSNLEXER` (`include/dsnlexer.h`) + keyword tables generated from `*.keywords` files.

### 4f. SPICE
Simulation is behind a pure-virtual `SPICE_SIMULATOR` (`eeschema/sim/spice_simulator.h`) with an ngspice implementation that dynamically loads `libngspice.so`. This is trivially replaceable / reusable - you can drive ngspice directly from `tool` with ~500 LoC of C++.

### 4g. Python bindings
Only **pcbnew** exposes a Python API (29 SWIG `.i` files). eeschema has none. The newer replacement is a **protobuf + nng** IPC API in `~/work/kicad/api/` - 12 `.proto` files (~3.2 K lines) modeling board/schematic CRUD ops. This is a more portable integration surface than SWIG.

### 4h. 3D + STEP export
`pcbnew/exporters/step/` uses **OpenCascade** (heavy C++ dep, 500 MB installed). If you don't need STEP export, dropping this saves a large dep. If you do, plan for OCCT in your dep chain.

### 4i. Router
PNS router in `pcbnew/router/` (38 K LoC). Interface `PNS::ROUTER_IFACE` has a `_BASE` split meant for headless use. Interesting for an AI tool: an agent that lays out routes could call PNS directly instead of freerouting-in-a-JVM.

---

## 5. Effort estimates by path

Assumes one senior C++ engineer (or a small AI-augmented effort tracking similar throughput).

### Path A - `tool` orchestrates `kicad-cli` (recommended MVP)

| Task | Effort |
|---|---|
| Bundle KiCad binaries or provide install helper; detect on startup | 1 wk |
| Populate `modules/843_schematic_capture` etc. as thin C++ modules that write `.kicad_sch`/`.kicad_pcb` files and invoke `kicad-cli` | 2-4 wk |
| Schematic drawer: web UI in `010_interface` for wire/symbol placement, backed by a small in-memory model that serializes to `.kicad_sch` | 4-8 wk |
| Footprint chooser + library search (uses KiCad's default footprint libs, GPLv3-independent - they're licensed CC-BY-SA-4.0 which is also share-alike; flag) | 2 wk |
| DRC round-trip: run `kicad-cli pcb drc`, parse JSON output, surface violations in the chat trail | 1 wk |
| Gerber/drill/step export via CLI | 1 wk |
| Ngspice integration in `849_spice_simulator` (direct link to libngspice, not KiCad-mediated) | 2 wk |
| Gerber viewer in `869_...`: render gerber → SVG/PNG for preview (there are permissive gerber libs; **or** shell to `gerbv` which is GPLv2+ - same license concern) | 3 wk |
| AI pipeline: prompt → BOM → symbols → schematic → footprints → routed board (this is the actually novel work) | 8-16 wk |

**Total Path A: ~5-9 months** for a working "describe a circuit, get gerbers" flow. `tool` stays PD; KiCad runs beside it.

### Path B - vendor GPLv3 KiCad into `tool` and relicense `tool`

| Task | Effort |
|---|---|
| Import KiCad as a submodule or `git subtree` under `modules/kicad/` | 1 wk |
| Adapt CMake: KiCad wants its own CMake tree; either build it as external project or graft its libs into `tool`'s glob-recursive build (glob-recursive will break - you'll need explicit KICAD_SOURCES lists) | 3-6 wk |
| Wrap KIWAY/KIFACE for use inside a single-binary tool (KIFACE assumes DSO loading; can call it as static libs but you lose the plugin story) | 4-6 wk |
| Add HTTP endpoints inside 010_interface that map to KIFACE calls | 4-6 wk |
| Build a web UI over the KiCad data model - this is a lot of UI work you're recreating what wxWidgets already does | 12-24 wk |
| Full end-to-end integration + AI pipeline | 8-16 wk |

**Total Path B: ~8-14 months**, and `tool` becomes GPLv3.

### Path C - clean-room port only what you need, stay PD

| Component | Clean-room effort |
|---|---|
| `.kicad_sch` / `.kicad_pcb` / `.kicad_mod` parser + writer | 3-6 mo |
| Geometry kernel (`SHAPE_POLY_SET` equivalents) | 4-8 mo (Clipper2 is permissive - start there) |
| ERC engine | 2-4 mo |
| DRC engine (rules language + tests) | 6-12 mo |
| Netlist generation + SPICE netlisting | 2-3 mo |
| Gerber/Excellon writer (IPC docs are public) | 1-2 mo |
| Pick-and-place / BOM | 0.5-1 mo |
| STEP export (needs OCCT LGPL - permissive-ish) | 2-4 mo |
| PNS-equivalent router | **12-24 mo** (this is a research project) |
| Schematic capture UI (web) | 3-6 mo |
| PCB layout UI (web canvas w/ zoom/pan/drag) | 6-12 mo |
| Footprint editor UI | 2-4 mo |

**Total Path C: ~4-8 person-years** to reach rough parity with the pieces `tool` actually needs. Skip the router (use `freerouting`, EPL license - still not PD, or ship as external process) and this collapses to 2-4 person-years.

### Path D - hybrid

- Use `libs/kimath`-style permissive kernels where they exist (Clipper2 for polygons, OCCT for 3D - LGPL, arms-length link).
- Clean-room the file-format layer.
- Shell out to `kicad-cli` for DRC/Gerber/STEP for now (Option A style) with a clear roadmap to replace each subsystem in place.
- Roughly Path A cost up front, with the option to swap to Path C piecewise.

**This is probably the pragmatic answer.** Ship the AI flow now on top of `kicad-cli`, replace stages with in-process code as the value case for each becomes clear.

---

## 6. Immediate next steps

1. **Decide on license posture.** Is `tool` staying PD? If yes, we're doing Path A / D. If negotiable, Path B is the fastest to something integrated.
2. **Confirm the build here works** - I have it building at 26% in background; will report result and any errors when it finishes.
3. **Prototype: the smallest thing that proves the AI can drive KiCad.** Something like:
   - Given a prompt: "555 timer astable, 1 Hz LED blinker"
   - AI pipeline emits a `.kicad_sch` s-expression from scratch (small enough to hand-model)
   - `kicad-cli sch export netlist` produces a netlist
   - AI emits a `.kicad_pcb` with a rough placement
   - `kicad-cli pcb drc` reports violations back into the chat trail
   - `kicad-cli pcb export gerbers` produces fab output
   - Total: probably 2-3 weeks to a demo, will surface every real integration issue.
4. **Vendor the file-format docs**: KiCad has a `Documentation/development/` tree in a separate repo (`kicad-doc`). Grab the file-format specs before you commit to a clean-room path.

---

## 7. What I'd flag as risky

- **UI parity.** KiCad's UI is 80 K+ lines of dialogs and 50 K+ lines of interactive tools. Reproducing even a fraction in a web UI is a serious frontend project. The reason KiCad exists at scale is the two decades of UI polish. Consider whether `tool` needs a real EDA UI or whether "AI generates files, human opens them in KiCad" is enough.
- **Router.** PNS is a substantial research artifact. Don't underestimate.
- **3D / STEP.** OpenCascade is a 30+ MB library with a real learning curve. Skip until you actually need STEP.
- **Footprint / symbol libraries.** These live in separate repos (`kicad-symbols`, `kicad-footprints`, `kicad-packages3D`) under **CC-BY-SA-4.0** - share-alike again. If you ship these, they contaminate your output through the SA clause differently than GPLv3 does code. Worth reading carefully.
