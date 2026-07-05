// SPDX-License-Identifier: GPL-3.0-or-later
// Fab output UI: plot dialog, BOM viewer, gerber preview panel.

(function () {
    'use strict';

    class PlotDialog {
        constructor(sessionId) {
            this.sessionId = sessionId;
            const modal = document.createElement('div');
            modal.className = 'eda-modal';
            modal.innerHTML = `
                <div class="eda-modal-inner">
                    <h3>Plot / Fabrication output</h3>
                    <div class="eda-layer-toggles" id="plot-layers">
                        ${['F.Cu','B.Cu','F.SilkS','B.SilkS','F.Mask','B.Mask','F.Paste','B.Paste','Edge.Cuts']
                            .map(L => `<label><input type="checkbox" value="${L}" checked> ${L}</label>`).join('')}
                    </div>
                    <div class="eda-fab-actions">
                        <button data-act="gerbers"    class="primary">Emit Gerbers</button>
                        <button data-act="drill">Emit Drill</button>
                        <button data-act="pos">Emit Pick-and-Place</button>
                        <button data-act="cancel">Close</button>
                    </div>
                </div>
            `;
            document.body.appendChild(modal);
            this.modal = modal;

            modal.addEventListener('click', ev => {
                const b = ev.target.closest('button');
                if (!b) return;
                if (b.dataset.act === 'cancel') { modal.remove(); return; }
                if (b.dataset.act === 'gerbers') this._gerbers();
                if (b.dataset.act === 'drill')   this._drill();
                if (b.dataset.act === 'pos')     this._pos();
            });
        }

        _selectedLayers() {
            return Array.from(this.modal.querySelectorAll('#plot-layers input:checked'))
                        .map(i => i.value);
        }

        async _gerbers() {
            const layers = this._selectedLayers();
            for (const L of layers) {
                const j = await fetch('/api/eda/editor/write_fab', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ session_id: this.sessionId, kind: 'gerber', layer: L })
                }).then(r => r.json());
                if (j.ok && j.text) this._download(L.replace('.', '_') + '.gbr', j.text);
            }
        }
        async _drill() {
            const j = await fetch('/api/eda/editor/write_fab', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ session_id: this.sessionId, kind: 'drill_pth' })
            }).then(r => r.json());
            if (j.ok && j.text) this._download('board.drl', j.text);
        }
        async _pos() {
            const j = await fetch('/api/eda/editor/write_fab', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ session_id: this.sessionId, kind: 'pos' })
            }).then(r => r.json());
            if (j.ok && j.text) this._download('pos.csv', j.text);
        }

        _download(name, text) {
            const blob = new Blob([text], { type: 'text/plain' });
            const url  = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url; a.download = name; a.click();
            URL.revokeObjectURL(url);
        }
    }

    class BomView {
        constructor(sessionId) {
            this.sessionId = sessionId;
            const modal = document.createElement('div');
            modal.className = 'eda-modal';
            modal.innerHTML = `
                <div class="eda-modal-inner">
                    <h3>Bill of Materials</h3>
                    <div class="eda-bom-toolbar">
                        <label><input type="checkbox" id="bom-exclude-dnp"> Exclude DNP</label>
                        <label>Combine:
                            <select id="bom-combine">
                                <option value="list">R1, R2, R3</option>
                                <option value="range">R1-R3</option>
                            </select>
                        </label>
                        <button data-act="refresh">Refresh</button>
                        <button data-act="csv">CSV</button>
                        <button data-act="html">HTML</button>
                        <button data-act="close">Close</button>
                    </div>
                    <div class="eda-bom-body" id="bom-body">Loading&hellip;</div>
                </div>
            `;
            document.body.appendChild(modal);
            this.modal = modal;
            modal.addEventListener('click', ev => {
                const b = ev.target.closest('button');
                if (!b) return;
                if (b.dataset.act === 'close') { modal.remove(); return; }
                if (b.dataset.act === 'refresh') this._render();
                if (b.dataset.act === 'csv' || b.dataset.act === 'html') this._export(b.dataset.act);
            });
            this._render();
        }
        async _render() {
            const j = await fetch('/api/eda/editor/bom', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    session_id: this.sessionId,
                    format: 'html',
                    exclude_dnp: this.modal.querySelector('#bom-exclude-dnp').checked,
                    combine:    this.modal.querySelector('#bom-combine').value
                })
            }).then(r => r.json());
            this.modal.querySelector('#bom-body').innerHTML = (j && j.text) || '<em>Failed to render.</em>';
        }
        async _export(fmt) {
            const j = await fetch('/api/eda/editor/bom', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    session_id: this.sessionId, format: fmt,
                    exclude_dnp: this.modal.querySelector('#bom-exclude-dnp').checked,
                    combine:    this.modal.querySelector('#bom-combine').value
                })
            }).then(r => r.json());
            if (!(j && j.text)) return;
            const blob = new Blob([j.text], { type: fmt === 'csv' ? 'text/csv' : 'text/html' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url; a.download = 'bom.' + fmt; a.click();
            URL.revokeObjectURL(url);
        }
    }

    window.EDA_Fab = { PlotDialog, BomView };
})();
