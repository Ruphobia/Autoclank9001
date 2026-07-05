// SPDX-License-Identifier: GPL-3.0-or-later
// PCB-side tools not covered by tools.js.
// Registers into window.EDA_Tools alongside DrawTrackTool, DrawViaTool,
// PlaceFootprintTool.

(function () {
    'use strict';
    if (!window.EDA_Tools) window.EDA_Tools = {};
    const T = window.EDA_Tools;
    const NS = 'http://www.w3.org/2000/svg';

    // ---------------------- DrawZoneTool ----------------------
    // Polygon-outline zone on a copper layer. Click a sequence of
    // vertices; Enter closes and commits.
    class DrawZoneTool {
        constructor(api, layer = 'F.Cu', net = 0) {
            this.api = api; this.layer = layer; this.net = net; this.pts = [];
        }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onLeave(scene) { scene.container.style.cursor = ''; this.pts = []; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            this.pts.push({ x: gx, y: gy });
        }
        onMouseMove(scene, p) { this.hover = scene.snapPoint(p.wx, p.wy); scene.render(); }
        onKey(scene, ev) {
            if (ev.key === 'Escape') { this.pts = []; this.api.setTool('select'); }
            if (ev.key === 'Enter' && this.pts.length >= 3) {
                this.api.addPcbItem({
                    kind: 'zone',
                    layer: this.layer,
                    net: this.net,
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
            l.setAttribute('fill', 'rgba(200,138,23,0.15)');
            l.setAttribute('stroke', '#c88a17');
            l.setAttribute('stroke-dasharray', '5,3');
            l.setAttribute('stroke-width', 1.5);
            svg.appendChild(l);
        }
    }

    // ---------------------- DrawCopperPolygonTool ----------------------
    // Same shape as DrawZoneTool but emits a filled polygon (no net).
    class DrawCopperPolygonTool {
        constructor(api, layer = 'F.Cu') { this.api = api; this.layer = layer; this.pts = []; }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            this.pts.push({ x: gx, y: gy });
        }
        onMouseMove(scene, p) { this.hover = scene.snapPoint(p.wx, p.wy); scene.render(); }
        onKey(scene, ev) {
            if (ev.key === 'Escape') { this.pts = []; this.api.setTool('select'); }
            if (ev.key === 'Enter' && this.pts.length >= 3) {
                this.api.addPcbItem({
                    kind: 'gr_polygon',
                    layer: this.layer,
                    fill: 'solid',
                    pts: this.pts.map(p => [p.x / 1e6, p.y / 1e6])
                });
                this.pts = [];
            }
        }
    }

    // ---------------------- DrawGrLineTool ----------------------
    // Straight line on any user layer (Edge.Cuts, silk, fab, margin).
    class DrawGrLineTool {
        constructor(api, layer = 'Edge.Cuts', width_mm = 0.15) {
            this.api = api; this.layer = layer; this.width = width_mm;
            this.start = null;
        }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            if (!this.start) { this.start = { x: gx, y: gy }; return; }
            this.api.addPcbItem({
                kind: 'gr_line',
                start: [this.start.x / 1e6, this.start.y / 1e6],
                end:   [gx / 1e6, gy / 1e6],
                width: this.width,
                layer: this.layer
            });
            this.start = { x: gx, y: gy };
        }
        onMouseMove(scene, p) { this.hover = scene.snapPoint(p.wx, p.wy); scene.render(); }
        onKey(scene, ev) { if (ev.key === 'Escape') { this.start = null; this.api.setTool('select'); } }
        drawOverlay(scene, svg) {
            if (!this.start || !this.hover) return;
            const s = scene.view;
            const l = document.createElementNS(NS, 'line');
            l.setAttribute('x1', s.toScreenX(this.start.x));
            l.setAttribute('y1', s.toScreenY(this.start.y));
            l.setAttribute('x2', s.toScreenX(this.hover[0]));
            l.setAttribute('y2', s.toScreenY(this.hover[1]));
            l.setAttribute('stroke', '#f2f200');
            l.setAttribute('stroke-dasharray', '4,3');
            svg.appendChild(l);
        }
    }

    // ---------------------- DrawGrArcTool ----------------------
    // Three-point arc: click start, mid, end.
    class DrawGrArcTool {
        constructor(api, layer = 'F.SilkS', width_mm = 0.15) {
            this.api = api; this.layer = layer; this.width = width_mm;
            this.pts = [];
        }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            this.pts.push({ x: gx, y: gy });
            if (this.pts.length === 3) {
                this.api.addPcbItem({
                    kind: 'gr_arc',
                    start: [this.pts[0].x / 1e6, this.pts[0].y / 1e6],
                    mid:   [this.pts[1].x / 1e6, this.pts[1].y / 1e6],
                    end:   [this.pts[2].x / 1e6, this.pts[2].y / 1e6],
                    width: this.width,
                    layer: this.layer
                });
                this.pts = [];
            }
        }
        onKey(scene, ev) { if (ev.key === 'Escape') { this.pts = []; this.api.setTool('select'); } }
    }

    // ---------------------- DrawDimensionTool ----------------------
    // Center-point aligned linear dimension. Two clicks.
    class DrawDimensionTool {
        constructor(api, layer = 'Dwgs.User') { this.api = api; this.layer = layer; this.a = null; }
        onEnter(scene) { scene.container.style.cursor = 'crosshair'; }
        onMouseDown(scene, p) {
            const [gx, gy] = scene.snapPoint(p.wx, p.wy);
            if (!this.a) { this.a = { x: gx, y: gy }; return; }
            this.api.addPcbItem({
                kind: 'dimension',
                type: 'aligned',
                start: [this.a.x / 1e6, this.a.y / 1e6],
                end:   [gx / 1e6, gy / 1e6],
                layer: this.layer
            });
            this.a = null;
        }
        onKey(scene, ev) { if (ev.key === 'Escape') { this.a = null; this.api.setTool('select'); } }
    }

    // ---------------------- LayerVisibilityPanel ----------------------
    // Not a canvas tool; renders a per-layer checkbox strip. Wire this
    // into the sidebar in the interactive editor shell.
    class LayerVisibilityPanel {
        constructor(root, pcbRenderer) {
            this.root = root;
            this.pcbRenderer = pcbRenderer;
            this._build();
        }
        _build() {
            const layers = [
                'F.Cu','B.Cu','F.SilkS','B.SilkS','F.Mask','B.Mask',
                'F.Paste','B.Paste','F.CrtYd','B.CrtYd','F.Fab','B.Fab','Edge.Cuts'
            ];
            this.root.classList.add('eda-layer-panel');
            this.root.innerHTML = layers.map(name => `
                <label class="eda-layer-row" data-layer="${name}">
                    <input type="checkbox" ${this.pcbRenderer.visible.has(name) ? 'checked' : ''}>
                    <span class="swatch" style="background:${LAYER_COLOR[name] || '#888'}"></span>
                    <span class="name">${name}</span>
                </label>`).join('') + `
                <label class="eda-layer-row">
                    <input type="checkbox" ${this.pcbRenderer.showRatsnest ? 'checked' : ''} data-ratsnest>
                    <span class="swatch" style="background:#666"></span>
                    <span class="name">Ratsnest</span>
                </label>`;
            this.root.addEventListener('change', ev => {
                const row = ev.target.closest('label');
                if (!row) return;
                if (row.querySelector('[data-ratsnest]') === ev.target) {
                    this.pcbRenderer.setRatsnestVisible(ev.target.checked);
                    return;
                }
                const name = row.dataset.layer;
                this.pcbRenderer.setLayerVisible(name, ev.target.checked);
            });
        }
    }

    const LAYER_COLOR = {
        'F.Cu':'#c88a17','B.Cu':'#4d7fc4','F.SilkS':'#eaeaea','B.SilkS':'#eaeaea',
        'F.Mask':'#4bb051','B.Mask':'#4bb051','F.Paste':'#808080','B.Paste':'#808080',
        'F.CrtYd':'#7f7f7f','B.CrtYd':'#7f7f7f','F.Fab':'#a08040','B.Fab':'#a08040',
        'Edge.Cuts':'#f2f200'
    };

    T.DrawZoneTool          = DrawZoneTool;
    T.DrawCopperPolygonTool = DrawCopperPolygonTool;
    T.DrawGrLineTool        = DrawGrLineTool;
    T.DrawGrArcTool         = DrawGrArcTool;
    T.DrawDimensionTool     = DrawDimensionTool;

    window.EDA_LayerPanel = { LayerVisibilityPanel };
})();
