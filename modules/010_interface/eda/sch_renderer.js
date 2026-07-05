// SPDX-License-Identifier: GPL-3.0-or-later
// Schematic SVG renderer.
// Consumes a JSON model matching the shape produced by
// /api/eda/editor/state + a per-session `sch_json` fetched via
// /api/eda/editor/save?text=1. In the MVP the client re-fetches the
// full schematic after every mutation and re-renders. Delta rendering
// is a follow-up.

(function () {
    'use strict';

    // Colors match KiCad's default schematic theme.
    const COLORS = {
        wire:            '#008484',
        bus:             '#008080',
        junction:        '#008484',
        noConnect:       '#0000aa',
        localLabel:      '#009900',
        globalLabel:     '#ff9500',
        hierLabel:       '#a020f0',
        symbolBody:      '#800000',
        symbolFill:      '#fffbe6',
        text:            '#eeeeee',
        selection:       '#ffcc00'
    };

    // Simple linear-scale conversion helpers using a passed View.
    function sx(view, x) { return view.toScreenX(x); }
    function sy(view, y) { return view.toScreenY(y); }

    class SchematicRenderer {
        constructor(scene) {
            this.scene = scene;
            this.model = { root: { items: [] }, lib_symbols: {} };
            this.selected = new Set();
            // Attach as a scene drawable.
            scene.add({
                drawSvg: (svg, view, ns) => this._draw(svg, view, ns),
                hitTest: (wx, wy) => this._hitTest(wx, wy)
            });
        }

        setModel(schJson) {
            // schJson is the parsed .kicad_sch we get back from
            // /api/eda/editor/save?text=1 (server serializes on demand).
            // For MVP we simulate a light client-side JSON view by
            // reading /api/eda/editor/state summary + fetching each item
            // via a to-be-added endpoint. Until that endpoint exists,
            // the caller may pass a hand-shaped { items, lib_symbols }.
            this.model = schJson || { root: { items: [] }, lib_symbols: {} };
            if (this.model.items && !this.model.root) {
                // Accept a flat items[] shape too.
                this.model = { root: { items: this.model.items }, lib_symbols: this.model.lib_symbols || {} };
            }
            this.scene.render();
        }

        setSelection(uuidSet) {
            this.selected = new Set(uuidSet);
            this.scene.render();
        }

        modelBBox() {
            const pts = [];
            for (const it of (this.model.root && this.model.root.items) || []) {
                if (it.at) pts.push({ x: it.at.x, y: it.at.y });
                if (it.pts) for (const p of it.pts) pts.push(p);
            }
            return window.EDA_Canvas.bboxFromPoints(pts);
        }

        _draw(svg, view, ns) {
            const items = (this.model.root && this.model.root.items) || [];
            const g = document.createElementNS(ns, 'g');
            g.setAttribute('class', 'eda-sch');
            for (const it of items) this._drawItem(g, ns, view, it);
            svg.appendChild(g);
        }

        _drawItem(g, ns, view, it) {
            const isSel = it.uuid && this.selected.has(it.uuid);
            switch (it.kind || it.type || (it.lib_id ? 'symbol' : it.text !== undefined ? 'label' : 'wire')) {
                case 'symbol':     this._drawSymbol   (g, ns, view, it, isSel); break;
                case 'wire':       this._drawWire     (g, ns, view, it, isSel); break;
                case 'bus':        this._drawWire     (g, ns, view, it, isSel, COLORS.bus); break;
                case 'junction':   this._drawJunction (g, ns, view, it, isSel); break;
                case 'no_connect': this._drawNoConnect(g, ns, view, it, isSel); break;
                case 'label':
                case 'global_label':
                case 'hier_label': this._drawLabel    (g, ns, view, it, isSel); break;
                case 'text':       this._drawText     (g, ns, view, it, isSel); break;
                default: break;
            }
        }

        _drawSymbol(g, ns, view, sym, sel) {
            // The symbol box (placeholder graphic; per-lib_symbol drawing
            // will follow when we ship the lib_symbol parser client side).
            const cx = sx(view, sym.at.x);
            const cy = sy(view, sym.at.y);
            const half = 12 * (view.scale * 1e6);
            const rect = document.createElementNS(ns, 'rect');
            rect.setAttribute('x', cx - half); rect.setAttribute('y', cy - half);
            rect.setAttribute('width',  2 * half);
            rect.setAttribute('height', 2 * half);
            rect.setAttribute('fill',   COLORS.symbolFill);
            rect.setAttribute('stroke', sel ? COLORS.selection : COLORS.symbolBody);
            rect.setAttribute('stroke-width', sel ? 3 : 1.5);
            rect.setAttribute('data-uuid', sym.uuid || '');
            if (sym.angle) rect.setAttribute('transform', `rotate(${sym.angle} ${cx} ${cy})`);
            g.appendChild(rect);

            // Reference (top).
            const ref = document.createElementNS(ns, 'text');
            ref.setAttribute('x', cx);
            ref.setAttribute('y', cy - half - 6);
            ref.setAttribute('text-anchor', 'middle');
            ref.setAttribute('fill', COLORS.text);
            ref.setAttribute('font-family', 'monospace');
            ref.setAttribute('font-size', '11');
            ref.textContent = sym.reference || '';
            g.appendChild(ref);

            // Value (bottom).
            const val = document.createElementNS(ns, 'text');
            val.setAttribute('x', cx);
            val.setAttribute('y', cy + half + 12);
            val.setAttribute('text-anchor', 'middle');
            val.setAttribute('fill', COLORS.text);
            val.setAttribute('font-family', 'monospace');
            val.setAttribute('font-size', '11');
            val.textContent = sym.value || sym.lib_id || '';
            g.appendChild(val);
        }

        _drawWire(g, ns, view, w, sel, color) {
            if (!w.pts || w.pts.length < 2) return;
            const pts = w.pts.map(p => `${sx(view, p.x)},${sy(view, p.y)}`).join(' ');
            const line = document.createElementNS(ns, 'polyline');
            line.setAttribute('points', pts);
            line.setAttribute('fill', 'none');
            line.setAttribute('stroke', sel ? COLORS.selection : (color || COLORS.wire));
            line.setAttribute('stroke-width', sel ? 3 : 1.5);
            line.setAttribute('data-uuid', w.uuid || '');
            g.appendChild(line);
        }

        _drawJunction(g, ns, view, j, sel) {
            const c = document.createElementNS(ns, 'circle');
            c.setAttribute('cx', sx(view, j.at.x));
            c.setAttribute('cy', sy(view, j.at.y));
            c.setAttribute('r', sel ? 4 : 3);
            c.setAttribute('fill', sel ? COLORS.selection : COLORS.junction);
            c.setAttribute('data-uuid', j.uuid || '');
            g.appendChild(c);
        }

        _drawNoConnect(g, ns, view, n, sel) {
            const cx = sx(view, n.at.x);
            const cy = sy(view, n.at.y);
            const d = 6;
            const p = document.createElementNS(ns, 'path');
            p.setAttribute('d', `M${cx-d},${cy-d} L${cx+d},${cy+d} M${cx-d},${cy+d} L${cx+d},${cy-d}`);
            p.setAttribute('stroke', sel ? COLORS.selection : COLORS.noConnect);
            p.setAttribute('stroke-width', 2);
            p.setAttribute('data-uuid', n.uuid || '');
            g.appendChild(p);
        }

        _drawLabel(g, ns, view, L, sel) {
            const color = L.kind === 'global_label' ? COLORS.globalLabel
                        : L.kind === 'hier_label'   ? COLORS.hierLabel
                        : COLORS.localLabel;
            const x = sx(view, L.at.x), y = sy(view, L.at.y);
            const t = document.createElementNS(ns, 'text');
            t.setAttribute('x', x + 4);
            t.setAttribute('y', y - 4);
            t.setAttribute('fill', sel ? COLORS.selection : color);
            t.setAttribute('font-family', 'monospace');
            t.setAttribute('font-size', '11');
            t.setAttribute('data-uuid', L.uuid || '');
            t.textContent = L.text || '';
            g.appendChild(t);
        }

        _drawText(g, ns, view, tx, sel) {
            const t = document.createElementNS(ns, 'text');
            t.setAttribute('x', sx(view, tx.at.x));
            t.setAttribute('y', sy(view, tx.at.y));
            t.setAttribute('fill', sel ? COLORS.selection : COLORS.text);
            t.setAttribute('font-family', 'monospace');
            t.setAttribute('font-size', '10');
            t.textContent = tx.text || '';
            g.appendChild(t);
        }

        // Very coarse: symbol box hit vs point.
        _hitTest(wx, wy) {
            const items = (this.model.root && this.model.root.items) || [];
            const scale = 1e6;
            for (let i = items.length - 1; i >= 0; --i) {
                const it = items[i];
                if (it.at) {
                    const half = 3175000;   // 3.175 mm in nm
                    if (Math.abs(wx - it.at.x) < half && Math.abs(wy - it.at.y) < half) return it;
                }
                if (it.pts && it.pts.length >= 2) {
                    for (let k = 0; k + 1 < it.pts.length; ++k) {
                        const a = it.pts[k], b = it.pts[k+1];
                        const d = pointSegDist(wx, wy, a, b);
                        if (d < 1_000_000) return it;
                    }
                }
            }
            return null;
        }
    }

    function pointSegDist(px, py, a, b) {
        const dx = b.x - a.x, dy = b.y - a.y;
        const L2 = dx*dx + dy*dy;
        if (L2 === 0) return Math.hypot(px - a.x, py - a.y);
        let t = ((px - a.x) * dx + (py - a.y) * dy) / L2;
        t = Math.max(0, Math.min(1, t));
        return Math.hypot(px - (a.x + t*dx), py - (a.y + t*dy));
    }

    window.EDA_Sch = { SchematicRenderer };
})();
