# KiCad integration into `tool`: architecture and plan

Date: 2026-07-05
KiCad rev: 10.0.4 (built locally at `~/work/kicad/build/`)
Tool license: GPL-3.0-or-later (as of commit c15125d6)

---

## 1. Decision: subprocess for MVP, in-process later

Now that `tool` is GPLv3, we could statically link KiCad's kifaces. We are **not going to** for the first cut, for two reasons:

1. `tool`'s build is a single glob-recursive CMake pass. KiCad has ~1.5 M LoC and its own multi-target CMake tree with dozens of dependencies (wxWidgets, OpenCascade, ngspice, nng, protobuf, poppler, zint, libgit2). Grafting that into a glob build is weeks of CMake work before the first useful line of integration ships.
2. KiCad already provides a battle-tested headless surface: `kicad-cli`. Every operation the AI needs (parse, ERC, DRC, netlist, gerber, drill, STEP, SVG, PDF) is one subprocess away. `kicad-cli` is the same code path that KiCad Automation users depend on; it will not silently regress.

So: `tool` invokes `kicad-cli` as a child process for compute. In-process linking (KIFACE loads, protobuf/nng client to a running instance) becomes a later optimization when there is a concrete reason for it (real-time DRC feedback while the user drags a track, for example).

The subprocess boundary is not throwaway. It becomes a permanent tool boundary that also happens to isolate potential KiCad crashes from `tool`.

---

## 2. Module layout

New module slots (fill existing stubs, add new ones):

| Module | Role | Currently |
|---|---|---|
| `843_schematic_capture` | Emit `.kicad_sch` from an AI-produced circuit graph; render for display | stub |
| `844_pcb_layout` | Emit `.kicad_pcb` from schematic + placement hints; call PNS router via kicad-cli or eventually direct | stub |
| `846_footprint_editor` | Load, search, preview KiCad footprint libs; no editing in MVP | stub |
| `849_spice_simulator` | Direct libngspice binding (KiCad-independent); accepts SPICE netlist, returns waveforms | stub |
| `869_gerber_and_fab_output_viewer` | Render exported gerbers to layered SVG for the browser | stub |
| `NNN_kicad_bridge` (new; propose `340_kicad_bridge`) | Thin C++ wrapper around `kicad-cli`; owns process lifecycle, path resolution, output parsing | none |
| `NNN_kicad_libs` (new; propose `341_kicad_libs`) | Index and search symbol / footprint / 3D-model libraries | none |
| `009_tools/components` | Existing Mouser part search; wired into the pipeline as the parts oracle | exists |
| `849_spice_simulator` also owns SPICE netlist synthesis from schematic graph | | |

Numbering: `340`-`341` are currently free per the module listing; keep the electronics tools clustered but the new ones early enough to sort with the existing `330_circuit_builder`, `348_circuit_equivalence_checker`.

Everything under `NNN_kicad_bridge` is what talks to the subprocess. Everything else consumes its output.

---

## 3. Data flow: prompt to gerbers

```
user prompt
    │
    ▼
[001] cleanup ─► [002] dictionary ─► [003] stylize ─►
[004] expertise ─► [005] context ─► [006] disambiguate ─►
[007] knowledge ─► [008] entities ─► [009] tools
    │
    │  expertise router detects electronics/EDA intent
    ▼
── EDA branch ──────────────────────────────────────────
    │
    ▼
"circuit intent" (structured JSON produced by AI)
{
  "goal": "555 astable, 1 Hz LED blinker, single 9V supply",
  "constraints": {"supply_v":9, "target_freq_hz":1, "form_factor":"through-hole"},
  "parts_hint": ["NE555","LED red","220R resistor","1uF cap","33k resistor"],
  "topology": [
    {"ref":"U1","kind":"555_astable","pin_map":{"VCC":"+9V","GND":"GND","OUT":"R_LED"}},
    ...
  ]
}
    │
    ▼
[009/components] Mouser search per parts_hint ─► concrete MPNs, datasheets
    │
    ▼
[341/kicad_libs] map each concrete part to a KiCad symbol + footprint
    │
    ▼
[843/schematic_capture] emit  circuit.kicad_sch
    │
    ▼
[340/kicad_bridge] kicad-cli sch erc  ─► erc.json
    │  (violations surfaced to chat trail; AI may retry)
    ▼
[340/kicad_bridge] kicad-cli sch export netlist -> circuit.net
    │
    ▼
[844/pcb_layout] emit initial circuit.kicad_pcb with footprints placed
    (MVP: simple grid placement + ratsnest; PNS routing later)
    │
    ▼
[340/kicad_bridge] kicad-cli pcb drc  ─► drc.json
    │  (violations back to chat, AI may re-place)
    ▼
[340/kicad_bridge] kicad-cli pcb export {gerbers,drill,step,svg,pos,pdf}
    │
    ▼
[869/gerber_viewer] rasterize gerbers to SVG per layer for the web UI
    │
    ▼
chat trail: preview + downloadable zip
```

