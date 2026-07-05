<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# KiCad interoperation notice

**AutoClank (binary: `ac9`)** is licensed under **GPL-3.0-or-later**
(see `LICENSE` for the full text). This notice documents how AutoClank
interoperates with KiCad and why the licensing lines up.

## Summary

AutoClank is a fresh-authored codebase. It **reads and writes KiCad's
native file formats** (`.kicad_sch`, `.kicad_pcb`, `.kicad_sym`,
`.kicad_mod`, `.kicad_pro`, `.kicad_prl`, RS-274X Gerber, Excellon
drill, IPC-2581, Specctra DSN / SES). It can also **invoke `kicad-cli`
as a separate subprocess** for operations it doesn't yet do natively
(ERC, DRC, netlist, gerber, drill, STEP, SVG, PDF, 3D PNG).

Neither of those operations links KiCad code into the `ac9` binary,
and neither requires any KiCad copyright material to be redistributed
together with AutoClank.

## Licensing lines up because

1. **File formats are not copyrightable.** Reading and writing
   `.kicad_sch`, `.kicad_pcb`, `.kicad_sym`, `.kicad_mod`, and
   `.kicad_pro` is done from AutoClank's own s-expression parser and
   emitters (`modules/344_sexpr`, `347_kicad_io`), authored freshly.
   No verbatim KiCad code is included.

2. **`kicad-cli` runs at arm's length.** When AutoClank uses it (via
   `modules/340_kicad_bridge`), it forks a separate process and
   exchanges files. GPLv3 §5 explicitly permits "mere aggregation" of
   independent programs, and shelling out is the canonical example.
   `ac9` and `kicad-cli` are two distinct executables.

3. **AutoClank itself is GPLv3.** Even in the case where a future
   version chooses to statically or dynamically link KiCad libraries
   (`_pcbnew.kiface`, `_eeschema.kiface`, `libkicommon.so`), that is
   permitted because GPLv3 and GPLv3-or-later are compatible with
   themselves. No relicensing or "license upgrade" is required.

4. **The `kicad/` directory (if present at repository root) is
   reference material only.** It contains a snapshot of the upstream
   KiCad source, retained so contributors can look up format details
   and rendering behavior. It is excluded from the `ac9` CMake build
   via the `_TOOL_EXCLUDE_REGEX` filter in `CMakeLists.txt`. No file
   under `kicad/` is compiled into or linked with `ac9`. KiCad's own
   license notices (GPLv3 and several permissive supplements) remain
   in place under that tree; if you redistribute the repository
   including `kicad/`, you redistribute those license files with it,
   as GPLv3 requires.

## KiCad library data

The **KiCad symbol / footprint / 3D-model libraries** (`kicad-symbols`,
`kicad-footprints`, `kicad-packages3D`) are separate upstream
repositories licensed **CC-BY-SA-4.0**. AutoClank does not bundle
them; it indexes whatever copy the user has installed locally (or
points at a copy set via `TOOL_KICAD_SYMBOL_ROOT`, `TOOL_KICAD_
FOOTPRINT_ROOT`, `TOOL_KICAD_PACKAGE3D_ROOT` environment variables).
Distributing those libraries alongside AutoClank is fine because
Creative Commons §5(c) of CC-BY-SA-4.0 explicitly lists GPLv3 as a
"BY-SA Compatible License."

## Third-party components linked into `ac9`

The `ac9` binary itself pulls in the following components (all
GPLv3-compatible):

| Component | License | Where |
|---|---|---|
| nlohmann/json | MIT | JSON model I/O throughout |
| cpp-httplib | MIT | `modules/010_interface/httplib.h` |
| SQLite (amalgamation) | Public domain | Context store |
| ngspice (libngspice) | Modified BSD | `modules/849_spice_simulator` (dlopen at runtime) |
| three.js | MIT | Optional 3D viewer (browser-side, not linked into `ac9`) |
| llama.cpp / ggml | MIT | Language model runtimes |

When adding new deps, this table needs an entry.

## What this document is not

This is not legal advice. It documents the licensing intent so that
downstream users, distributors, and contributors have a single place
to check the story. Actual legal questions still route through
qualified counsel.
