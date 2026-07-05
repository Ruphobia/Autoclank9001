// SPDX-License-Identifier: GPL-3.0-or-later
// Editor coordinator. Wires:
//   * canvas.Scene
//   * sch_renderer.SchematicRenderer or pcb_renderer.PcbRenderer
//   * tools.* tool state machine
//   * inspector.Inspector
//   * menus.MenuBar
// against the /api/eda/editor/* backend registered by
// modules/353_editor_session/editor_routes.cpp.
//
// Mount:
//   const editor = new EDA_Editor.Editor(rootEl, { sessionId, mode });
//   editor.attach();

(function () {
    'use strict';

    async function apiCall(path, opts) {
        opts = opts || {};
        const init = { method: opts.method || 'GET', headers: {} };
        if (opts.body !== undefined) {
            init.headers['Content-Type'] = 'application/json';
            init.body = typeof opts.body === 'string' ? opts.body : JSON.stringify(opts.body);
        }
        const r = await fetch(path, init);
        const j = await r.json().catch(() => ({ ok: false, error: 'bad JSON' }));
        if (!r.ok || j.ok === false) throw new Error(j.error || 'HTTP ' + r.status);
        return j;
    }

    class Editor {
        constructor(root, opts) {
            opts = opts || {};
            this.sessionId = opts.sessionId || 'default';
            this._mode = opts.mode || 'sch';   // 'sch' | 'pcb'
            this.root = root;
            this.remoteVersion = 0;

            // Build the shell.
            root.classList.add('eda-editor');
            root.innerHTML = `
                <div class="eda-editor-menubar" id="eda-editor-menubar"></div>
                <div class="eda-editor-body">
                    <div class="eda-editor-canvas" id="eda-editor-canvas" tabindex="0"></div>
                    <aside class="eda-editor-inspector" id="eda-editor-inspector"></aside>
                </div>
                <div class="eda-editor-statusbar" id="eda-editor-statusbar">
                    <span class="eda-status-mode">SCH</span>
                    <span class="eda-status-tool">select</span>
                    <span class="eda-status-cursor">(0.000, 0.000) mm</span>
                    <span class="eda-status-version">v0</span>
                </div>
            `;

            const canvasEl    = root.querySelector('#eda-editor-canvas');
            const inspectorEl = root.querySelector('#eda-editor-inspector');
            const menubarEl   = root.querySelector('#eda-editor-menubar');
            this.statusMode   = root.querySelector('.eda-status-mode');
            this.statusTool   = root.querySelector('.eda-status-tool');
            this.statusCursor = root.querySelector('.eda-status-cursor');
            this.statusVersion= root.querySelector('.eda-status-version');

            // Scene + renderers.
            this.scene = new EDA_Canvas.Scene(canvasEl);
            this.schRenderer = new EDA_Sch.SchematicRenderer(this.scene);
            this.pcbRenderer = new EDA_Pcb.PcbRenderer(this.scene);
            this.scene.schRenderer = this.schRenderer;
            this.scene.pcbRenderer = this.pcbRenderer;
            this.scene.kind = this._mode;

            // Inspector + menus.
            this.inspector = new EDA_Inspector.Inspector(inspectorEl, this._api());
            this.menubar   = new EDA_Menus.MenuBar(menubarEl,       this._api());

            // Track cursor.
            canvasEl.addEventListener('mousemove', ev => {
                const rect = canvasEl.getBoundingClientRect();
                const px = ev.clientX - rect.left, py = ev.clientY - rect.top;
                const wx = this.scene.view.toWorldX(px), wy = this.scene.view.toWorldY(py);
                this.statusCursor.textContent = `(${(wx/1e6).toFixed(3)}, ${(wy/1e6).toFixed(3)}) mm`;
            });

            // Default tool.
            this._toolName = 'select';
            this._applyTool();
            this._refreshStatusMode();
        }

        // Public API surface used by tools / menus / inspector.
        _api() {
            const E = this;
            return {
                // Mode.
                mode:        () => E._mode,
                toggleMode:  () => E._toggleMode(),

                // Tool switching.
                setTool:     (name) => E.setTool(name),

                // Selection.
                setSelection: (sel) => E._sendSelection(sel),
                selectAll:    () => E._selectAll(),
                deleteSelected: () => E._deleteSelected(),
                rotateSelected: (deg) => E._rotateSelected(deg),

                // Item ops.
                addSchItem:  (it) => E._add('sch', it),
                addPcbItem:  (it) => E._add('pcb', it),
                move:        (o)  => E._move(o.kind || E._mode, o.uuid, o.dx_mm, o.dy_mm),
                rotate:      (o)  => E._rotate(o.kind || E._mode, o.uuid, o.deg),
                remove:      (o)  => E._remove(o.kind || E._mode, o.uuid),
                editField:   (o)  => E._editField(o.uuid, o.field, o.value),

                // File.
                newSchematic: () => E._newSchematic(),
                open:         () => E._openDialog(),
                save:         () => E._save(),
                saveAs:       () => E._saveAs(),

                // Fab.
                exportKind:   (kind) => E._exportKind(kind),

                // Analysis.
                runErc:        () => E._runErc(),
                runDrc:        () => E._runDrc(),
                deriveNetlist: () => E._deriveNetlist(),

                // Undo/Redo.
                undo:  () => E._undo(),
                redo:  () => E._redo(),

                // View.
                fit:         () => E.scene.view.fitWorld(E._modelBBox()),
                toggleGrid:  () => { E.scene.gridVisible = !E.scene.gridVisible; E.scene.render(); }
            };
        }

        setTool(name) {
            this._toolName = name;
            this._applyTool();
            this.statusTool.textContent = name;
        }

        _applyTool() {
            const A = this._api();
            let tool;
            switch (this._toolName) {
                case 'select':          tool = new EDA_Tools.SelectTool(A);              break;
                case 'place_symbol':    tool = new EDA_Tools.PlaceSymbolTool(A);         break;
                case 'wire':            tool = new EDA_Tools.DrawWireTool(A);            break;
                case 'label':           tool = new EDA_Tools.DrawLabelTool(A, 'label');  break;
                case 'global_label':    tool = new EDA_Tools.DrawLabelTool(A, 'global_label'); break;
                case 'hier_label':      tool = new EDA_Tools.DrawLabelTool(A, 'hier_label'); break;
                case 'junction':        tool = new EDA_Tools.DrawJunctionTool(A);        break;
                case 'no_connect':      tool = new EDA_Tools.DrawNoConnectTool(A);       break;
                case 'place_footprint': tool = new EDA_Tools.PlaceFootprintTool(A);      break;
                case 'track':           tool = new EDA_Tools.DrawTrackTool(A);           break;
                case 'via':             tool = new EDA_Tools.DrawViaTool(A);             break;
                default:                tool = new EDA_Tools.SelectTool(A);              break;
            }
            this.scene.setTool(tool);
        }

        _toggleMode() {
            this._mode = this._mode === 'sch' ? 'pcb' : 'sch';
            this.scene.kind = this._mode;
            this._refreshStatusMode();
            this._reloadModel();
        }
        _refreshStatusMode() {
            this.statusMode.textContent = this._mode === 'pcb' ? 'PCB' : 'SCH';
            this.root.classList.toggle('eda-mode-pcb', this._mode === 'pcb');
            this.root.classList.toggle('eda-mode-sch', this._mode === 'sch');
        }

        // Send POST /api/eda/editor/*.
        async _post(path, body) {
            body = Object.assign({ session_id: this.sessionId }, body);
            try {
                const j = await apiCall('/api/eda/editor/' + path, { method:'POST', body });
                if (j.version) { this.remoteVersion = j.version; this.statusVersion.textContent = 'v' + j.version; }
                return j;
            } catch (e) {
                console.error(path, e);
                throw e;
            }
        }

        async _add(kind, item)    { const j = await this._post('add',   { kind, item });         await this._reloadModel(); return j; }
        async _remove(kind, uuid) { const j = await this._post('remove',{ kind, uuid });         await this._reloadModel(); return j; }
        async _move(kind, uuid, dx_mm, dy_mm) { const j = await this._post('move', { kind, uuid, dx_mm, dy_mm }); await this._reloadModel(); return j; }
        async _rotate(kind, uuid, deg) { const j = await this._post('rotate', { kind, uuid, deg }); await this._reloadModel(); return j; }
        async _editField(uuid, field, value) { const j = await this._post('edit_field', { uuid, field, value }); await this._reloadModel(); return j; }
        async _undo() { const j = await this._post('undo', {}); await this._reloadModel(); return j; }
        async _redo() { const j = await this._post('redo', {}); await this._reloadModel(); return j; }
        async _sendSelection(sel) {
            const s = { sch: sel.sch || [], pcb: sel.pcb || [] };
            await this._post('selection', s);
            this.inspector.setSelection((s.sch || []).concat(s.pcb || []));
        }

        _selectAll() {
            const model = this._mode === 'sch' ? this._schModel : this._pcbModel;
            if (!model) return;
            const arr = ((this._mode === 'sch' && model.root && model.root.items) || model.items || [])
                .map(it => it.uuid).filter(Boolean);
            this._sendSelection(this._mode === 'sch' ? { sch: arr } : { pcb: arr });
            if (this._mode === 'sch') this.schRenderer.setSelection(arr);
            else                      this.pcbRenderer.setSelection(arr);
        }
        async _deleteSelected() {
            const items = this._mode === 'sch'
                ? Array.from(this.schRenderer.selected)
                : Array.from(this.pcbRenderer.selected);
            for (const u of items) await this._remove(this._mode, u);
        }
        async _rotateSelected(deg) {
            const items = this._mode === 'sch'
                ? Array.from(this.schRenderer.selected)
                : Array.from(this.pcbRenderer.selected);
            for (const u of items) await this._rotate(this._mode, u, deg);
        }

        async _newSchematic() {
            // Reset session by loading an empty schematic template.
            const empty = `(kicad_sch (version 20250114) (generator "ac9") (uuid "00000000-0000-0000-0000-000000000000") (paper "A4") (lib_symbols) (sheet_instances (path "/" (page "1"))) (embedded_fonts no))`;
            await this._post('load', { kind: 'sch', text: empty });
            await this._reloadModel();
        }
        async _openDialog() {
            const path = prompt('Path to .kicad_sch or .kicad_pcb:');
            if (!path) return;
            const kind = /\.kicad_sch$/i.test(path) ? 'sch' : 'pcb';
            await this._post('load', { kind, path });
            await this._reloadModel();
        }
        async _save()  {
            const j = await this._post('save', { kind: this._mode });
            if (j.text) {
                // Serve as a download.
                const blob = new Blob([j.text], { type: 'text/plain' });
                const url = URL.createObjectURL(blob);
                const a = document.createElement('a');
                a.href = url;
                a.download = 'board.' + (this._mode === 'sch' ? 'kicad_sch' : 'kicad_pcb');
                a.click();
                URL.revokeObjectURL(url);
            }
        }
        async _saveAs() {
            const path = prompt('Save to path:');
            if (!path) return;
            await this._post('save', { kind: this._mode, path });
        }

        async _exportKind(kind) {
            const map = { gerbers: { kind:'gerber', layer:'F.Cu' }, drill: { kind:'drill_pth' }, pos: { kind:'pos' } };
            const args = map[kind];
            if (!args) return;
            const j = await this._post('write_fab', args);
            if (!j.text) return;
            const blob = new Blob([j.text], { type:'text/plain' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = kind === 'gerbers' ? 'F_Cu.gbr'
                       : kind === 'drill'   ? 'board.drl'
                       : 'pos.csv';
            a.click();
            URL.revokeObjectURL(url);
        }

        async _runErc() {
            const j = await this._post('run_erc', {});
            alert('ERC: ' + j.errors + ' errors, ' + j.warnings + ' warnings.');
        }
        async _runDrc() {
            const j = await this._post('run_drc', {});
            alert('DRC: ' + j.errors + ' errors, ' + j.warnings + ' warnings.');
        }
        async _deriveNetlist() {
            const j = await this._post('derive_netlist', { format: 'kicadsexpr' });
            const blob = new Blob([j.text || ''], { type:'text/plain' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url; a.download = 'netlist.net'; a.click();
            URL.revokeObjectURL(url);
        }

        _modelBBox() {
            if (this._mode === 'sch' && this.schRenderer) return this.schRenderer.modelBBox();
            return null;
        }

        // Attach: get initial state, load current model, start SSE poll.
        async attach() {
            await this._reloadModel();
            this._startEvents();
        }

        async _reloadModel() {
            // Ask the server to serialize the current session doc; we
            // then parse it client-side. For MVP we shortcut: the
            // server-side state endpoint returns counts; for the actual
            // drawable items we'd need a JSON-model endpoint (follow-up).
            // Until then, we build a tiny snapshot from state + a local
            // scratch cache in this._schModel/_pcbModel maintained by
            // the tools' add() calls.
            const j = await this._post('state', {});
            this.statusVersion.textContent = 'v' + (j.version || 0);
            // Set renderer models from local cache if we have one; else empty.
            if (this._mode === 'sch') {
                this.schRenderer.setModel(this._schModel || { root: { items: [] } });
            } else {
                this.pcbRenderer.setModel(this._pcbModel || { items: [] });
            }
            this.inspector.setModel(this._mode === 'sch' ? this._schModel : { root: { items: this._pcbModel && this._pcbModel.items }});
        }

        _startEvents() {
            // Long-poll fallback for MVP: re-check state every 1s.
            if (this._pollTimer) clearInterval(this._pollTimer);
            this._pollTimer = setInterval(async () => {
                try {
                    const j = await apiCall('/api/eda/editor/state', { method:'POST', body: { session_id: this.sessionId } });
                    if (j.version && j.version !== this.remoteVersion) {
                        this.remoteVersion = j.version;
                        this.statusVersion.textContent = 'v' + j.version;
                    }
                } catch (_) {}
            }, 1000);
        }
    }

    window.EDA_Editor = { Editor };
})();
