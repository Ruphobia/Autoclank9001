// SPDX-License-Identifier: GPL-3.0-or-later
// Client-side scaffolds for the symbol editor + footprint editor.
// Mount either against an EDA_Canvas.Scene the same way the schematic
// editor does; the difference is the current document is a single
// LibSymbol or Footprint being edited, not the whole Schematic/Board.
//
// The MVP UI is deliberately thin: it presents pin/pad add/edit tools
// and a save-back-to-session button. Full drawing-tool parity with the
// schematic editor lands as follow-up.

(function () {
    'use strict';

    const NS = 'http://www.w3.org/2000/svg';

    // ---------------- Symbol Editor ----------------

    class SymbolEditor {
        constructor(root, opts) {
            opts = opts || {};
            this.root = root;
            this.sessionId = opts.sessionId || 'default';
            this.libId     = opts.libId     || 'New:Symbol';
            this.pins      = [];
            this.fields    = [
                { name: 'Reference', value: 'U' },
                { name: 'Value',     value: this.libId.split(':').pop() }
            ];

            root.classList.add('eda-lib-editor');
            root.innerHTML = `
                <div class="eda-editor-menubar" id="sym-menubar"></div>
                <div class="eda-editor-body">
                    <div class="eda-editor-canvas" id="sym-canvas" tabindex="0"></div>
                    <aside class="eda-editor-inspector" id="sym-inspector"></aside>
                </div>
                <div class="eda-editor-statusbar">
                    <span class="eda-status-mode">SYMBOL</span>
                    <span class="eda-status-tool">draw pins</span>
                    <span class="eda-status-cursor">(0, 0) mm</span>
                </div>
            `;
            this.scene = new EDA_Canvas.Scene(root.querySelector('#sym-canvas'));
            this.scene.snap = 2_540_000;   // 100 mil, KiCad symbol grid
            const symApi = this._api();
            this.scene.setTool(new PinTool(symApi, this));
            this._renderInspector(root.querySelector('#sym-inspector'));
            this._drawSelf();
        }
        _api() {
            const E = this;
            return {
                addPin: (pin) => { pin.uuid = crypto.randomUUID(); E.pins.push(pin); E._drawSelf(); },
                setTool: (name) => { E.scene.setTool(new PinTool(this._api(), E)); }
            };
        }
        _drawSelf() {
            this.scene.clear();
            this.scene.add({
                drawSvg: (svg, view) => {
                    // Body box (half the extent of the outer pins).
                    let maxX = 5_000_000, maxY = 5_000_000;
                    for (const p of this.pins) {
                        maxX = Math.max(maxX, Math.abs(p.at.x) + 2_540_000);
                        maxY = Math.max(maxY, Math.abs(p.at.y) + 2_540_000);
                    }
                    const r = document.createElementNS(NS, 'rect');
                    r.setAttribute('x', view.toScreenX(-maxX));
                    r.setAttribute('y', view.toScreenY(-maxY));
                    r.setAttribute('width',  view.toScreenX(maxX) - view.toScreenX(-maxX));
                    r.setAttribute('height', view.toScreenY(maxY) - view.toScreenY(-maxY));
                    r.setAttribute('fill', '#fffbe6');
                    r.setAttribute('stroke', '#800000');
                    r.setAttribute('stroke-width', 1.5);
                    svg.appendChild(r);
                    for (const p of this.pins) {
                        const cx = view.toScreenX(p.at.x), cy = view.toScreenY(p.at.y);
                        const c = document.createElementNS(NS, 'circle');
                        c.setAttribute('cx', cx); c.setAttribute('cy', cy); c.setAttribute('r', 3);
                        c.setAttribute('fill', '#008484');
                        svg.appendChild(c);
                        const t = document.createElementNS(NS, 'text');
                        t.setAttribute('x', cx + 6); t.setAttribute('y', cy + 3);
                        t.setAttribute('font-size', 10);
                        t.setAttribute('fill', '#d7dae0');
                        t.textContent = `${p.name || '~'}#${p.number}`;
                        svg.appendChild(t);
                    }
                },
                hitTest: () => null
            });
        }
        _renderInspector(el) {
            el.innerHTML = `
                <div class="eda-inspector-header">Symbol <span class="uuid">${this.libId}</span></div>
                ${this.fields.map(f => `
                    <div class="eda-inspector-row">
                        <span class="eda-key">${f.name}</span>
                        <input type="text" value="${f.value}" data-field="${f.name}">
                    </div>`).join('')}
                <div class="eda-inspector-section">Pins (${this.pins.length})</div>
                <div class="eda-inspector-empty">Click canvas to add pins.</div>
                <div class="eda-inspector-add"><button data-save>Save to session</button></div>
            `;
            el.addEventListener('change', ev => {
                if (ev.target.dataset && ev.target.dataset.field) {
                    for (const f of this.fields) if (f.name === ev.target.dataset.field) f.value = ev.target.value;
                }
            });
            el.querySelector('[data-save]').addEventListener('click', async () => {
                await fetch('/api/eda/editor/lib_symbol_save', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({
                        session_id: this.sessionId,
                        lib_id: this.libId,
                        fields: this.fields,
                        pins:   this.pins.map(p => ({
                            number: p.number, name: p.name,
                            electrical: p.electrical || 'passive',
                            at_mm: [p.at.x / 1e6, p.at.y / 1e6],
                            length_mm: 2.54
                        }))
                    })
                });
                alert('Saved to session lib_symbols.');
            });
        }
    }

    class PinTool {
        constructor(api, ed) { this.api = api; this.ed = ed; this.n = ed.pins.length + 1; }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            const number = String(this.n++);
            const name = prompt(`Pin ${number} name (default "~"):`, '~') || '~';
            const electrical = prompt(`Pin ${number} type (input/output/passive/power_in/power_out):`, 'passive') || 'passive';
            this.api.addPin({ number, name, electrical, at: { x: gx, y: gy } });
        }
        onKey(scene, ev) { if (ev.key === 'Escape') { /* stay */ } }
    }

    // ---------------- Footprint Editor ----------------

    class FootprintEditor {
        constructor(root, opts) {
            opts = opts || {};
            this.root = root;
            this.sessionId = opts.sessionId || 'default';
            this.libId     = opts.libId     || 'New:Footprint';
            this.pads      = [];

            root.classList.add('eda-lib-editor');
            root.innerHTML = `
                <div class="eda-editor-menubar" id="fp-menubar"></div>
                <div class="eda-editor-body">
                    <div class="eda-editor-canvas" id="fp-canvas" tabindex="0"></div>
                    <aside class="eda-editor-inspector" id="fp-inspector"></aside>
                </div>
                <div class="eda-editor-statusbar">
                    <span class="eda-status-mode">FOOTPRINT</span>
                    <span class="eda-status-tool">draw pads</span>
                </div>
            `;
            this.scene = new EDA_Canvas.Scene(root.querySelector('#fp-canvas'));
            this.scene.snap = 254_000;   // 0.254 mm typical
            this.scene.setTool(new PadTool(this._api(), this));
            this._renderInspector(root.querySelector('#fp-inspector'));
            this._drawSelf();
        }
        _api() {
            const E = this;
            return {
                addPad: (pad) => { pad.uuid = crypto.randomUUID(); E.pads.push(pad); E._drawSelf(); }
            };
        }
        _drawSelf() {
            this.scene.clear();
            this.scene.add({
                drawSvg: (svg, view) => {
                    for (const pad of this.pads) {
                        const cx = view.toScreenX(pad.at.x), cy = view.toScreenY(pad.at.y);
                        const w = pad.size.x * view.scale, h = pad.size.y * view.scale;
                        const r = document.createElementNS(NS, 'rect');
                        r.setAttribute('x', cx - w/2); r.setAttribute('y', cy - h/2);
                        r.setAttribute('width', w);   r.setAttribute('height', h);
                        r.setAttribute('fill', '#c88a17'); svg.appendChild(r);
                        const t = document.createElementNS(NS, 'text');
                        t.setAttribute('x', cx); t.setAttribute('y', cy + 3);
                        t.setAttribute('text-anchor', 'middle');
                        t.setAttribute('font-size', 9);
                        t.setAttribute('fill', '#000');
                        t.textContent = pad.number;
                        svg.appendChild(t);
                    }
                },
                hitTest: () => null
            });
        }
        _renderInspector(el) {
            el.innerHTML = `
                <div class="eda-inspector-header">Footprint <span class="uuid">${this.libId}</span></div>
                <div class="eda-inspector-section">Pads (${this.pads.length})</div>
                <div class="eda-inspector-add"><button data-save>Save to session</button></div>
            `;
            el.querySelector('[data-save]').addEventListener('click', async () => {
                await fetch('/api/eda/editor/footprint_save', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({
                        session_id: this.sessionId,
                        lib_id: this.libId,
                        pads: this.pads.map(p => ({
                            number: p.number,
                            kind:   p.kind || 'smd',
                            shape:  p.shape || 'rect',
                            at_mm:  [p.at.x / 1e6, p.at.y / 1e6],
                            size_mm:[p.size.x / 1e6, p.size.y / 1e6],
                            layers: ['F.Cu','F.Mask','F.Paste']
                        }))
                    })
                });
                alert('Saved to session footprints.');
            });
        }
    }

    class PadTool {
        constructor(api, ed) { this.api = api; this.ed = ed; this.n = ed.pads.length + 1; }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            const w = parseFloat(prompt('Pad width (mm):', '1.0')) * 1e6;
            const h = parseFloat(prompt('Pad height (mm):', '0.6')) * 1e6;
            if (isNaN(w) || isNaN(h)) return;
            this.api.addPad({
                number: String(this.n++),
                kind: 'smd', shape: 'rect',
                at: { x: gx, y: gy },
                size: { x: w, y: h }
            });
        }
    }

    // ---------------- Library Manager ----------------

    class LibraryManager {
        constructor(root) {
            root.classList.add('eda-lib-manager');
            root.innerHTML = `
                <h3>Symbol libraries</h3>
                <ul id="lm-sym-list"></ul>
                <h3>Footprint libraries</h3>
                <ul id="lm-fp-list"></ul>
                <div class="eda-inspector-add">
                    <button data-refresh>Refresh</button>
                </div>
            `;
            root.querySelector('[data-refresh]').addEventListener('click', () => this._refresh());
            this._refresh();
        }
        async _refresh() {
            const j = await fetch('/api/eda/kicad_status').then(r => r.json()).catch(() => ({}));
            // MVP: just show counts. Full pin/unpin UI needs more backend surface.
            this.root.querySelector('#lm-sym-list').innerHTML =
                `<li>${(j.symbol_libs || 0)} symbol libs indexed</li>`;
            this.root.querySelector('#lm-fp-list').innerHTML =
                `<li>${(j.footprint_libs || 0)} footprint libs indexed</li>`;
        }
    }

    window.EDA_LibEditor = { SymbolEditor, FootprintEditor, LibraryManager };
})();