Each stage is a discrete tool call. The pipeline surfaces every intermediate output in the collapsible "thinking" expander the interface already ships.

---

## 4. Subprocess bridge design

`modules/340_kicad_bridge/`:

```cpp
namespace kicad_bridge {

struct RunResult {
    int         exit_code;
    std::string stdout_text;
    std::string stderr_text;
    std::string output_path;   // when the operation produces one file
};

// Configuration resolved once at startup and cached.
struct Config {
    std::string cli_path;      // /home/jwoods/work/kicad/build/kicad/kicad-cli
    std::string stock_data;    // KICAD_STOCK_DATA_HOME target
    std::string symbol_lib_table_path;
    std::string footprint_lib_table_path;
    bool        available = false;
};

void init();
const Config & config();

// Schematic
RunResult sch_erc(const std::string & sch_path,
                  const std::string & report_out,
                  bool json_format = true);

RunResult sch_netlist(const std::string & sch_path,
                      const std::string & net_out,
                      const std::string & format = "kicadsexpr");

RunResult sch_export_svg(const std::string & sch_path,
                         const std::string & dir_out);

RunResult sch_export_pdf(const std::string & sch_path,
                         const std::string & pdf_out);

RunResult sch_export_bom(const std::string & sch_path,
                         const std::string & csv_out,
                         const std::string & fields);

// PCB
RunResult pcb_drc(const std::string & pcb_path,
                  const std::string & report_out,
                  bool json_format = true,
                  bool schematic_parity = true);

RunResult pcb_export_gerbers(const std::string & pcb_path,
                             const std::string & dir_out,
                             const std::vector<std::string> & layers);

RunResult pcb_export_drill(const std::string & pcb_path,
                           const std::string & dir_out);

RunResult pcb_export_step(const std::string & pcb_path,
                          const std::string & step_out);

RunResult pcb_export_svg(const std::string & pcb_path,
                         const std::string & svg_out,
                         const std::vector<std::string> & layers);

RunResult pcb_export_pos(const std::string & pcb_path,
                         const std::string & csv_out,
                         const std::string & side = "both",
                         const std::string & fmt = "csv");

// Rendered 3D preview (existing kicad-cli pcb render → PNG)
RunResult pcb_render_png(const std::string & pcb_path,
                         const std::string & png_out,
                         int width_px = 1280,
                         int height_px = 720,
                         const std::string & side = "top");

// Footprint / symbol SVG previews for UI thumbnails
RunResult fp_export_svg(const std::string & lib_path,
                        const std::string & footprint_name,
                        const std::string & svg_out);

RunResult sym_export_svg(const std::string & lib_path,
                         const std::string & symbol_name,
                         const std::string & svg_out);

}
```

Implementation notes:
- Use `fork` + `execve` + pipes; do not use `popen`. Need to capture both streams and the exit code cleanly.
- Set `KICAD_STOCK_DATA_HOME` in the child env to point at the built KiCad's `share/` (or, after `make install`, `/usr/local/share/kicad`). This is what silences the `api.v1.schema.json` warning we saw.
- All `--format json` capable operations use JSON so `340_kicad_bridge` can parse them (`erc`, `drc`; bom uses CSV).
- Every RunResult carries stderr; surface it in the chat trail unedited so users see exactly what KiCad said.
- The child process runs under a resource cap (rlimit CPU/AS) so a stuck kicad-cli cannot wedge `tool`.

---

## 5. HTTP surface added to `010_interface/server.cpp`

Follows the existing `/api/<area>/<op>` convention (see the tickets and terminal endpoints at lines 1317, 826). New endpoints:

