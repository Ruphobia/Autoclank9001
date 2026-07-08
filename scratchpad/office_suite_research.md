# Office-Suite Integration Research for Autoclank9001

**Scope.** ac9 is a local-first C++17 project (LICENSE: GNU GPL v3.0 - see
`/home/jwoods/work/Autoclank9001/LICENSE`) built around vendored llama.cpp
and a `cpp-httplib`-based web server (`modules/010_interface/server.cpp`,
~5,100 lines). Static assets are embedded into the binary via
`modules/010_interface/embed_assets.py` at CMake configure time. The
existing front-end already bundles Toast UI editor and CodeJar for
in-browser editing. The goal is to add first-class Word processor +
Spreadsheet + Slide deck + block-diagram tooling, matching the scope of
what LibreOffice ships. Everything must work with **no external network
required**.

This report compares seven realistic option families, scores each on six
axes, then recommends a concrete hybrid.

---

## 1. ac9's constraints (short)

Every recommendation below is filtered through these:

- **Copyleft.** ac9 is GPL-3.0-or-later. Any embedded office code must
  either be GPL-compatible or run as a genuinely separate process (i.e.,
  IPC / RPC / HTTP boundary) so ac9 can link/bundle without absorbing
  its license terms.
- **No cloud.** All rendering, persistence, collaboration must happen on
  the local box. Rules out anything that phones home for fonts, tiles,
  templates.
- **Single-binary discipline.** ac9 embeds all UI assets into the
  binary today. Any static-asset option that stays consistent with that
  is a strong signal.
- **httplib server, not nginx.** Reverse-proxying WebSocket + tile
  traffic through `httplib::Server` is possible but non-trivial -
  httplib does not natively proxy; a passthrough handler must marshal
  bodies + upgrade the WS.
- **Local LLMs already saturate one GPU per role.** Any option that
  wants a hot 4-8 GB RAM daemon idle is expensive but not
  disqualifying.

---

## 2. The candidates

### 2.1 LibreOffice (core) as vendored source, embedded via LibreOfficeKit

**Repo:** <https://github.com/LibreOffice/core> - read-only mirror of the
Gerrit-hosted canonical source.

**Size:** 6,926,426 KB (~6.6 GB) at the master branch tip - this is
far over the 200 MB ceiling in the task instructions, so I did not
clone. A shallow clone (`--depth=1 --filter=blob:none`) would still be
>1 GB.

**Language mix:** Primary C++, with substantial BASIC (StarBasic
macros), Python, XML/XSLT (UNO configuration + XSL filters), Java
(select components; Base uses it heavily, Writer/Calc/Impress do not
strictly require it if disabled).

**License:** Dual **MPL 2.0 + LGPL v3+** for new source files. Older
files are LGPL v3+. Third-party components pull in additional licenses:
Poppler (GPL v2/v3), Hunspell (MPL v1.1 / GPL v2 / LGPL v2.1
tri-license), Graphite2 (LGPL v2.1 / MPL / GPL v2+), plus a long list
of Apache 2.0, MIT, and BSD-style deps. Confirmed at
<https://api.libreoffice.org/share/readme/LICENSE.html>.

**GPL-3.0-or-later compatibility:** ✅ Yes.
- LGPL v3+ is trivially GPL v3+ compatible.
- MPL 2.0 §3.3 makes MPL-covered files **explicitly compatible** with
  GPL v3 (dual-license transition path). The combined work is GPL v3 as
  a whole; MPL-covered files also remain independently redistributable
  under MPL.
- No AGPL-only components.

**Subset needed for Writer + Calc + Impress + Draw:** The module
skeleton (from the README) is `sal → tools → vcl → framework → sfx2 →
svx → sw (Writer) → sc (Calc) → sd (Draw+Impress, one module)` plus
`basegfx/canvas/cppcanvas/drawinglayer`. `desktop` is the bootstrap.
Even the "minimum viable" subset drags in nearly the entire VCL stack
because the four apps share it. `autogen.sh` flags to disable
non-required components:
`--disable-firebird-sdbc --disable-postgresql-sdbc --disable-mariadb-connector
--disable-base --without-java --disable-report-builder
--disable-online-update --disable-extension-integration --disable-avmedia
--disable-python`. Even so, expect the build tree to weigh **15-25 GB
on disk** after a full compile.

