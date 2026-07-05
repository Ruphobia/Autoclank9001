// SPDX-License-Identifier: GPL-3.0-or-later
// PCB Canvas2D renderer with per-layer compositing.
// The board is rendered layer by layer, each with its own color, in
// KiCad-canonical stackup order (edge cuts on top). Each layer's
// visibility is toggled from the layer panel and re-renders on demand.

(function () {
    'use strict';

    const LAYER_COLORS = {
        'F.Cu':      '#c88a17',
        'B.Cu':      '#4d7fc4',
        'In1.Cu':    '#e08000',
        'In2.Cu':    '#00a080',
        'F.Paste':   '#808080',
        'B.Paste':   '#808080',
        'F.SilkS':   '#eaeaea',
        'B.SilkS':   '#eaeaea',
        'F.Mask':    '#4bb051',
        'B.Mask':    '#4bb051',
        'Edge.Cuts': '#f2f200',
        'F.CrtYd':   '#7f7f7f',
        'B.CrtYd':   '#7f7f7f',
        'F.Fab':     '#a08040',
        'B.Fab':     '#a08040',
        'ratsnest':  '#666666'
    };

    class PcbRenderer {
        constructor(scene) {
            this.scene = scene;
            this.model = { items: [], layers: [], nets: [] };
            this.visible = new Set(['F.Cu','B.Cu','F.SilkS','Edge.Cuts']);
            this.selected = new Set();
            this.showRatsnest = true;

            // Enable canvas surface.
            scene.canvas.style.display = 'block';
            scene.canvas.style.pointerEvents = 'auto';

            // Register ourselves as a scene drawable for hit-testing;
            // draw() happens through render() override below.
            scene.add({
                drawSvg: () => {},    // canvas draws itself
                hitTest: (wx, wy) => this._hit(wx, wy)
            });

            // Bind to scene render cycle so we redraw every time view changes.
            const origRender = scene.render.bind(scene);
            scene.render = () => {
                origRender();
                this._draw();
            };
        }

        setModel(m) {
            this.model = m || { items: [], layers: [], nets: [] };
            this.scene.render();
        }

        setLayerVisible(name, visible) {
            if (visible) this.visible.add(name); else this.visible.delete(name);
            this.scene.render();
        }
        setRatsnestVisible(v) { this.showRatsnest = v; this.scene.render(); }
        setSelection(set)     { this.selected = new Set(set); this.scene.render(); }

        _draw() {
            const cv = this.scene.canvas;
            const ctx = cv.getContext('2d');
            ctx.clearRect(0, 0, cv.width, cv.height);

            // Draw in stackup order so top layers overlay bottom.
            const stack = ['B.Cu','B.SilkS','F.Mask','B.Mask','F.Paste','B.Paste','F.Cu','F.SilkS','F.CrtYd','B.CrtYd','F.Fab','B.Fab','Edge.Cuts'];
            for (const layer of stack) {
                if (!this.visible.has(layer)) continue;
                this._drawLayer(ctx, layer);
            }
            if (this.showRatsnest) this._drawRatsnest(ctx);
        }

        _drawLayer(ctx, layer) {
            ctx.save();
            ctx.strokeStyle = LAYER_COLORS[layer] || '#888';
            ctx.fillStyle   = LAYER_COLORS[layer] || '#888';
            ctx.lineCap     = 'round';

            const v = this.scene.view;
            for (const it of this.model.items || []) {
                if (it.kind === 'footprint') {
                    this._drawFootprint(ctx, v, it, layer);
                } else if (it.kind === 'track' && it.layer === layer) {
                    ctx.beginPath();
                    ctx.moveTo(v.toScreenX(it.start.x), v.toScreenY(it.start.y));
                    ctx.lineTo(v.toScreenX(it.end.x),   v.toScreenY(it.end.y));
                    ctx.lineWidth = Math.max(1, it.width * v.scale);
                    ctx.stroke();
                } else if (it.kind === 'via' && layer === 'F.Cu') {
                    ctx.beginPath();
                    ctx.arc(v.toScreenX(it.at.x), v.toScreenY(it.at.y),
                            (it.size / 2) * v.scale, 0, Math.PI * 2);
                    ctx.fill();
                    // drill hole
                    ctx.save();
                    ctx.fillStyle = '#000';
                    ctx.beginPath();
                    ctx.arc(v.toScreenX(it.at.x), v.toScreenY(it.at.y),
                            (it.drill / 2) * v.scale, 0, Math.PI * 2);
                    ctx.fill();
                    ctx.restore();
                } else if (it.kind === 'gr_line' && it.layer === layer) {
                    ctx.beginPath();
                    ctx.moveTo(v.toScreenX(it.start.x), v.toScreenY(it.start.y));
                    ctx.lineTo(v.toScreenX(it.end.x),   v.toScreenY(it.end.y));
                    ctx.lineWidth = Math.max(1, (it.width || 100000) * v.scale);
                    ctx.stroke();
                }
            }
            ctx.restore();
        }

        _drawFootprint(ctx, v, fp, layer) {
            for (const pad of (fp.pads || [])) {
                if (!(pad.layers || []).some(L => L === layer || L === '*.Cu' && (layer === 'F.Cu' || layer === 'B.Cu'))) continue;
                const a = fp.angle || 0;
                const c = Math.cos(a * Math.PI / 180), s = Math.sin(a * Math.PI / 180);
                const wx = fp.at.x + pad.at.x * c - pad.at.y * s;
                const wy = fp.at.y + pad.at.x * s + pad.at.y * c;
                const sxp = v.toScreenX(wx), syp = v.toScreenY(wy);
                const w = pad.size.x * v.scale, h = pad.size.y * v.scale;
                ctx.beginPath();
                if (pad.shape === 'circle') {
                    ctx.arc(sxp, syp, w / 2, 0, Math.PI * 2);
                    ctx.fill();
                } else {
                    ctx.fillRect(sxp - w / 2, syp - h / 2, w, h);
                }
                if (pad.drill && pad.drill > 0) {
                    ctx.save();
                    ctx.fillStyle = '#000';
                    ctx.beginPath();
                    ctx.arc(sxp, syp, (pad.drill / 2) * v.scale, 0, Math.PI * 2);
                    ctx.fill();
                    ctx.restore();
                }
            }
        }

        _drawRatsnest(ctx) {
            const v = this.scene.view;
            ctx.save();
            ctx.strokeStyle = LAYER_COLORS.ratsnest;
            ctx.setLineDash([4, 4]);
            ctx.lineWidth = 1;

            // Build net -> list of pad world centers.
            const byNet = new Map();
            for (const it of this.model.items || []) {
                if (it.kind !== 'footprint') continue;
                const a = it.angle || 0;
                const c = Math.cos(a * Math.PI / 180), s = Math.sin(a * Math.PI / 180);
                for (const pad of (it.pads || [])) {
                    if (!pad.net) continue;
                    const wx = it.at.x + pad.at.x * c - pad.at.y * s;
                    const wy = it.at.x + pad.at.x * s + pad.at.y * c;
                    let arr = byNet.get(pad.net);
                    if (!arr) { arr = []; byNet.set(pad.net, arr); }
                    arr.push({ x: wx, y: wy });
                }
            }
            for (const [, pads] of byNet) {
                for (let i = 1; i < pads.length; ++i) {
                    ctx.beginPath();
                    ctx.moveTo(v.toScreenX(pads[0].x), v.toScreenY(pads[0].y));
                    ctx.lineTo(v.toScreenX(pads[i].x), v.toScreenY(pads[i].y));
                    ctx.stroke();
                }
            }
            ctx.setLineDash([]);
            ctx.restore();
        }

        _hit(wx, wy) {
            for (const it of (this.model.items || []).slice().reverse()) {
                if (it.kind === 'footprint') {
                    const half = 2_000_000;
                    if (Math.abs(wx - it.at.x) < half && Math.abs(wy - it.at.y) < half) return it;
                }
                if (it.kind === 'track') {
                    const d = pointSegDist(wx, wy, it.start, it.end);
                    if (d < (it.width || 100000)) return it;
                }
                if (it.kind === 'via') {
                    const d = Math.hypot(wx - it.at.x, wy - it.at.y);
                    if (d < (it.size / 2)) return it;
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

    window.EDA_Pcb = { PcbRenderer };
})();