```
GET  /api/eda/kicad_status                         → {available, version, cli_path}
GET  /api/eda/symbol_search?q=555&limit=20         → [{lib, name, description, svg_url}]
GET  /api/eda/footprint_search?q=SOIC-8&limit=20   → [{lib, name, dims_mm, pads, svg_url}]
GET  /api/eda/symbol_svg?lib=&name=                → image/svg+xml
GET  /api/eda/footprint_svg?lib=&name=             → image/svg+xml

POST /api/eda/generate_schematic
     body: {intent_json, session_id}
     → {sch_path, symbols_used, warnings}
POST /api/eda/erc  body: {sch_path}                → {violations: [{severity, rule, at, message}], report_path}
POST /api/eda/netlist  body: {sch_path}            → {net_path}

POST /api/eda/generate_pcb
     body: {sch_path, options: {board_size_mm, layer_count, ...}}
     → {pcb_path, unrouted_ratsnest_count}
POST /api/eda/drc  body: {pcb_path}                → {violations: [...], report_path}
POST /api/eda/route  body: {pcb_path}              → {pcb_path_routed, unrouted}
POST /api/eda/export
     body: {pcb_path, kind: "gerbers"|"drill"|"step"|"pos"|"pdf"|"svg", options}
     → {output_dir | output_path}
GET  /api/eda/render_pcb?pcb_path=&layers=&format=svg
     → image/svg+xml or image/png

GET  /api/eda/spice/run?net=<path>&stimulus=...    → JSON {t, v, i} waveforms
```

All paths are `session_scratch/<uuid>/kicad/<work_id>/...` so multiple concurrent chat sessions do not step on each other. The session-scratch directory already exists in the sessions store convention; add a `kicad/` subdir.

---

## 6. The circuit-intent JSON schema

This is the interface between the AI and the `843_schematic_capture` emitter. Keep it small and stable; it is easier for the AI to generate a clean intermediate than a full `.kicad_sch`.

```json
{
  "meta": {
    "title": "555 astable, 1 Hz LED blinker",
    "notes": "single 9V supply, through-hole"
  },
  "power": {
    "nets": [
      {"name": "+9V", "voltage": 9.0},
      {"name": "GND", "voltage": 0.0}
    ]
  },
  "parts": [
    {"ref": "U1", "value": "NE555",   "lib_hint": "Timer:NE555", "footprint_hint": "Package_DIP:DIP-8_W7.62mm"},
    {"ref": "R1", "value": "33k",     "lib_hint": "Device:R",    "footprint_hint": "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal"},
    {"ref": "R2", "value": "33k",     "lib_hint": "Device:R",    "footprint_hint": "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal"},
    {"ref": "C1", "value": "22uF",    "lib_hint": "Device:C_Polarized", "footprint_hint": "Capacitor_THT:CP_Radial_D5.0mm_P2.50mm"},
    {"ref": "C2", "value": "10nF",    "lib_hint": "Device:C",    "footprint_hint": "Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P5.00mm"},
    {"ref": "D1", "value": "LED",     "lib_hint": "Device:LED",  "footprint_hint": "LED_THT:LED_D5.0mm"},
    {"ref": "R3", "value": "220",     "lib_hint": "Device:R",    "footprint_hint": "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal"}
  ],
  "connections": [
    ["U1.VCC", "+9V"],
    ["U1.GND", "GND"],
    ["U1.RESET", "+9V"],
    ["R1.1", "+9V"],
    ["R1.2", "U1.DIS"],
    ["R2.1", "U1.DIS"],
    ["R2.2", "U1.THR"],
    ["U1.THR", "U1.TR"],
    ["U1.TR", "C1.+"],
    ["C1.-", "GND"],
    ["U1.CTRL", "C2.1"],
    ["C2.2", "GND"],
    ["U1.OUT", "R3.1"],
    ["R3.2", "D1.A"],
    ["D1.K", "GND"]
  ],
  "placement_hints": {
    "board": {"size_mm": [50, 30], "layers": 2},
    "group": [{"refs": ["R1","R2","C1"], "near": "U1"}]
  }
}
```

The AI produces this from the user prompt. `843_schematic_capture` translates it. The `lib_hint`/`footprint_hint` fields are the AI's *suggestion*; the emitter validates against KiCad's actual library index (see §7).

---

## 7. Symbol / footprint library indexing

KiCad ships three data repos separate from the main source:

- `kicad-symbols` (schematic symbols, roughly 20k)
- `kicad-footprints` (roughly 15k)
- `kicad-packages3D` (STEP and WRL 3D models)

All three are CC-BY-SA-4.0, GPLv3-compatible (per §5(c) of CC-BY-SA-4.0). Bundle them under `resources/kicad/` (chunked per the existing self-contained convention) or point at a user-installed location.

`341_kicad_libs` builds a SQLite FTS5 index on first run:

