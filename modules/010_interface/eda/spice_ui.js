// SPDX-License-Identifier: GPL-3.0-or-later
// SPICE simulator UI. Talks to /api/eda/spice/run.
// Plots waveforms on a lightweight canvas (no external chart lib).

(function () {
    'use strict';

    class SpicePanel {
        constructor(root) {
            this.root = root;
            root.classList.add('eda-spice-panel');
            root.innerHTML = `
                <div class="eda-spice-controls">
                    <label>Netlist source:
                        <select id="sp-source">
                            <option value="derived">From current schematic</option>
                            <option value="paste">Paste netlist</option>
                        </select>
                    </label>
                    <label>Preset:
                        <select id="sp-preset">
                            <option value="">Custom</option>
                            <option value="tran">Transient</option>
                            <option value="ac">AC sweep</option>
                            <option value="dc">DC sweep</option>
                            <option value="op">Operating point</option>
                            <option value="noise">Noise</option>
                            <option value="tf">Transfer function</option>
                            <option value="sens">Sensitivity</option>
                            <option value="disto">Distortion</option>
                        </select>
                    </label>
                    <label>Analysis:
                        <input id="sp-analysis" type="text" value="tran 10u 5m" style="width:280px">
                    </label>
                    <button id="sp-run" class="primary">Run</button>
                </div>
                <textarea id="sp-netlist" placeholder="* SPICE netlist" style="display:none; width:100%; height:120px; font-family:monospace;"></textarea>
                <div class="eda-spice-signals" id="sp-signals"></div>
                <canvas id="sp-plot" style="width:100%; height:280px; background:#131518;"></canvas>
                <pre class="eda-spice-log" id="sp-log"></pre>
            `;
            this._bind();
            this.signals = [];
            this.visible = new Set();
        }

        _bind() {
            const source   = this.root.querySelector('#sp-source');
            const paste    = this.root.querySelector('#sp-netlist');
            const preset   = this.root.querySelector('#sp-preset');
            const analysis = this.root.querySelector('#sp-analysis');
            source.addEventListener('change', () => {
                paste.style.display = source.value === 'paste' ? 'block' : 'none';
            });
            const PRESETS = {
                tran:  'tran 10u 5m',
                ac:    'ac dec 10 1 10meg',
                dc:    'dc V1 0 5 0.1',
                op:    'op',
                noise: 'noise v(out) V1 dec 10 1 100k',
                tf:    'tf v(out) V1',
                sens:  'sens v(out)',
                disto: 'disto dec 10 1 100k'
            };
            preset.addEventListener('change', () => {
                const p = preset.value;
                if (p && PRESETS[p]) analysis.value = PRESETS[p];
            });
            this.root.querySelector('#sp-run').addEventListener('click', () => this._run());
        }

        async _run() {
            const source = this.root.querySelector('#sp-source').value;
            const analysis = this.root.querySelector('#sp-analysis').value;
            let netlist = '';
            if (source === 'paste') {
                netlist = this.root.querySelector('#sp-netlist').value;
            } else {
                // Ask the editor to derive a SPICE netlist from the current sch.
                const j = await fetch('/api/eda/editor/derive_netlist', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ session_id: 'default', format: 'spice', analysis: '' })
                }).then(r => r.json()).catch(() => ({}));
                netlist = (j && j.text) || '';
            }
            if (!netlist) { alert('No netlist to simulate.'); return; }
            const j = await fetch('/api/eda/spice_run', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ netlist, analysis })
            }).then(r => r.json()).catch(e => ({ ok: false, error: e.message }));
            this.root.querySelector('#sp-log').textContent = (j.log || '') + (j.error ? '\n' + j.error : '');
            this.signals = (j.signals || []).filter(s => s.values && s.values.length > 1);
            this.visible = new Set(this.signals.map(s => s.name));
            this._renderSignalList();
            this._plot();
        }

        _renderSignalList() {
            const ul = this.root.querySelector('#sp-signals');
            ul.innerHTML = this.signals.map(s => `
                <label>
                    <input type="checkbox" ${this.visible.has(s.name) ? 'checked' : ''} data-name="${s.name}">
                    ${s.name} <span class="len">(${s.values.length})</span>
                </label>`).join('');
            ul.addEventListener('change', ev => {
                if (!ev.target.dataset.name) return;
                if (ev.target.checked) this.visible.add(ev.target.dataset.name);
                else                    this.visible.delete(ev.target.dataset.name);
                this._plot();
            });
        }

        _plot() {
            const canvas = this.root.querySelector('#sp-plot');
            const rect = canvas.getBoundingClientRect();
            canvas.width  = rect.width;
            canvas.height = rect.height;
            const ctx = canvas.getContext('2d');
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            if (!this.signals.length) return;

            const time = this.signals.find(s => s.name.toLowerCase() === 'time');
            if (!time) { ctx.fillStyle = '#c00'; ctx.fillText('no time vector', 20, 20); return; }
            const N = time.values.length;
            const t0 = time.values[0], t1 = time.values[N - 1];
            const width = canvas.width - 40, height = canvas.height - 20;

            const active = this.signals.filter(s => s !== time && this.visible.has(s.name));
            if (!active.length) return;
            let ymin = Infinity, ymax = -Infinity;
            for (const s of active) for (const v of s.values) {
                if (v < ymin) ymin = v; if (v > ymax) ymax = v;
            }
            const yspan = (ymax - ymin) || 1;
            const px = t => 20 + (t - t0) / (t1 - t0 || 1) * width;
            const py = v => canvas.height - 10 - ((v - ymin) / yspan) * height;

            // Axes.
            ctx.strokeStyle = '#3a3f45';
            ctx.beginPath();
            ctx.moveTo(20, canvas.height - 10);
            ctx.lineTo(20 + width, canvas.height - 10);
            ctx.moveTo(20, canvas.height - 10);
            ctx.lineTo(20, canvas.height - 10 - height);
            ctx.stroke();

            // Series.
            const colors = ['#c88a17','#4d7fc4','#4bb051','#a020f0','#c9a20e','#e05252'];
            active.forEach((s, i) => {
                ctx.strokeStyle = colors[i % colors.length];
                ctx.beginPath();
                for (let k = 0; k < N; ++k) {
                    const x = px(time.values[k]);
                    const y = py(s.values[k]);
                    if (k === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
                }
                ctx.stroke();
                ctx.fillStyle = colors[i % colors.length];
                ctx.fillText(s.name, 30, 20 + i * 14);
            });
        }
    }

    window.EDA_Spice = { SpicePanel };
})();
