// SPDX-License-Identifier: GPL-3.0-or-later
// Canvas framework for the KiCad-in-browser editor.
// Provides:
//   * a scene wrapper that owns a viewport (pan + zoom)
//   * a hit-testing pass over registered items
//   * a grid + snap + rulers overlay
//   * unified pointer events feeding into whatever tool is active
//
// SchematicRenderer / PcbRenderer plug into this by registering
// draw callbacks and hit-test callbacks; ToolStateMachine plugs in
// as the pointer sink.

(function () {
    'use strict';

    // --- View: world coordinates (nm) <-> screen coordinates (px) ---

    class View {
        constructor(el) {
            this.el = el;
            // Pan is measured in screen px so wheel/mouse arithmetic
            // stays linear regardless of zoom.
            this.tx = 0;
            this.ty = 0;
            // Scale is screen-px per world-nm. Sensible schematic
            // default: 1 nm = 1e-6 mm, want ~4 px/mm -> 4e-6.
            this.scale = 4e-6;
        }
        // World -> screen.
        toScreenX(x) { return x * this.scale + this.tx; }
        toScreenY(y) { return y * this.scale + this.ty; }
        // Screen -> world.
        toWorldX(px) { return (px - this.tx) / this.scale; }
        toWorldY(py) { return (py - this.ty) / this.scale; }
        // Zoom around a screen point.
        zoomAt(px, py, factor) {
            const wx = this.toWorldX(px), wy = this.toWorldY(py);
            this.scale *= factor;
            // Keep (wx, wy) under the same screen pixel.
            this.tx = px - wx * this.scale;
            this.ty = py - wy * this.scale;
        }
        // Fit a bounding box.
        fitWorld(bbox, pad = 32) {
            if (!bbox) return;
            const rect = this.el.getBoundingClientRect();
            const w = rect.width  - 2 * pad;
            const h = rect.height - 2 * pad;
            if (w <= 0 || h <= 0) return;
            const bw = Math.max(1, bbox.hi.x - bbox.lo.x);
            const bh = Math.max(1, bbox.hi.y - bbox.lo.y);
            this.scale = Math.min(w / bw, h / bh);
            this.tx = pad - bbox.lo.x * this.scale;
            this.ty = pad - bbox.lo.y * this.scale;
        }
    }

    // --- Scene: owns layers of drawables + hit index ---

    class Scene {
        constructor(container) {
            container.style.position = 'relative';
            container.style.overflow = 'hidden';
            container.classList.add('eda-scene');
            this.container = container;

            // SVG surface for schematic-style vector rendering.
            this.svgNS = 'http://www.w3.org/2000/svg';
            this.svg = document.createElementNS(this.svgNS, 'svg');
            this.svg.setAttribute('width',  '100%');
            this.svg.setAttribute('height', '100%');
            this.svg.style.position = 'absolute';
            this.svg.style.top = '0';
            this.svg.style.left = '0';
            container.appendChild(this.svg);

            // Canvas surface for PCB-style raster/copper rendering.
            this.canvas = document.createElement('canvas');
            this.canvas.style.position = 'absolute';
            this.canvas.style.top = '0';
            this.canvas.style.left = '0';
            this.canvas.style.pointerEvents = 'none';
            this.canvas.style.display = 'none';    // enabled by pcb renderer
            container.appendChild(this.canvas);

            // Overlay for UI: rubber band, cursor crosshair, tooltips.
            this.overlay = document.createElement('div');
            this.overlay.style.position = 'absolute';
            this.overlay.style.inset = '0';
            this.overlay.style.pointerEvents = 'none';
            container.appendChild(this.overlay);

            this.view = new View(container);
            this.snap = 1_270_000;  // 1.27 mm in nm (KiCad's schematic 50-mil grid)
            this.gridVisible = true;

            this.drawables = [];     // { draw(view, layer), hitTest(worldXY) }
            this.tool = null;        // active tool (from tools.js)

            this._resizeObserver = new ResizeObserver(() => this._resize());
            this._resizeObserver.observe(container);
            this._resize();

            this._bindPointer();
        }

        setTool(tool) {
            if (this.tool && this.tool.onLeave) this.tool.onLeave(this);
            this.tool = tool;
            if (tool && tool.onEnter) tool.onEnter(this);
            this.render();
        }

        add(drawable) {
            this.drawables.push(drawable);
            this.render();
        }
        clear() { this.drawables.length = 0; this.render(); }

        // Return the topmost drawable whose hit-test succeeds at world (x,y).
        pick(wx, wy) {
            for (let i = this.drawables.length - 1; i >= 0; --i) {
                const d = this.drawables[i];
                if (d.hitTest && d.hitTest(wx, wy)) return d;
            }
            return null;
        }

        // Snap a world point to the grid.
        snapPoint(wx, wy) {
            const s = this.snap;
            return [ Math.round(wx / s) * s, Math.round(wy / s) * s ];
        }

        setGridSnap(nm) { this.snap = nm; this.render(); }

        _resize() {
            const r = this.container.getBoundingClientRect();
            this.canvas.width  = r.width;
            this.canvas.height = r.height;
            this.render();
        }

        _bindPointer() {
            const el = this.container;
            let dragging = false;
            let panning  = false;
            let sx = 0, sy = 0;

            const worldPoint = (ev) => {
                const rect = el.getBoundingClientRect();
                const px = ev.clientX - rect.left;
                const py = ev.clientY - rect.top;
                return { px, py, wx: this.view.toWorldX(px), wy: this.view.toWorldY(py) };
            };

            el.addEventListener('mousedown', (ev) => {
                el.focus();
                const p = worldPoint(ev);
                if (ev.button === 1 || (ev.button === 0 && ev.altKey)) {
                    panning = true; sx = ev.clientX; sy = ev.clientY;
                    el.style.cursor = 'grabbing';
                    return;
                }
                if (this.tool && this.tool.onMouseDown) {
                    dragging = this.tool.onMouseDown(this, p, ev) === true;
                }
            });

            window.addEventListener('mouseup', (ev) => {
                const p = worldPoint(ev);
                if (panning) { panning = false; el.style.cursor = ''; return; }
                if (this.tool && this.tool.onMouseUp) this.tool.onMouseUp(this, p, ev);
                dragging = false;
            });

            window.addEventListener('mousemove', (ev) => {
                const p = worldPoint(ev);
                if (panning) {
                    this.view.tx += ev.clientX - sx;
                    this.view.ty += ev.clientY - sy;
                    sx = ev.clientX; sy = ev.clientY;
                    this.render();
                    return;
                }
                if (this.tool && this.tool.onMouseMove) this.tool.onMouseMove(this, p, ev);
            });

            el.addEventListener('wheel', (ev) => {
                ev.preventDefault();
                const rect = el.getBoundingClientRect();
                const factor = ev.deltaY < 0 ? 1.15 : 1 / 1.15;
                this.view.zoomAt(ev.clientX - rect.left, ev.clientY - rect.top, factor);
                this.render();
            }, { passive: false });

            el.tabIndex = 0;
            el.addEventListener('keydown', (ev) => {
                if (this.tool && this.tool.onKey) this.tool.onKey(this, ev);
            });
        }

        // Clear the SVG surface and let each drawable re-emit.
        render() {
            // Grid first.
            while (this.svg.firstChild) this.svg.removeChild(this.svg.firstChild);
            if (this.gridVisible) this._drawGrid();
            for (const d of this.drawables) {
                if (d.drawSvg) d.drawSvg(this.svg, this.view, this.svgNS);
            }
            // PCB canvas draws lazily via pcb_renderer.
            if (this.tool && this.tool.drawOverlay) this.tool.drawOverlay(this, this.svg);
        }

        _drawGrid() {
            const rect = this.container.getBoundingClientRect();
            const spacing = this.snap * this.view.scale;
            if (spacing < 5) return;   // too dense to be useful
            const startX = ((this.view.tx % spacing) + spacing) % spacing;
            const startY = ((this.view.ty % spacing) + spacing) % spacing;
            const ns = this.svgNS;
            const g = document.createElementNS(ns, 'g');
            g.setAttribute('class', 'eda-grid');
            const stroke = spacing > 20 ? '#26292d' : '#1c1e22';
            for (let x = startX; x < rect.width;  x += spacing) {
                const line = document.createElementNS(ns, 'line');
                line.setAttribute('x1', x); line.setAttribute('x2', x);
                line.setAttribute('y1', 0); line.setAttribute('y2', rect.height);
                line.setAttribute('stroke', stroke);
                line.setAttribute('stroke-width', '1');
                g.appendChild(line);
            }
            for (let y = startY; y < rect.height; y += spacing) {
                const line = document.createElementNS(ns, 'line');
                line.setAttribute('x1', 0);          line.setAttribute('x2', rect.width);
                line.setAttribute('y1', y);          line.setAttribute('y2', y);
                line.setAttribute('stroke', stroke);
                line.setAttribute('stroke-width', '1');
                g.appendChild(line);
            }
            this.svg.appendChild(g);
        }
    }

    // Bounding box helper used by fitWorld().
    function bboxUnion(a, b) {
        if (!a) return b;
        if (!b) return a;
        return {
            lo: { x: Math.min(a.lo.x, b.lo.x), y: Math.min(a.lo.y, b.lo.y) },
            hi: { x: Math.max(a.hi.x, b.hi.x), y: Math.max(a.hi.y, b.hi.y) }
        };
    }
    function bboxFromPoints(pts) {
        if (!pts.length) return null;
        let lox = pts[0].x, loy = pts[0].y, hix = pts[0].x, hiy = pts[0].y;
        for (const p of pts) {
            if (p.x < lox) lox = p.x; if (p.x > hix) hix = p.x;
            if (p.y < loy) loy = p.y; if (p.y > hiy) hiy = p.y;
        }
        return { lo: { x: lox, y: loy }, hi: { x: hix, y: hiy } };
    }

    window.EDA_Canvas = { Scene, View, bboxUnion, bboxFromPoints };
})();