```sql
CREATE TABLE symbol (
  id INTEGER PRIMARY KEY,
  lib TEXT NOT NULL,       -- e.g. "Timer"
  name TEXT NOT NULL,      -- e.g. "NE555"
  description TEXT,
  keywords TEXT,           -- from KiCad's symbol keyword field
  fp_filters TEXT,         -- footprint filter globs
  path TEXT NOT NULL,      -- .kicad_sym file
  offset INTEGER           -- byte offset for fast slice
);
CREATE VIRTUAL TABLE symbol_fts USING fts5(name, description, keywords, content='symbol');

CREATE TABLE footprint (
  id INTEGER PRIMARY KEY,
  lib TEXT NOT NULL,       -- e.g. "Package_DIP"
  name TEXT NOT NULL,
  description TEXT,
  keywords TEXT,
  pad_count INTEGER,
  smd INTEGER,
  attrs TEXT,
  path TEXT NOT NULL       -- .pretty/*.kicad_mod
);
CREATE VIRTUAL TABLE footprint_fts USING fts5(name, description, keywords, content='footprint');
```

The KB is the same shape as `tool`'s existing dictionary / Wikipedia indices, so the storage and query patterns already exist in `005_context` and `002_dictionary`.

Lookup flow: AI's `lib_hint` first tries an exact match; if it misses, falls through to FTS with the value string. If still no match, returns candidates to the chat trail as a disambiguation stop (following the pipeline's existing stop-and-ask pattern).

---

## 8. Emitter: circuit intent → .kicad_sch

The `.kicad_sch` format is stable s-expression; the head of every file follows the pattern shown in §11. `843_schematic_capture` builds the file in one pass, no wxWidgets involved:

```
(kicad_sch
  (version 20250114)
  (generator "tool")
  (generator_version "0.1")
  (uuid <fresh-uuid>)
  (paper "A4")
  (lib_symbols
    <inlined LIB_SYMBOL blocks copied from kicad-symbols>
  )
  <SCH_SYMBOL instances (one per part) with (at X Y ANG) placement>
  <SCH_JUNCTION for each net-node fanout>
  <SCH_WIRE segments connecting pins>
  <SCH_LABEL for named nets (+9V, GND, etc.)>
  (sheet_instances (path "/" (page "1")))
)
```

MVP placement: grid layout by ref-designator group, wires as orthogonal L-shapes. Not pretty, but ERC-valid. A follow-up pass invokes eeschema's `sch export svg` and, if the pipeline detects the schematic is unreadable (heuristic: crossings above threshold), retries with `dagre`-style hierarchical placement.

Lib symbol copy strategy: rather than reference external libraries (which requires the running project to have the right lib table), inline the referenced `lib_symbols` block. This is exactly what KiCad's own project files do for portability. The inlined blocks come straight from the index in §7.

---

## 9. Emitter: netlist → .kicad_pcb (initial layout)

Two stages inside `844_pcb_layout`:

**Stage A** (`place`): read `.kicad_sch` and the netlist, allocate a board of the requested size, place each footprint on a grid with headroom, drop the ratsnest. Emits a valid `.kicad_pcb` with no routed traces.

**Stage B** (`route`): three options in order of preference:

1. Shell to `freerouting` (Java, EPL-2.0 license, GPLv3-compatible). Accepts DSN specctra format; KiCad already exports it. Slow but works today.
2. Wrap the built KiCad's PNS router by loading `_pcbnew.kiface` in-process and calling `PNS_KICAD_IFACE_BASE::SyncWorld` + `ROUTER::RouteAll`. Requires the kiface DSO load path but no CMake grafting. Fast, in-tree. Moderate effort.
3. Ship kicad-cli with a `pcb autoroute` verb we contribute upstream. Right thing to do long-term; not MVP.

MVP goes with Option 1. Option 2 is the natural upgrade after MVP.

---

## 10. UI in `010_interface`

The current interface is per-file-tab. Adds:

- **Schematic tab**: file with `.kicad_sch` extension opens as an SVG (via `sch export svg`), zoom/pan. Selection currently one-way (click a symbol → chat mentions its ref). Edit UI comes later.
- **PCB tab**: file with `.kicad_pcb` extension opens as layered SVG (one path element per layer, toggleable), zoom/pan, layer visibility panel. Ratsnest overlay from the netlist. 3D preview button calls `pcb render` and shows the PNG.
- **Gerber tab**: file with `.gbr` or a directory of gerbers opens the gerber viewer canvas. Layers from the file's `.gbrjob` companion.
- **EDA sidebar** (new, in the file-tree pane): a section listing symbol libs, footprint libs, and recently generated boards; drag a symbol into the schematic tab to place.

