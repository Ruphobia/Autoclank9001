# EDA (KiCad) frontend

This subdirectory holds the client-side pieces of the KiCad integration.
Kept separate from the main interface files (`app.js`, `app.css`,
`index.html`) so it can land without conflicting with in-flight work
there.

Files

- `eda.css` — styles for the EDA panels and viewers.
- `eda.js` — client-side controller: sidebar (symbol / footprint search),
  schematic viewer, PCB layered viewer, gerber preview, DRC/ERC list.
- `eda_panels.html` — the HTML fragments that get injected into the app
  shell (schematic tab, pcb tab, gerber tab, sidebar).

Wiring in (three changes when you're ready)

1. `modules/010_interface/index.html`
   Add a script tag for `eda.js` and a link tag for `eda.css` next to
   the existing app assets. Add one `<div id="eda-root"></div>` where
   you want the EDA panels to hang (typically inside the current
   tab-host container).

2. `modules/010_interface/app.js`
   After the app's own boot, call `window.EDA.mount(document.getElementById('eda-root'))`.
   When a chat message references a `.kicad_sch` or `.kicad_pcb` file
   path, call `window.EDA.openFile(path)` instead of the generic text
   editor.

3. `modules/010_interface/server.cpp`
   In the route-setup section, include and call:
   ```cpp
   #include "../340_kicad_bridge/eda_routes.hpp"
   ...
   eda_routes::register_all(app);
   ```

4. `modules/010_interface/embed_assets.py`
   If the interface uses baked-in-binary assets, extend the asset list
   to include `eda/*.js`, `eda/*.css`, `eda/*.html`. Otherwise the
   server can serve them directly from disk via the existing static
   file route.

Backend endpoints

`eda.js` calls these routes (all provided by
`340_kicad_bridge/eda_routes.cpp`):

- `GET  /api/eda/kicad_status`
- `GET  /api/eda/symbol_search?q=...&limit=...`
- `GET  /api/eda/footprint_search?q=...&limit=...`
- `GET  /api/eda/symbol_svg?lib=...&name=...`
- `POST /api/eda/emit_project`      { session_id, title }
- `POST /api/eda/emit_schematic`    { session_id, title, intent }
- `POST /api/eda/emit_pcb`          { session_id, title, intent }
- `POST /api/eda/erc`               { session_id, sch_path }
- `POST /api/eda/drc`               { session_id, pcb_path, schematic_parity? }
- `POST /api/eda/netlist`           { session_id, sch_path, format }
- `POST /api/eda/export`            { session_id, pcb_path, kind: gerbers|drill|step|svg|pos }
- `POST /api/eda/render_layers`     { session_id, pcb_path }
- `POST /api/eda/bundle_fab`        { session_id, pcb_path }
- `POST /api/eda/spice_run`         { netlist, analysis }
- `GET  /api/eda/file?path=...`     passthrough for artifacts under /tmp/tool_eda
