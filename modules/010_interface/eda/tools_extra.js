// SPDX-License-Identifier: GPL-3.0-or-later
// Additional schematic and PCB tools not covered by tools.js.
// Registers into window.EDA_Tools so editor.js can dispatch them.

(function () {
    'use strict';
    if (!window.EDA_Tools) window.EDA_Tools = {};
    const T = window.EDA_Tools;

    const NS = 'http://www.w3.org/2000/svg';

    // ---------------------- DrawBusTool ----------------------
    // Same behavior as DrawWireTool but emits kind:"bus".
    class DrawBusTool {
        constructor(api) { this.api = api; this.pts = []; }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onLeave(scene) { scene.container.style.cursor = ''; this.pts = []; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            if (this.pts.length === 0) { this.pts = [{ x: gx, y: gy }]; return; }
            const last = this.pts[this.pts.length - 1];
            if (last.x !== gx) this.pts.push({ x: gx, y: last.y });
            this.pts.push({ x: gx, y: gy });
            this.api.addSchItem({
                kind: 'bus',
                pts: this.pts.map(pt => [pt.x / 1e6, pt.y / 1e6])
            });
            this.pts = [{ x: gx, y: gy }];
        }
        onMouseMove(scene, p) { this.hover = scene.snapPoint(p.wx, p.wy); scene.render(); }
        onKey(scene, ev) { if (ev.key === 'Escape') { this.pts = []; this.api.setTool('select'); } }
    }

    // ---------------------- DrawBusEntryTool ----------------------
    // Places a small diagonal bus entry at a grid point.
    class DrawBusEntryTool {
        constructor(api, size_mm = 2.54) {
            this.api = api;
            this.size_mm = size_mm;
        }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            this.api.addSchItem({
                kind: 'bus_entry',
                at: [gx / 1e6, gy / 1e6],
                size: [this.size_mm, this.size_mm]
            });
        }
        onKey(scene, ev) { if (ev.key === 'Escape') this.api.setTool('select'); }
    }

    // ---------------------- DrawTextTool ----------------------
    class DrawTextTool {
        constructor(api) { this.api = api; }
        onEnter(scene) { scene.container.style.cursor = 'text'; }
        onMouseDown(scene, p) {
            const text = prompt('Text:');
            if (!text) return;
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            this.api.addSchItem({
                kind: 'text',
                text,
                at: [gx / 1e6, gy / 1e6],
                angle: 0
            });
        }
        onKey(scene, ev) { if (ev.key === 'Escape') this.api.setTool('select'); }
    }

    // ---------------------- DrawTextBoxTool ----------------------
    // Click two corners; prompts for text.
    class DrawTextBoxTool {
        constructor(api) { this.api = api; this.first = null; }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            if (!this.first) { this.first = { x: gx, y: gy }; return; }
            const text = prompt('Text-box content:');
            if (!text) { this.first = null; return; }
            const x = Math.min(this.first.x, gx);
            const y = Math.min(this.first.y, gy);
            const w = Math.abs(gx - this.first.x);
            const h = Math.abs(gy - this.first.y);
            this.api.addSchItem({
                kind: 'text_box',
                text,
                at:   [x / 1e6, y / 1e6],
                size: [w / 1e6, h / 1e6]
            });
            this.first = null;
        }
        onKey(scene, ev) { if (ev.key === 'Escape') { this.first = null; this.api.setTool('select'); } }
        drawOverlay(scene, svg) {
            if (!this.first || !this.hover) return;
            const [hx, hy] = this.hover;
            const p1 = scene.view.toScreenX(this.first.x), q1 = scene.view.toScreenY(this.first.y);
            const p2 = scene.view.toScreenX(hx),           q2 = scene.view.toScreenY(hy);
            const r = document.createElementNS(NS, 'rect');
            r.setAttribute('x', Math.min(p1, p2));
            r.setAttribute('y', Math.min(q1, q2));
            r.setAttribute('width',  Math.abs(p2 - p1));
            r.setAttribute('height', Math.abs(q2 - q1));
            r.setAttribute('fill', 'none');
            r.setAttribute('stroke', '#c9a20e');
            r.setAttribute('stroke-dasharray', '4,3');
            svg.appendChild(r);
        }
        onMouseMove(scene, p) { this.hover = scene.snapPoint(p.wx, p.wy); scene.render(); }
    }

    // ---------------------- DrawRectangleTool ----------------------
    class DrawRectangleTool {
        constructor(api) { this.api = api; this.first = null; }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            if (!this.first) { this.first = { x: gx, y: gy }; return; }
            this.api.addSchItem({
                kind: 'shape',
                shape: 'rectangle',
                start: [this.first.x / 1e6, this.first.y / 1e6],
                end:   [gx / 1e6,           gy / 1e6]
            });
            this.first = null;
        }
        onMouseMove(scene, p) { this.hover = scene.snapPoint(p.wx, p.wy); scene.render(); }
        onKey(scene, ev) { if (ev.key === 'Escape') { this.first = null; this.api.setTool('select'); } }
        drawOverlay(scene, svg) {
            if (!this.first || !this.hover) return;
            const [hx, hy] = this.hover;
            const s = scene.view;
            const r = document.createElementNS(NS, 'rect');
            r.setAttribute('x', Math.min(s.toScreenX(this.first.x), s.toScreenX(hx)));
            r.setAttribute('y', Math.min(s.toScreenY(this.first.y), s.toScreenY(hy)));
            r.setAttribute('width',  Math.abs(s.toScreenX(hx) - s.toScreenX(this.first.x)));
            r.setAttribute('height', Math.abs(s.toScreenY(hy) - s.toScreenY(this.first.y)));
            r.setAttribute('fill', 'none');
            r.setAttribute('stroke', '#c9a20e');
            r.setAttribute('stroke-dasharray', '4,3');
            svg.appendChild(r);
        }
    }

    // ---------------------- DrawCircleTool ----------------------
    // Center + radius. First click sets center, second click sets radius.
    class DrawCircleTool {
        constructor(api) { this.api = api; this.center = null; }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            if (!this.center) { this.center = { x: gx, y: gy }; return; }
            const rx = gx - this.center.x, ry = gy - this.center.y;
            const r  = Math.hypot(rx, ry);
            this.api.addSchItem({
                kind: 'shape',
                shape: 'circle',
                center: [this.center.x / 1e6, this.center.y / 1e6],
                radius: r / 1e6
            });
            this.center = null;
        }
        onMouseMove(scene, p) { this.hover = { x: p.wx, y: p.wy }; scene.render(); }
        onKey(scene, ev) { if (ev.key === 'Escape') { this.center = null; this.api.setTool('select'); } }
        drawOverlay(scene, svg) {
            if (!this.center || !this.hover) return;
            const rx = this.hover.x - this.center.x, ry = this.hover.y - this.center.y;
            const r_world = Math.hypot(rx, ry);
            const c = document.createElementNS(NS, 'circle');
            c.setAttribute('cx', scene.view.toScreenX(this.center.x));
            c.setAttribute('cy', scene.view.toScreenY(this.center.y));
            c.setAttribute('r',  r_world * scene.view.scale);
            c.setAttribute('fill', 'none');
            c.setAttribute('stroke', '#c9a20e');
            c.setAttribute('stroke-dasharray', '4,3');
            svg.appendChild(c);
        }
    }

    // ---------------------- DrawPolylineTool ----------------------
    // Chained click; double-click or Enter to finalize.
    class DrawPolylineTool {
        constructor(api) { this.api = api; this.pts = []; }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            this.pts.push({ x: gx, y: gy });
        }
        onMouseMove(scene, p) { this.hover = scene.snapPoint(p.wx, p.wy); scene.render(); }
        onKey(scene, ev) {
            if (ev.key === 'Escape') { this.pts = []; this.api.setTool('select'); }
            if (ev.key === 'Enter' && this.pts.length >= 2) {
                this.api.addSchItem({
                    kind: 'shape',
                    shape: 'polyline',
                    pts: this.pts.map(p => [p.x / 1e6, p.y / 1e6])
                });
                this.pts = [];
            }
        }
        drawOverlay(scene, svg) {
            if (this.pts.length === 0) return;
            const s = scene.view;
            const pointsAttr = this.pts.map(p => `${s.toScreenX(p.x)},${s.toScreenY(p.y)}`).join(' ')
                            + (this.hover ? ` ${s.toScreenX(this.hover[0])},${s.toScreenY(this.hover[1])}` : '');
            const l = document.createElementNS(NS, 'polyline');
            l.setAttribute('points', pointsAttr);
            l.setAttribute('fill', 'none');
            l.setAttribute('stroke', '#c9a20e');
            l.setAttribute('stroke-dasharray', '4,3');
            l.setAttribute('stroke-width', 1.5);
            svg.appendChild(l);
        }
    }

    // ---------------------- MirrorHelpers ----------------------
    // Not a tool per se; used by the SelectTool via keybinding.
    // Mirror is implemented server-side by dispatching a rotate(180)
    // for the MVP; a proper mirror needs a schema change to the model
    // command set (add mirror_x/mirror_y toggles).
    const MirrorHelpers = {
        // Best effort until we ship a real mirror command.
        mirrorX(api, uuid, kind) { api.rotate({ uuid, deg: 180, kind }); },
        mirrorY(api, uuid, kind) { api.rotate({ uuid, deg: 180, kind }); }
    };

    T.DrawBusTool         = DrawBusTool;
    T.DrawBusEntryTool    = DrawBusEntryTool;
    T.DrawTextTool        = DrawTextTool;
    T.DrawTextBoxTool     = DrawTextBoxTool;
    T.DrawRectangleTool   = DrawRectangleTool;
    T.DrawCircleTool      = DrawCircleTool;
    T.DrawPolylineTool    = DrawPolylineTool;
    T.MirrorHelpers       = MirrorHelpers;
})();
