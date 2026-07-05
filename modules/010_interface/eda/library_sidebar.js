// SPDX-License-Identifier: GPL-3.0-or-later
// Symbol and footprint library sidebar.
// Fetches /api/eda/symbol_search + /api/eda/footprint_search as the
// user types; drag-to-place drops a placeholder symbol/footprint onto
// the canvas at the drop point.

(function () {
    'use strict';

    class LibrarySidebar {
        constructor(root, editorApi) {
            this.root = root;
            this.api = editorApi;
            this.timer = null;

            root.classList.add('eda-lib-sidebar');
            root.innerHTML = `
                <input type="search" placeholder="Search symbols &amp; footprints&hellip;" class="eda-lib-q">
                <div class="eda-lib-group-title">Symbols</div>
                <ul class="eda-lib-list" data-kind="symbol"></ul>
                <div class="eda-lib-group-title">Footprints</div>
                <ul class="eda-lib-list" data-kind="footprint"></ul>
            `;

            this.qEl    = root.querySelector('.eda-lib-q');
            this.symUL  = root.querySelector('.eda-lib-list[data-kind="symbol"]');
            this.fpUL   = root.querySelector('.eda-lib-list[data-kind="footprint"]');

            this.qEl.addEventListener('input', () => {
                clearTimeout(this.timer);
                this.timer = setTimeout(() => this._search(this.qEl.value.trim()), 180);
            });

            [this.symUL, this.fpUL].forEach(ul => {
                ul.addEventListener('dragstart', ev => {
                    const li = ev.target.closest('li');
                    if (!li) return;
                    const payload = {
                        kind: ul.dataset.kind,
                        lib:  li.dataset.lib,
                        name: li.dataset.name
                    };
                    ev.dataTransfer.setData('application/x-eda-lib', JSON.stringify(payload));
                    ev.dataTransfer.effectAllowed = 'copy';
                });
                ul.addEventListener('click', ev => {
                    const li = ev.target.closest('li');
                    if (!li) return;
                    // Click alternative: switch to the appropriate placement
                    // tool preloaded with this lib_id.
                    const lib_id = li.dataset.lib + ':' + li.dataset.name;
                    if (ul.dataset.kind === 'symbol') this.api.setPlaceSymbolProto({ lib_id, reference: 'U?', value: li.dataset.name });
                    else                              this.api.setPlaceFootprintProto({ lib_id, reference: 'U?', value: '' });
                    this.api.setTool(ul.dataset.kind === 'symbol' ? 'place_symbol' : 'place_footprint');
                });
            });
        }

        attachDropTarget(canvasEl, scene) {
            canvasEl.addEventListener('dragover', ev => {
                if (!ev.dataTransfer.types.includes('application/x-eda-lib')) return;
                ev.preventDefault();
                ev.dataTransfer.dropEffect = 'copy';
            });
            canvasEl.addEventListener('drop', ev => {
                ev.preventDefault();
                const raw = ev.dataTransfer.getData('application/x-eda-lib');
                if (!raw) return;
                let payload; try { payload = JSON.parse(raw); } catch (_) { return; }
                const rect = canvasEl.getBoundingClientRect();
                const px = ev.clientX - rect.left, py = ev.clientY - rect.top;
                const wx = scene.view.toWorldX(px), wy = scene.view.toWorldY(py);
                const [gx, gy] = scene.snapPoint(wx, wy);
                const lib_id = payload.lib + ':' + payload.name;
                if (payload.kind === 'symbol') {
                    this.api.addSchItem({
                        kind: 'symbol', lib_id, reference: 'U?', value: payload.name,
                        at: [gx / 1e6, gy / 1e6], angle: 0
                    });
                } else {
                    this.api.addPcbItem({
                        kind: 'footprint', lib_id, reference: 'U?', value: '',
                        at: [gx / 1e6, gy / 1e6], angle: 0, layer: 'F.Cu'
                    });
                }
            });
        }

        async _search(q) {
            if (!q) { this.symUL.innerHTML = ''; this.fpUL.innerHTML = ''; return; }
            try {
                const [sym, fp] = await Promise.all([
                    fetch('/api/eda/symbol_search?q='    + encodeURIComponent(q) + '&limit=15').then(r => r.json()),
                    fetch('/api/eda/footprint_search?q=' + encodeURIComponent(q) + '&limit=15').then(r => r.json())
                ]);
                this._paint(this.symUL, (sym.hits || []).map(h => ({
                    lib: h.lib, name: h.name, description: h.description, tag: null
                })));
                this._paint(this.fpUL,  (fp.hits  || []).map(h => ({
                    lib: h.lib, name: h.name, description: h.description,
                    tag: (h.pad_count ? h.pad_count + ' pads' : '') + (h.smd ? ' SMD' : '')
                })));
            } catch (e) {
                this.symUL.innerHTML = `<li class="err">${e.message}</li>`;
                this.fpUL.innerHTML  = '';
            }
        }
        _paint(ul, items) {
            ul.innerHTML = items.map(h => `
                <li draggable="true" data-lib="${escape(h.lib)}" data-name="${escape(h.name)}">
                    <div class="name">${escape(h.name)}</div>
                    <div class="lib">${escape(h.lib)}${h.tag ? ' &middot; ' + escape(h.tag) : ''}</div>
                    ${h.description ? `<div class="desc">${escape(h.description)}</div>` : ''}
                </li>
            `).join('');
        }
    }

    function escape(s) {
        return String(s || '').replace(/&/g, '&amp;').replace(/</g, '&lt;')
                              .replace(/>/g, '&gt;').replace(/"/g, '&quot;');
    }

    window.EDA_LibSidebar = { LibrarySidebar };
})();