**Compile cost (from the LibreOffice Development Blog and community
forums):**
- 24-core Intel Xeon: ~40 min from clean.
- 16-core / 32-thread / 128 GB workstation: ~4 hours (some builds
  hit link-time bottlenecks).
- ccache warm rebuild: ~5 minutes.
- Raspberry Pi 5: ~5 hours.

Practical read: on a build-farm class box this is a one-hour affair;
on typical developer hardware, hours. Not something you rebuild per
commit; you'd freeze a specific release tag as a submodule.

**LibreOfficeKit (LOK) - the embedding API.** File:
<https://raw.githubusercontent.com/LibreOffice/core/master/include/LibreOfficeKit/LibreOfficeKit.h>
The header carries the MPL 2.0 notice. Public surface (compressed):

Office-level (`LibreOfficeKitClass`):
- `documentLoad`, `documentLoadWithOptions`, `getError`, `freeError`
- `registerCallback`, `getFilterTypes`, `setOptionalFeatures`
- `setDocumentPassword`, `getVersionInfo`, `runMacro`, `signDocument`
- `runLoop`, `setOption`, `dumpState`, `extractRequest`,
  `trimMemory`, `startURP/stopURP`, `joinThreads/startThreads`
- `setForkedChild`, `registerAnyInputCallback`, `getDocsCount`,
  `registerFileSaveDialogCallback`

Document-level (`LibreOfficeKitDocumentClass`):
- Rendering (unstable API - gated by `LOK_USE_UNSTABLE_API`):
  `paintTile`, `paintPartTile`, `paintWindow`, `paintWindowDPI`,
  `paintWindowForView`, `renderFont`, `renderFontOrientation`,
  `renderShapeSelection`, `renderSearchResult`,
  `renderNextSlideLayer`, `createSlideRenderer`.
- Interaction: `postKeyEvent`, `postMouseEvent`, `postWindowKeyEvent`,
  `postWindowMouseEvent`, `postWindowGestureEvent`,
  `postWindowExtTextInputEvent`, `postUnoCommand` (executes any UNO
  command via URL - the real programmable surface),
  `resizeWindow`, `postWindow`, `postSlideshowCleanup`.
- Selection: `setTextSelection`, `getTextSelection`,
  `setGraphicSelection`, `setWindowTextSelection`, `resetSelection`,
  `getSelectionType`, `getSelectionTypeAndText`, `removeTextContext`.
- Views: `createView`, `createViewWithOptions`, `destroyView`,
  `setView`, `getView`, `getViewsCount`, `getViewIds`,
  `setViewLanguage`, `setViewTimezone`, `setViewReadOnly`,
  `setViewOption`.
- Parts (sheets/slides): `getParts`, `getPart`, `setPart`,
  `getPartName`, `getPartInfo`, `getPartHash`, `selectPart`,
  `moveSelectedParts`, `setPartMode`, `setOutlineState`.
- Save / clipboard / config / A11y / signing - see header.

Entry point is `libreofficekit_hook` in `libmergedlo.so`, wrapped by
`lok_init_2()` from `LibreOfficeKitInit.h`. That returns an
`LibreOfficeKit*` whose vtable is the class above.

**Existing embedding prior art:**
- `gtktiledviewer` - LibreOffice's in-tree reference LOK viewer
  (`libreoffice/desktop/qa/gtktiledviewer/`).
- **Collabora Online** (see §2.2) - the largest real deployment; renders
  tiles into an HTML5 canvas over WebSocket.
- LibreOffice on iOS + Android office viewers.
- Nextcloud's `richdocumentscode` desktop bundles LOK.