None of this is a full editor. It is enough to *see and download* what the AI produces, and to inspect DRC violations by click.

Frontend deps: SVG panning is `svg-pan-zoom` (MIT). Gerber → SVG rasterization on the backend, not the frontend.

---

## 11. File-format targets to lock down

For the AI to reliably synthesize valid KiCad files, we need three format specs solid enough that a small model can emit them. Order of importance:

1. `.kicad_sch` v20250114 (schematic): sexpression, ~40 top-level forms.
2. `.kicad_pcb` v20241229 (PCB): sexpression, larger; ~90 top-level forms.
3. `.kicad_sym` and `.kicad_mod`: only need the *read* path for symbol/footprint inlining; write path not needed in MVP.

Reference sources:
- KiCad docs: `~/work/kicad/Documentation/file_formats/` (bundled in the main repo).
- Definitive parser: `~/work/kicad/pcbnew/pcb_io/kicad_sexpr/pcb_io_kicad_sexpr_parser.cpp` and `eeschema/sch_io/kicad_sexpr/sch_io_kicad_sexpr_parser.cpp`. When docs disagree with parser, parser wins.
- Real-world corpus: `~/work/kicad/demos/` for the "canonical" shape; `~/work/kicad/qa/data/` for edge cases.

Write a small test harness in `testing/`: for every `.kicad_sch` in demos, round-trip through `843_schematic_capture` (read → intermediate → emit) and `diff` the s-expr forms modulo whitespace and UUID. Non-goal: byte-exact match. Goal: KiCad opens the emitted file without warnings.

---

## 12. SPICE integration in `849_spice_simulator`

Independent of KiCad. Direct link against `libngspice0` (already installed):

```cpp
namespace spice_simulator {

struct Signal {
    std::string name;
    std::vector<double> t;
    std::vector<double> v;    // or i, depending on probe
};

struct RunResult {
    std::vector<Signal> signals;
    std::string log;
    bool success;
};

RunResult run(const std::string & spice_netlist,
              const std::string & analysis_command);  // e.g. "tran 10us 5ms"

}
```

Netlist generation: call `kicad-cli sch export netlist --format spice`. Feed the output straight into ngspice.

For the chat trail: when the AI answers a question that would benefit from simulation (transient / AC / DC sweep on a circuit under discussion), the pipeline auto-generates the SPICE stimulus, runs it, and includes the plot (SVG rendered from the signal points) in the answer.

---

## 13. Gerber viewer in `869_gerber_and_fab_output_viewer`

Two choices:

1. Rasterize per-layer via `kicad-cli pcb export svg --layers F.Cu` etc. Cheapest. Requires the source `.kicad_pcb`. But we already have that. Use this.
2. A standalone gerber renderer (there is Tracespace on npm, MIT-licensed; a C++ gerber parser could be lifted from `~/work/kicad/gerbview/rs274x.cpp` under GPLv3, fine now).

MVP: Option 1. Add Option 2 later so `.gbr` files without a project source open.

---

## 14. Concrete first prototype: 555 astable

Two weeks of work, end to end:

**Week 1**
- Build `340_kicad_bridge` (subprocess wrapper, ~600 LoC).
- Build `341_kicad_libs` symbol/footprint index (~800 LoC + SQLite migration).
- Import `kicad-symbols`/`kicad-footprints` as submodules or chunked resources.
- Hand-author a valid `555_blinker.kicad_sch` and `.kicad_pcb` to lock the format targets. Run through `kicad-cli sch erc` and `pcb drc`. Fix until clean.
- Add the `/api/eda/*` endpoints in `server.cpp`; simple frontend tab for viewing.

**Week 2**
- Build `843_schematic_capture`: intent JSON → `.kicad_sch`. Test with a hand-authored intent for the same 555 blinker. Aim for byte-identical output to the hand-authored `.kicad_sch` after normalization.
- Wire the expertise router (in `004_expertise`) to detect an "electronics design" intent and hand off to the EDA branch.
- Add a small "circuit intent producer" prompt to the answering model (`009_tools/answer.cpp`) that yields the JSON shape from §6.
- End-to-end demo: user types "make me a 1 Hz LED blinker with a 555, 9V"; pipeline lands `.kicad_sch`, ERC clean; `.kicad_pcb` with unrouted footprints; opens in the UI. Gerbers on demand.

