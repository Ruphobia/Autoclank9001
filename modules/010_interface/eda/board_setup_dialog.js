// SPDX-License-Identifier: GPL-3.0-or-later
// Board setup dialog. Talks to /api/eda/editor/board_setup (GET + PUT).
// Presents editable groups for design rules, layers, and netclasses.

(function () {
    'use strict';

    class BoardSetupDialog {
        constructor(sessionId) {
            this.sessionId = sessionId;
            this.setup = null;

            const modal = document.createElement('div');
            modal.className = 'eda-modal';
            modal.innerHTML = `
                <div class="eda-modal-inner">
                    <h3>Board setup</h3>
                    <div class="eda-setup-body" id="setup-body">Loading&hellip;</div>
                    <div class="eda-fab-actions">
                        <button data-act="save" class="primary">Save</button>
                        <button data-act="cancel">Close</button>
                    </div>
                </div>`;
            document.body.appendChild(modal);
            this.modal = modal;
            modal.addEventListener('click', ev => {
                const b = ev.target.closest('button');
                if (!b) return;
                if (b.dataset.act === 'cancel') { modal.remove(); return; }
                if (b.dataset.act === 'save')   this._save();
            });
            this._load();
        }

        async _load() {
            const j = await fetch('/api/eda/editor/board_setup', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ session_id: this.sessionId })
            }).then(r => r.json()).catch(e => ({ ok: false, error: e.message }));
            if (!j.ok) { this.modal.querySelector('#setup-body').textContent = j.error || 'load failed'; return; }
            this.setup = JSON.parse(j.text || '{}');
            this._render();
        }

        _render() {
            const s = this.setup;
            const rows = [
                ['Board thickness (mm)',           'board_thickness_mm'],
                ['Minimum clearance (mm)',         'min_clearance_mm'],
                ['Minimum track width (mm)',       'min_track_width_mm'],
                ['Minimum via diameter (mm)',      'min_via_diameter_mm'],
                ['Minimum via drill (mm)',         'min_via_drill_mm'],
                ['Minimum hole clearance (mm)',    'min_hole_clearance_mm'],
                ['Minimum hole-to-hole (mm)',      'min_hole_to_hole_mm'],
                ['Copper edge clearance (mm)',     'min_copper_edge_clearance_mm']
            ];
            const cls = (s.classes || []).map((c, i) => `
                <div class="eda-setup-class" data-idx="${i}">
                    <strong>${escape(c.name)}</strong>
                    <div class="eda-inspector-row"><span class="eda-key">Clearance (mm)</span>
                        <input type="number" step="0.01" value="${c.clearance_mm}" data-field="clearance_mm"></div>
                    <div class="eda-inspector-row"><span class="eda-key">Track width (mm)</span>
                        <input type="number" step="0.01" value="${c.track_width_mm}" data-field="track_width_mm"></div>
                    <div class="eda-inspector-row"><span class="eda-key">Via diameter (mm)</span>
                        <input type="number" step="0.01" value="${c.via_diameter_mm}" data-field="via_diameter_mm"></div>
                    <div class="eda-inspector-row"><span class="eda-key">Via drill (mm)</span>
                        <input type="number" step="0.01" value="${c.via_drill_mm}" data-field="via_drill_mm"></div>
                    <div class="eda-inspector-row"><span class="eda-key">Patterns (comma-sep)</span>
                        <input type="text" value="${(c.patterns || []).map(escape).join(', ')}" data-field="patterns"></div>
                </div>`).join('');

            const layerRows = (s.layers || []).map((L, i) => `
                <div class="eda-inspector-row">
                    <span class="eda-key">${escape(L.canonical || L.canonical_name || '')}</span>
                    <select data-layer-idx="${i}">
                        <option value="signal" ${L.type === 'signal' ? 'selected' : ''}>signal</option>
                        <option value="power"  ${L.type === 'power'  ? 'selected' : ''}>power</option>
                        <option value="mixed"  ${L.type === 'mixed'  ? 'selected' : ''}>mixed</option>
                        <option value="user"   ${L.type === 'user'   ? 'selected' : ''}>user</option>
                    </select>
                </div>`).join('');

            this.modal.querySelector('#setup-body').innerHTML = `
                <div class="eda-inspector-section">Design rules</div>
                ${rows.map(([lbl, field]) => `
                    <div class="eda-inspector-row">
                        <span class="eda-key">${lbl}</span>
                        <input type="number" step="0.001" value="${s[field] ?? ''}" data-rule="${field}">
                    </div>
                `).join('')}
                <div class="eda-inspector-row">
                    <span class="eda-key">Allow blind/buried vias</span>
                    <input type="checkbox" ${s.allow_blind_buried_vias ? 'checked' : ''} data-rule="allow_blind_buried_vias">
                </div>
                <div class="eda-inspector-row">
                    <span class="eda-key">Allow microvias</span>
                    <input type="checkbox" ${s.allow_microvias ? 'checked' : ''} data-rule="allow_microvias">
                </div>
                <div class="eda-inspector-section">Netclasses</div>
                <div id="setup-classes">${cls}</div>
                <div class="eda-inspector-section">Layers</div>
                ${layerRows}
            `;
        }

        _collect() {
            const s = Object.assign({}, this.setup);
            for (const el of this.modal.querySelectorAll('[data-rule]')) {
                const f = el.dataset.rule;
                if (el.type === 'checkbox') s[f] = el.checked;
                else if (el.type === 'number') s[f] = parseFloat(el.value);
            }
            s.classes = Array.from(this.modal.querySelectorAll('.eda-setup-class')).map(row => {
                const idx = +row.dataset.idx;
                const base = (this.setup.classes && this.setup.classes[idx]) || {};
                const c = Object.assign({}, base);
                for (const el of row.querySelectorAll('[data-field]')) {
                    const f = el.dataset.field;
                    if (f === 'patterns') c[f] = el.value.split(',').map(s => s.trim()).filter(Boolean);
                    else c[f] = parseFloat(el.value);
                }
                return c;
            });
            s.layers = (this.setup.layers || []).slice();
            for (const el of this.modal.querySelectorAll('[data-layer-idx]')) {
                s.layers[+el.dataset.layerIdx].type = el.value;
            }
            return s;
        }

        async _save() {
            const s = this._collect();
            const j = await fetch('/api/eda/editor/board_setup', {
                method: 'PUT',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ session_id: this.sessionId, text: JSON.stringify(s) })
            }).then(r => r.json()).catch(e => ({ ok: false, error: e.message }));
            if (!j.ok) { alert('Save failed: ' + (j.error || 'unknown')); return; }
            this.modal.remove();
        }
    }

    function escape(s) {
        return String(s || '').replace(/&/g, '&amp;').replace(/</g, '&lt;')
                              .replace(/>/g, '&gt;').replace(/"/g, '&quot;');
    }

    window.EDA_BoardSetup = { BoardSetupDialog };
})();