**Verdict - direct LOK embed into ac9.** Technically possible: link
`libmergedlo.so` + font/config data (~700 MB installed), call
`lok_init_2()`, drive it from ac9's httplib worker threads, serve
tiles as PNG or WebP over HTTP. **The engineering weight is
enormous**: LibreOffice expects its own UNO runtime rooted at a
specific `program/` directory, wants environment variables
(`URE_BOOTSTRAP`, `SAL_USE_COMMON_ONE_ACCEL`), and its threading
requirements collide with httplib's per-connection thread model unless
you funnel all LOK calls to a single owner thread. The rebuild + tile
protocol reinvention effectively means reimplementing Collabora
Online. Only pursue this if you specifically need to avoid a
subprocess boundary; otherwise §2.2 is the same code with the hard
parts already done.

---

### 2.2 Collabora Online (LibreOffice Online, MPL 2.0)

**Repo:** <https://github.com/CollaboraOnline/online> - active
development is on Gerrit; this GitHub repo is now issue-tracker + build
+ Helm chart mirror. Code mirror: <https://github.com/CollaboraOnline/online.mirror>.

**Size:** 455 MB (455,486 KB) - well within budget.

**Language mix:** C++ (core `wsd/`, `kit/`, `common/`) + TypeScript /
JavaScript (`browser/` canvas UI) + Python build helpers. GitHub
labels it "Shell" because of the enormous build scripts.

**License:** **Primarily MPL 2.0**, with some components under other
open-source licenses. README language: "Open Source - primarily under
the MPLv2 license." Compatible with ac9 GPL v3.

**Architecture (from README):**
- `wsd/` - Web Services Daemon (`coolwsd`), long-lived, listens on
  9980, handles WebSocket + WOPI HTTP.
- `kit/` - per-document `coolkit` worker; forks from `coolwsd` and
  hosts a headless LibreOffice via LOK. Isolated for security.
- `common/` - shared C++ between wsd + kit.
- `browser/` - TypeScript/JS canvas client that renders the tiles the
  kit emits, sends key/mouse events back over WebSocket.
- `engine/` - rendering glue.