Explicitly NOT in prototype: routing (leave ratsnest), 3D, footprint editing, custom symbols. Those come next.

---

## 15. Roadmap after prototype

| Milestone | New capability | Effort |
|---|---|---|
| M2 | Autoroute via freerouting subprocess; produce fabricable gerbers end-to-end | 2-3 wk |
| M3 | Symbol / footprint search UI in the sidebar; drag-to-place | 3-4 wk |
| M4 | Ngspice waveform tab; auto-simulate on request | 2-3 wk |
| M5 | Constraints (impedance-controlled traces, matched lengths) driven by the intent JSON | 4-6 wk |
| M6 | Multi-sheet hierarchical schematics; larger designs | 3-5 wk |
| M7 | Load `.kiface` in-process for real-time DRC (replace subprocess for hot paths) | 6-10 wk |
| M8 | Footprint editor UI; custom footprints from datasheet-scraped dims | 6-10 wk |
| M9 | 3D preview via the built-in `kicad-cli pcb render` PNG; then GLB streaming for browser 3D | 3-5 wk |
| M10 | Netlist import from other formats (Altium, EAGLE) using KiCad's existing importers via kicad-cli or direct kiface | 2-4 wk |

---

## 16. Risks and open questions

- **Symbol/footprint library size**: `kicad-symbols` + `kicad-footprints` + `kicad-packages3D` is roughly 2 GB uncompressed (mostly the 3D models). If we honor the self-contained rule, that is a significant chunk. Options: chunk 3D models, or omit them for the first cut (STEP export can still work with per-part downloads from Mouser).
- **kicad-cli availability at runtime**: `340_kicad_bridge::init` needs to find a `kicad-cli`. First-cut: expect the user has KiCad installed at a common location (Ubuntu: `/usr/bin/kicad-cli`; our build: `~/work/kicad/build/kicad/kicad-cli`). Longer-term: bundle a statically-linked kicad-cli in `bin/`. That is a nontrivial build project on its own.
- **Version drift**: KiCad file formats bump their version integer (`(version 20250114)`) periodically. Pin the target version per release; `sch upgrade` / `pcb upgrade` handle upgrades from older versions when we need to consume them.
- **Router quality**: freerouting is slow and produces uneven results on dense boards. Nobody in the open EDA world has a great answer to autorouting except paid tools. This is where an AI-driven placer might actually add novel value; keep an eye on it.
- **Manufacturability constraints**: The DRC we run from `kicad-cli` uses the *board file's* rule set. We need a small default profile (JLCPCB-friendly: 6/6 mil trace/space, 0.3 mm min drill) that we drop into every generated board unless overridden.

---

## 17. Anchor points in the codebase to reference during implementation

- Interface HTTP server: `modules/010_interface/server.cpp` — new endpoints live here.
- Session-scratch convention: `modules/010_interface/sessions_store.cpp`.
- Pipeline dispatch: `modules/009_tools/answer.cpp`, `modules/009_tools/classify.cpp`, `modules/004_expertise/expertise.cpp`.
- Existing tool-shape module for reference: `modules/009_tools/components/components.hpp` (Mouser search) is the template for the EDA modules' API surface.
- KiCad demos to learn from: `~/work/kicad/demos/microwave/microwave.kicad_pcb` is the smallest and cleanest; `~/work/kicad/demos/complex_hierarchy/` shows multi-sheet.
- KiCad parsers as ground truth:
  - Schematic: `~/work/kicad/eeschema/sch_io/kicad_sexpr/sch_io_kicad_sexpr_parser.h:77`.
  - PCB: `~/work/kicad/pcbnew/pcb_io/kicad_sexpr/pcb_io_kicad_sexpr_parser.h`.
- kicad-cli entry: `~/work/kicad/build/kicad/kicad-cli`. Source: `~/work/kicad/kicad/cli/`.

---

## 18. Summary

Ship the subprocess integration first. It gives you the whole KiCad compute surface (ERC, DRC, gerbers, drill, STEP, position files, PDF, SVG, 3D render, netlist, BOM) in exchange for one C++ wrapper module and a couple of database tables for library indexing. The AI generates a small stable intermediate (§6) and one emitter (§8) turns it into files KiCad's own tools validate. Nothing is thrown away when you later switch to in-process kiface loading for the operations that need it.

Two weeks to the 555 blinker demo. Three months to something a hobbyist could reasonably use to spin a two-layer board.
