// SPDX-License-Identifier: GPL-3.0-or-later
// Tool state machine. Each tool implements a subset of:
//   onEnter(scene) / onLeave(scene)
//   onMouseDown(scene, {px,py,wx,wy}, ev)   returns true to start drag
//   onMouseMove(scene, {px,py,wx,wy}, ev)
//   onMouseUp  (scene, {px,py,wx,wy}, ev)
//   onKey      (scene, ev)                  called on keydown
//   drawOverlay(scene, svg)                 draw a cursor/rubber-band
//
// Tools use `window.EDA_Api` (defined by editor.js) to POST mutations
// to the backend. On success the caller re-fetches the model and the
// renderer re-renders.

(function () {
    'use strict';

    const NS = 'http://www.w3.org/2000/svg';

    // ------------------ SelectTool ------------------

    class SelectTool {
        constructor(api) { this.api = api; this.hover = null; this.dragStart = null; }
        onMouseDown(scene, p, ev) {
            const hit = scene.pick(p.wx, p.wy);
            if (hit) {
                const set = ev.shiftKey ? new Set(scene.selection || []) : new Set();
                set.add(hit.uuid || '');
                scene.selection = Array.from(set);
                this.api.setSelection({ sch: scene.selection });
                if (scene.schRenderer)  scene.schRenderer.setSelection(scene.selection);
                if (scene.pcbRenderer)  scene.pcbRenderer.setSelection(scene.selection);
                this.dragStart = { wx: p.wx, wy: p.wy, uuid: hit.uuid };
                return true;
            }
            scene.selection = [];
            this.api.setSelection({ sch: [] });
            if (scene.schRenderer) scene.schRenderer.setSelection([]);
            if (scene.pcbRenderer) scene.pcbRenderer.setSelection([]);
            return false;
        }
        onMouseMove(scene, p) {
            const hit = scene.pick(p.wx, p.wy);
            if (hit !== this.hover) { this.hover = hit; scene.render(); }
        }
        onMouseUp(scene, p, ev) {
            if (this.dragStart && (this.dragStart.wx !== p.wx || this.dragStart.wy !== p.wy)) {
                // Committed a move.
                const dx_mm = (p.wx - this.dragStart.wx) / 1e6;
                const dy_mm = (p.wy - this.dragStart.wy) / 1e6;
                this.api.move({ uuid: this.dragStart.uuid, dx_mm, dy_mm, kind: scene.kind });
            }
            this.dragStart = null;
        }
        onKey(scene, ev) {
            const key = ev.key.toLowerCase();
            if (key === 'delete' || key === 'backspace') {
                for (const u of (scene.selection || [])) {
                    this.api.remove({ uuid: u, kind: scene.kind });
                }
                ev.preventDefault();
            } else if (key === 'r' && scene.selection && scene.selection.length) {
                for (const u of scene.selection) this.api.rotate({ uuid: u, deg: 90, kind: scene.kind });
                ev.preventDefault();
            }
        }
    }

    // ------------------ PlaceSymbolTool ------------------

    class PlaceSymbolTool {
        constructor(api, symbolProto) {
            this.api = api;
            this.proto = symbolProto || { lib_id: 'Device:R', reference: 'R?', value: 'R' };
            this.previewPos = null;
        }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onLeave(scene) { scene.container.style.cursor = ''; this.previewPos = null; }
        onMouseMove(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            this.previewPos = { x: gx, y: gy };
            scene.render();
        }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            this.api.addSchItem({
                kind: 'symbol',
                lib_id: this.proto.lib_id,
                reference: this.proto.reference,
                value: this.proto.value,
                at: [gx / 1e6, gy / 1e6],
                angle: 0
            });
        }
        onKey(scene, ev) {
            if (ev.key === 'Escape') this.api.setTool('select');
        }
        drawOverlay(scene, svg) {
            if (!this.previewPos) return;
            const cx = scene.view.toScreenX(this.previewPos.x);
            const cy = scene.view.toScreenY(this.previewPos.y);
            const r = document.createElementNS(NS, 'rect');
            r.setAttribute('x', cx - 12);
            r.setAttribute('y', cy - 12);
            r.setAttribute('width', 24);
            r.setAttribute('height', 24);
            r.setAttribute('fill', 'none');
            r.setAttribute('stroke', '#c9a20e');
            r.setAttribute('stroke-dasharray', '4,3');
            svg.appendChild(r);
        }
    }

    // ------------------ DrawWireTool ------------------

    class DrawWireTool {
        constructor(api) { this.api = api; this.pts = []; }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onLeave(scene) { scene.container.style.cursor = ''; this.pts = []; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            if (this.pts.length === 0) {
                this.pts = [{ x: gx, y: gy }];
            } else {
                const last = this.pts[this.pts.length - 1];
                // Add an L-shape: horizontal-then-vertical.
                if (last.x !== gx) this.pts.push({ x: gx, y: last.y });
                this.pts.push({ x: gx, y: gy });
                // Commit as a wire.
                this.api.addSchItem({
                    kind: 'wire',
                    pts: this.pts.map(pt => [pt.x / 1e6, pt.y / 1e6])
                });
                this.pts = [{ x: gx, y: gy }];
            }
        }
        onMouseMove(scene, p) {
            this.hoverPos = scene.snapPoint(p.wx, p.wy);
            scene.render();
        }
        onKey(scene, ev) {
            if (ev.key === 'Escape') { this.pts = []; this.api.setTool('select'); }
        }
        drawOverlay(scene, svg) {
            if (this.pts.length === 0 || !this.hoverPos) return;
            const [hx, hy] = this.hoverPos;
            const last = this.pts[this.pts.length - 1];
            const p1x = scene.view.toScreenX(last.x), p1y = scene.view.toScreenY(last.y);
            const p2x = scene.view.toScreenX(hx),     p2y = scene.view.toScreenY(hy);
            // Same L-shape: horizontal segment then vertical.
            const l1 = document.createElementNS(NS, 'polyline');
            l1.setAttribute('points', `${p1x},${p1y} ${p2x},${p1y} ${p2x},${p2y}`);
            l1.setAttribute('fill', 'none');
            l1.setAttribute('stroke', '#c9a20e');
            l1.setAttribute('stroke-dasharray', '4,3');
            l1.setAttribute('stroke-width', 1.5);
            svg.appendChild(l1);
        }
    }

    // ------------------ DrawLabelTool ------------------

    class DrawLabelTool {
        constructor(api, kind = 'label') { this.api = api; this.kind = kind; }
        onEnter(scene) { scene.container.style.cursor = 'text'; }
        onMouseDown(scene, p) {
            const text = prompt(this.kind === 'global_label' ? 'Global label:' :
                                this.kind === 'hier_label'   ? 'Hierarchical label:' :
                                'Label:');
            if (!text) return;
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            this.api.addSchItem({
                kind: this.kind,
                text,
                at: [gx / 1e6, gy / 1e6],
                angle: 0
            });
        }
        onKey(scene, ev) { if (ev.key === 'Escape') this.api.setTool('select'); }
    }

    // ------------------ DrawJunctionTool ------------------

    class DrawJunctionTool {
        constructor(api) { this.api = api; }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            this.api.addSchItem({ kind: 'junction', at: [gx / 1e6, gy / 1e6] });
        }
        onKey(scene, ev) { if (ev.key === 'Escape') this.api.setTool('select'); }
    }

    // ------------------ DrawNoConnectTool ------------------

    class DrawNoConnectTool {
        constructor(api) { this.api = api; }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            this.api.addSchItem({ kind: 'no_connect', at: [gx / 1e6, gy / 1e6] });
        }
        onKey(scene, ev) { if (ev.key === 'Escape') this.api.setTool('select'); }
    }

    // ------------------ PlaceFootprintTool (PCB) ------------------

    class PlaceFootprintTool {
        constructor(api, proto) { this.api = api; this.proto = proto || { lib_id: 'test:generic', reference: 'U?', value: '' }; this.pos = null; }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseMove(scene, p) { this.pos = scene.snapPoint(p.wx, p.wy); scene.render(); }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            this.api.addPcbItem({
                kind: 'footprint',
                lib_id: this.proto.lib_id,
                reference: this.proto.reference,
                value: this.proto.value,
                at: [gx / 1e6, gy / 1e6],
                angle: 0,
                layer: 'F.Cu'
            });
        }
        onKey(scene, ev) { if (ev.key === 'Escape') this.api.setTool('select'); }
    }

    // ------------------ DrawTrackTool (PCB) ------------------

    class DrawTrackTool {
        constructor(api, layer = 'F.Cu', width_mm = 0.2, net = 0) {
            this.api = api; this.layer = layer; this.width = width_mm; this.net = net;
            this.start = null;
        }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            if (!this.start) { this.start = { x: gx, y: gy }; return; }
            this.api.addPcbItem({
                kind: 'track',
                start: [this.start.x / 1e6, this.start.y / 1e6],
                end:   [gx / 1e6,           gy / 1e6],
                width: this.width,
                layer: this.layer,
                net:   this.net
            });
            this.start = { x: gx, y: gy };
        }
        onMouseMove(scene, p) {
            this.hover = scene.snapPoint(p.wx, p.wy);
            scene.render();
        }
        onKey(scene, ev) { if (ev.key === 'Escape') { this.start = null; this.api.setTool('select'); } }
        drawOverlay(scene, svg) {
            if (!this.start || !this.hover) return;
            const s = scene.view;
            const l = document.createElementNS(NS, 'line');
            l.setAttribute('x1', s.toScreenX(this.start.x));
            l.setAttribute('y1', s.toScreenY(this.start.y));
            l.setAttribute('x2', s.toScreenX(this.hover[0]));
            l.setAttribute('y2', s.toScreenY(this.hover[1]));
            l.setAttribute('stroke', '#c88a17');
            l.setAttribute('stroke-width', Math.max(1, this.width * 1e6 * s.scale));
            l.setAttribute('stroke-dasharray', '4,3');
            svg.appendChild(l);
        }
    }

    // ------------------ DrawViaTool ------------------
    // Supports via_type: 'through' | 'blind' | 'buried' | 'micro'.
    // Layer span defaults to F.Cu -> B.Cu for through vias; blind/buried
    // vias take an explicit [top, bottom] layer pair; microvias default
    // to F.Cu -> In1.Cu (or B.Cu -> In(N).Cu).
    //
    // Cycle via type with 'V' key while the tool is active.

    class DrawViaTool {
        constructor(api, size_mm = 0.6, drill_mm = 0.3, net = 0, via_type = 'through') {
            this.api = api;
            this.size = size_mm;
            this.drill = drill_mm;
            this.net = net;
            this.via_type = via_type;
            this.layers = ['F.Cu', 'B.Cu'];
        }
        _defaultLayersForType(t) {
            switch (t) {
                case 'blind':   return ['F.Cu', 'In1.Cu'];
                case 'buried':  return ['In1.Cu', 'In2.Cu'];
                case 'micro':   return ['F.Cu', 'In1.Cu'];
                default:        return ['F.Cu', 'B.Cu'];
            }
        }
        setViaType(t) {
            this.via_type = t;
            this.layers = this._defaultLayersForType(t);
            this.api.setStatus && this.api.setStatus('via: ' + t);
        }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            this.api.addPcbItem({
                kind: 'via',
                type: this.via_type,
                at: [gx / 1e6, gy / 1e6],
                size:  this.size,
                drill: this.drill,
                net:   this.net,
                layers: this.layers
            });
        }
        onKey(scene, ev) {
            if (ev.key === 'Escape') { this.api.setTool('select'); return; }
            if (ev.key.toLowerCase() === 't') {
                // Cycle via type on 't' key.
                const cycle = ['through','blind','buried','micro'];
                const next  = cycle[(cycle.indexOf(this.via_type) + 1) % cycle.length];
                this.setViaType(next);
                ev.preventDefault();
            }
        }
    }

    window.EDA_Tools = {
        SelectTool, PlaceSymbolTool, DrawWireTool, DrawLabelTool,
        DrawJunctionTool, DrawNoConnectTool,
        PlaceFootprintTool, DrawTrackTool, DrawViaTool
    };
})();