Wire protocol is documented in `wsd/protocol.txt` - text lines over
WebSocket ("tile part=0 x=0 y=0 tileposx=... tileposy=... width=...
height=..."). Tiles come back as PNG binary frames.

**Runtime footprint (from CollaboraOnline/richdocumentscode issue #264
and community benchmarks):**
- Idle container: ~500 MiB RSS.
- Per additional active user: ~10 MiB.
- Per open document: 50-100 MiB depending on doc complexity.
- Recommended sizing: ~1 GB RAM headroom + 50 MB per user.
- CPU is idle when no editing occurs; peaks during initial tile paint
  and format conversion.

**Build cost:** Docker build "grows to 30 GB before shedding most of
it," "takes hours even on fast hardware." Native `.deb` / `.rpm`
packages exist for amd64/arm64/ppc64 - you almost never actually
build this from source; you ship the Collabora package.

**Integration story into ac9 (concrete):**
1. Ship the `coolwsd` binary + LibreOffice runtime as a **child process**
   spawned by `main.cpp` (analogous to how ac9 spawns model workers).
2. Add a WOPI host to `modules/010_interface/server.cpp`. The WOPI
   surface ac9 must expose is small:
   - `GET  /wopi/files/<fileid>?access_token=…` → `CheckFileInfo` JSON
     (BaseFileName, Size, OwnerId, UserId, Version, UserCanWrite…)
   - `GET  /wopi/files/<fileid>/contents` → raw bytes
   - `POST /wopi/files/<fileid>/contents` → save bytes
   These are ordinary REST endpoints; ac9's file-editor endpoints
   already do 90% of the work.
3. Front-end embeds `coolwsd` in an iframe with a POST form:
   ```html
   <form method="POST"
         action="http://localhost:9980/browser/dist/cool.html?
                 WOPISrc=http://localhost:8899/wopi/files/design.odt">
     <input type="hidden" name="access_token" value="…"/>
   </form>
   ```
   Reverse-proxy is optional - if it's fine for the browser to hit
   `localhost:9980` directly, you skip it. If not, add a
   pass-through in `server.cpp` that upgrades to WebSocket and pipes
   both directions.
4. Coverage - all four surfaces:
   - Writer (Word) ✔
   - Calc (Spreadsheet) ✔
   - Impress (Slide deck) ✔
   - **Draw (block diagrams)** ✔ - this is the whole point: Draw is
     the LibreOffice member that best matches the OpenOffice Draw
     experience the user asked for. Full connectors, snapping,
     stencils, ODG file format.

**Verdict:** The single lowest-friction way to cover all four surfaces
with acceptable license posture. Real cost is process supervision and
the ~500 MiB idle RAM. **This is the reference candidate.**

---

### 2.3 OnlyOffice DocumentServer (AGPL v3)

**Repo:** <https://github.com/ONLYOFFICE/DocumentServer>. **Size:**
~329 MB. **License:** GNU AGPL v3.

**Language mix:** Heavy JavaScript / TypeScript for editors,
Node.js + C++ for the DocumentServer daemon, native converters for
DOCX/XLSX/PPTX round-tripping.

**Included editors:** Document (Word), Spreadsheet, Presentation, Form,
PDF, and a **"Diagram Viewer"** - read-only. There is **no full Draw /
block-diagram authoring tool**. This is the disqualifier for the user's
specific ask.

**Rendering approach:** Canvas + native fonts; editors run entirely in
the browser and post OOXML-compatible ops back to Node.js server for
persistence + conversion. Feels the most like MS Office 365.

**Runtime footprint:** Community Edition docker image expects ≥ 4 GB
RAM, ships as a monolithic docker container.

**AGPL v3 vs GPL v3 compatibility.** This is the load-bearing legal
question.

From <https://www.gnu.org/licenses/gpl-faq.html>: GPL v3 and AGPL v3
have mutual §13 compatibility clauses that let you *combine* modules
under both licenses in a single project, but neither license lets you
*relicense* code from the other. In practice: ac9 can bundle
DocumentServer as a separate subprocess and combine, but the **combined
work triggers AGPL §13** - anyone running ac9 on a network-accessible
host must offer source of the OnlyOffice component (plus, arguably, of
ac9's OnlyOffice integration glue) to remote users. ac9 already ships
source under GPL, so this is closer to a documentation obligation than
a redesign - but it is a **contagion** the LibreOffice-family options
do not have.

**Verdict:** Powerful, well-polished editors, but missing the Draw
equivalent the user singled out as critical, and the AGPL adds
network-use disclosure obligations. Do not pick.

---

### 2.4 CryptPad (AGPL v3, xwiki-labs)

**Repo:** <https://github.com/xwiki-labs/cryptpad>. **Size:** ~549 MB.
**License:** AGPL v3.

**Apps included** (from the README landing page): Rich text, Sheet
(OnlyOffice-backed via the DocumentServer install script), Presentation
(also OnlyOffice-backed), Form, Kanban, Code, Whiteboard, Drive.

**Backend:** Node.js server. All storage is **end-to-end encrypted in
the browser** - server sees only ciphertext. That is a fantastic
property for a public multi-tenant deploy, but for a local single-user
box it is a pure overhead: everything already lives on the user's disk.

**Coverage:** Depends on OnlyOffice for Sheet/Presentation (same
Draw-gap problem), plus its own Whiteboard which is a rough
freehand/text tool, not block-diagram grade.

**AGPL contagion:** Same story as OnlyOffice.

**Verdict:** Wrong problem shape. CryptPad solves "encrypted
collaborative cloud office"; ac9 needs "embedded office in a local
tool." Skip.

---

### 2.5 Etherpad + EtherCalc + HedgeDoc (single-purpose collaborative)

- **Etherpad**: <https://github.com/ether/etherpad-lite> - collaborative
  plain/rich text. Apache 2.0. Great for realtime pads; not an office
  suite.
- **EtherCalc**: <https://github.com/audreyt/ethercalc> - collaborative
  spreadsheet. CPAL-1.0. Under-maintained.
- **HedgeDoc**: <https://github.com/hedgedoc/hedgedoc> - collaborative
  Markdown. AGPL v3. Overlaps ac9's existing Toast UI Markdown editor.

None of these covers slides or Draw-quality block diagrams; combining
three separate Node.js processes to get partial coverage is worse than
one Collabora subprocess.

**Verdict:** Skip.

---

### 2.6 drawio / diagrams.net (Apache 2.0) - the diagram-specific pick

**Repo:** <https://github.com/jgraph/drawio>. **Size:** 1.76 GB (most
of that is history - the actual webapp bundle is much smaller; see
below).

**License:** Apache 2.0 for source. The stencil/icon libraries carry
their own restrictions on use inside Atlassian products but are
unrestricted for use in end-user diagrams here. Apache 2.0 is
GPL-v3-compatible (per FSF).

**Language mix:** JavaScript (primary), some Java for the build/war
tooling (`etc/build`), CSS, HTML.

**Deployment.** 100% client-side - no backend at all. Diagrams live in
`localStorage` by default; File → Save As downloads a `.drawio` XML
file to disk (or the host app can persist via postMessage). The
`src/main/webapp/` directory is what you ship:

- `index.html`
- `js/app.min.js` - main editor bundle
- `js/viewer.min.js` - read-only viewer
- `mxgraph/` - mxGraph core (canvas engine)
- `stencils/` - hundreds of XML stencil files pre-compressed into a
  single `stencils.min.js` (Deflate + Base64-wrapped in a self-exec
  JS function)
- `shapes/`, `resources/`, `styles/`, `templates/`, `images/`, `img/`

Empirically the drawio webapp deployable is on the order of **10-15 MB
uncompressed**, well within what ac9 can `embed_assets.py` into the
binary. `tobyqin/drawio-local` and `i12bretro/tutorials/0197` document
how to strip Google/OneDrive/GitHub/Dropbox integration URLs so the
tool functions with **zero external network** - required for ac9.

**Embed API.** iframe + postMessage protocol at
<https://www.drawio.com/doc/faq/embed-mode>:
- Host embeds `.../index.html?embed=1&proto=json&saveAndExit=1&...`
- Editor emits `init`; host replies with `load {xml: "…"}`.
- Editor emits `save`, `autosave`, `exit`, `export`, `openLink`.
- Host can `invokeAction` for zoom / undo / layout, or `patch` for
  diff-sync collaboration.

**Diagram quality (versus Excalidraw etc.):** drawio is the accepted
open-source replacement for Visio / OO Draw:
- Enormous shape library (UML, BPMN, AWS/GCP/Azure icons,
  network/rack, floorplan, circuit).
- Formal orthogonal connectors that snap and stay attached.
- Auto-layout algorithms.
- Import/export .vsdx, .drawio, XML, PNG (with embedded XML for
  round-trip), SVG (with embedded XML), PDF.

This is the **best available block-diagram tool in the open-source
world**, and the license is friendly.

**Verdict:** For block diagrams specifically, drawio is the ceiling.
Embed as static assets under `modules/010_interface/drawio/`.

---

### 2.7 Excalidraw (MIT) - the sketchy-hand-drawn alternative

**Repo:** <https://github.com/excalidraw/excalidraw>. **Size:** ~102
MB. **License:** MIT. **Bundle:** `@excalidraw/excalidraw` npm package,
React component.

**Style:** Deliberately hand-drawn / whiteboard-doodle look. Fewer
shape libraries, no formal UML/BPMN, no snapping connectors of
drawio's quality. Great for architecture whiteboarding, wrong for
"Visio-style block diagram."

**Deploy:** React component embedded in a host - either add React to
ac9's front-end (currently vanilla JS + CodeJar) or run the standalone
`excalidraw` webapp as static assets. Self-hosting requires copying
the fonts folder from the npm package and pointing
`window.EXCALIDRAW_ASSET_PATH` at the local path (avoids the CDN font
download).

**Verdict:** Would be a good "sketchpad" companion to drawio if you
want sketching alongside formal diagrams - but doesn't replace drawio
for the user's ask. Skip unless you specifically want both.

---

### 2.8 tldraw

**Repo:** <https://github.com/tldraw/tldraw>. **Size:** 1.4 GB.
**License:** **Not open-source.** The SDK is under "the tldraw
license" - free in development, **paid license key required for
production use**. Explicitly disqualifying for ac9 (GPL-3.0-or-later).
Skip.

---

## 3. Comparison matrix

Each axis is 1-5. **License:** 5 = trivial (permissive, GPL-compat);
1 = incompatible. **Integration effort:** 5 = trivial static bundle,
1 = new heavyweight subprocess + IPC + auth plumbing (inverted - high
= easy). **Coverage:** union of the four surfaces the user named.
**Diagram quality:** specifically block-diagram-in-the-style-of-OO-Draw.
**Runtime cost:** 5 = negligible (client-side only), 1 = multi-GB
daemon. **Offline fit:** 5 = zero external calls out of the box.

| Option | License | Integration effort | Coverage | Diagram quality | Runtime cost | Offline fit |
|---|---:|---:|---:|---:|---:|---:|
| **LibreOffice core, direct LOK embed** | 5 | 1 | 5 | 5 | 2 | 5 |
| **Collabora Online (subprocess)** | 5 | 3 | 5 | 5 | 3 | 5 |
| **OnlyOffice DocumentServer** | 2 (AGPL contagion) | 3 | 3 (no Draw) | 2 (viewer only) | 2 | 4 |
| **CryptPad** | 2 (AGPL) | 2 | 3 (OO-backed) | 2 | 2 | 3 |
| **Etherpad / EtherCalc / HedgeDoc** | 4 | 3 | 2 (partial) | 1 | 3 | 4 |
| **drawio (static bundle)** | 5 | 5 | 1 (diagrams only) | 5 | 5 | 5 |
| **Excalidraw (static bundle or React)** | 5 | 4 | 1 (diagrams only) | 3 | 5 | 4 |
| **tldraw** | 1 (proprietary) | - | - | - | - | - |

Reading the matrix: no single option scores well on both **Coverage**
and **Diagram quality** except LibreOffice-family. LibreOffice as
direct-LOK is the only 5×5 combo, but the Integration effort is 1 -
months of work reimplementing what Collabora already did. Collabora
Online is 5×5 with Integration effort 3 (a couple of engineering
weeks). Everything else has a hole.

---

## 4. Hybrid combinations worth considering

### H1 - **Collabora Online only** (single subprocess)
- Covers all four surfaces including Draw.
- One thing to supervise from `main.cpp`.
- 500 MiB idle RAM (acceptable next to a 20 GB GGUF model).
- License clean (MPL 2.0).

### H2 - **Collabora Online + drawio side-by-side**
- Collabora handles docs/sheets/slides.
- drawio replaces Collabora Draw for users who prefer the Visio-style
  workflow. drawio's shape library (AWS/UML/BPMN/network) is
  substantially deeper than Draw's.
- Cost: extra static-asset bundle, ~10 MB in the ac9 binary. No new
  runtime process.
- Front-end can offer "New diagram → block diagram (drawio) / freeform
  drawing (Collabora Draw)" as a choice.

### H3 - **drawio only + (defer word/sheet/slide)**
- Fastest path to the user's headline concern ("drawing tool suited to
  block diagrams, you know the stuff open office has").
- Ships in one PR: static assets + a route in `server.cpp` +
  postMessage bridge in `app.js`.
- Punts on Writer/Calc/Impress until the user actually asks for them
  in anger.

### H4 - **OnlyOffice + drawio**
- OnlyOffice covers doc/sheet/slide with a slick UI.
- drawio covers Draw.
- Blocked by AGPL v3 contagion - any network-accessible ac9 instance
  would need to satisfy AGPL §13.

### H5 - **Direct LOK embed + drawio**
- Ambitious: no subprocess boundary.
- Realistic scope: ~2 quarters of C++ tile plumbing to catch up with
  Collabora's browser client.
- Only justified if you want to eliminate every extra daemon in
  principle.

---

## 5. What each option looks like *in ac9's actual codebase*

**Collabora Online (H1/H2)**
- New module: `modules/1720_office_suite/` (mirrors the numbering of
  the existing `1620_advanced_raster_image_editor_...`).
- New process supervisor: `office_suite::start()` in `main.cpp` after
  the model workers, launching `coolwsd --port=9981 --disable-ssl
  --allowlist=127.0.0.1 --o:storage.filesystem[@allow]=true …` as a
  child; `waitpid` in a monitor thread; SSE `event: office_paused`
  when it dies.
- New WOPI endpoints in `modules/010_interface/server.cpp`:
  `handle_wopi_check_file_info`, `handle_wopi_get_contents`,
  `handle_wopi_put_contents` - pattern-match the existing
  `handle_fs_read` / `handle_fs_write_raw`.
- Front-end: new tab in `app.js` "Office" whose iframe posts to
  `http://127.0.0.1:9981/browser/dist/cool.html?WOPISrc=…`. Auth token
  minted from the existing session store
  (`modules/010_interface/sessions_store.cpp`).
- Packaging: ship the Collabora `.deb` alongside ac9 (or bundle its
  `program/` tree under `/opt/ac9/coolwsd/`), pointed at by an env
  var. Do **not** rebuild from source in ac9's own CMake.

**drawio (H2/H3)**
- New directory: `modules/010_interface/drawio/` containing the
  stripped drawio webapp (index.html, js/app.min.js, mxgraph/,
  stencils/, shapes/, resources/, styles/, templates/, images/).
- Extend `modules/010_interface/embed_assets.py` to walk that tree -
  it already walks the sibling `.js`/`.css` assets.
- Add one route in `server.cpp`: `srv.Get(R"(/drawio/(.*))",
  serve_embedded_asset)`.
- Front-end: new tab "Diagram" whose iframe loads
  `/drawio/index.html?embed=1&proto=json&saveAndExit=0` and listens on
  postMessage. On `save`, POST XML to
  `/api/fs/write` (already exists) at the current project path.
- Add `.drawio` to the file-editor preview handlers so double-clicking
  a diagram in the file tree launches the tab.

**Direct LOK (H5)**
- Vendor LibreOffice as a submodule at a pinned release tag.
- New CMake target `office_lok` linking `libmergedlo.so`.
- Owner-thread pattern in ac9: single dedicated
  `std::thread` runs LOK's message loop; all httplib handlers post
  work via a lockfree queue.
- Implement tile HTTP endpoint that returns PNGs from
  `paintTile()`; front-end reimplements the Collabora browser
  canvas + keyboard/mouse marshaling.
- **Weeks estimate: 12-20.** Reject on cost/benefit vs H1.

---

## 6. Recommendation

**Pick H2 - Collabora Online (subprocess) + drawio (static bundle) -
and ship it in two phases.**

Reasoning:
- **License clean.** MPL 2.0 (Collabora) and Apache 2.0 (drawio) are
  both unambiguously GPL-3.0-or-later compatible. No AGPL §13
  contagion. Rules out OnlyOffice/CryptPad. Rules out tldraw.
- **Covers all four surfaces on day 2.** Collabora gives Writer /
  Calc / Impress out of the box, with correct ODF and OOXML round
  trip.
- **Best-in-class diagrams.** drawio outperforms Collabora Draw for
  the Visio-style block diagrams the user called out. Free bonus that
  it also runs 100% client-side, adds ~10 MB to the ac9 binary, and
  requires zero process supervision.
- **Fits ac9's shape.** Collabora becomes the fourth process
  main.cpp supervises (llama.cpp workers, sd.cpp, ac9 itself,
  coolwsd) - same pattern already in use. drawio becomes another
  embedded asset tree - same pattern already in use for Toast UI /
  CodeJar / xterm.
- **No external network required.** Both function fully offline once
  installed; drawio's cloud integrations can be stripped by removing
  the Google/OneDrive/GitHub/Dropbox HTML shims from the copied
  webapp.
- **Escape hatches preserved.** Because both are process- or asset-
  isolated, either can be swapped out later (e.g., for a future
  LOK-native implementation) without an ac9 rewrite.

**Engineering-weeks estimate for injection into ac9:**

| Phase | Scope | Weeks |
|---|---|---:|
| 1a | drawio static-asset bundle + `/drawio/*` route + `app.js` tab + postMessage save/load + `.drawio` file-editor preview | 1 |
| 1b | Strip drawio's external cloud shims; verify with the machine offline | 0.5 |
| 2a | Package Collabora coolwsd (choose: ship the Debian package as an ac9 dependency, or vendor the `/opt/collaboraoffice/` tree under a submodule build script) | 1.5 |
| 2b | `main.cpp` supervisor + graceful start/stop + SSE `office_paused` event mirroring the existing `run_paused` pattern | 1 |
| 2c | WOPI CheckFileInfo / GetFile / PutFile endpoints in `server.cpp` + session-token integration with `sessions_store.cpp` | 1.5 |
| 2d | Front-end Office tab, file-editor open-with hook for `.odt/.ods/.odp/.docx/.xlsx/.pptx` | 1 |
| 2e | Optional httplib reverse-proxy for WebSocket to avoid exposing 9981 to the browser | 1 |
| 3 | Test matrix, licensing/notices in `NOTICE-KICAD.md`-style file, `data/manifest.json` updates | 0.5 |
| - | **Total** | **~8 weeks** for one engineer, ~4 with two engineers running phases 1 and 2 in parallel |

**Phase 1 (drawio only) delivers on the user's headline ask - a
Draw-quality block-diagram tool - in a single week of work.** Phase 2
brings the Writer/Calc/Impress surface online over the following
month or two. That ordering matches the tenor of the user's message
("drawing tool specifically suited to making block diagrams" appears
in the same sentence as "critically" - do that first, do the rest
after).

---

## 7. Sources

Repositories:
- LibreOffice core: <https://github.com/LibreOffice/core>
- LibreOfficeKit header:
  <https://raw.githubusercontent.com/LibreOffice/core/master/include/LibreOfficeKit/LibreOfficeKit.h>
- Collabora Online: <https://github.com/CollaboraOnline/online>
- Collabora Online mirror: <https://github.com/CollaboraOnline/online.mirror>
- OnlyOffice DocumentServer: <https://github.com/ONLYOFFICE/DocumentServer>
- CryptPad: <https://github.com/xwiki-labs/cryptpad>
- drawio: <https://github.com/jgraph/drawio>
- Excalidraw: <https://github.com/excalidraw/excalidraw>
- tldraw: <https://github.com/tldraw/tldraw>

Docs / integration references:
- LibreOffice license breakdown: <https://api.libreoffice.org/share/readme/LICENSE.html>
- drawio embed-mode postMessage: <https://www.drawio.com/doc/faq/embed-mode>
- Collabora Online FOSDEM 2020 integration slides:
  <https://archive.fosdem.org/2020/schedule/event/integrate_collabora_online_with_web_applications/>
- Collabora Online FOSDEM 2020 "Bringing Collabora Online to your web-app":
  <https://archive.fosdem.org/2020/schedule/event/bringing_collabora_online_webapp/>
- Excalidraw integration docs: <https://docs.excalidraw.com/docs/@excalidraw/excalidraw/integration>
- OnlyOffice Docker system requirements:
  <https://helpcenter.onlyoffice.com/docs/installation/docs-community-sys-reqs-docker.aspx>

Legal / license compatibility:
- GNU GPL FAQ (GPL v3 ↔ AGPL v3): <https://www.gnu.org/licenses/gpl-faq.html>
- FSF license list (MPL 2.0 GPL compatibility):
  <https://www.gnu.org/licenses/license-list.en.html>

Runtime footprint / performance notes:
- Collabora RAM issue (real measurements):
  <https://github.com/CollaboraOnline/richdocumentscode/issues/264>
- Collabora Online metrics/protocol:
  <https://github.com/CollaboraOnline/online/blob/master/wsd/metrics.txt>
- LibreOffice ccache build time blog:
  <https://dev.blog.documentfoundation.org/2023/07/30/ccache-for-a-5-minutes-libreoffice-build/>
- drawio self-host guides: <https://selfhosting.sh/apps/drawio/>,
  <https://github.com/tobyqin/drawio-local>

---

*Written for the ac9 supervising agent by the research pass. All URLs
verified reachable at report time. No ac9 project files outside this
scratchpad were modified.*
