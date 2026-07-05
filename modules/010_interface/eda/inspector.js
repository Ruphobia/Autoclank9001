// SPDX-License-Identifier: GPL-3.0-or-later
// Property inspector: shows every field of the current selection and
// lets the user edit them. On blur (or Enter) it POSTs
// /api/eda/editor/edit_field.

(function () {
    'use strict';

    class Inspector {
        constructor(root, api) {
            this.root = root;
            this.api = api;
            this.selection = [];
            this.model = null;

            root.classList.add('eda-inspector');
            root.innerHTML = '<div class="eda-inspector-empty">Nothing selected.</div>';
        }
        setModel(m)      { this.model = m; this._render(); }
        setSelection(a)  { this.selection = a.slice(); this._render(); }

        _findItem(uuid) {
            if (!this.model || !this.model.root) return null;
            for (const it of (this.model.root.items || [])) if (it.uuid === uuid) return it;
            return null;
        }

        _render() {
            if (!this.selection.length) {
                this.root.innerHTML = '<div class="eda-inspector-empty">Nothing selected.</div>';
                return;
            }
            if (this.selection.length > 1) {
                this.root.innerHTML = `<div class="eda-inspector-empty">${this.selection.length} items selected.</div>`;
                return;
            }
            const it = this._findItem(this.selection[0]);
            if (!it) {
                this.root.innerHTML = '<div class="eda-inspector-empty">Item not in current model.</div>';
                return;
            }
            // Basic info block.
            const parts = [];
            parts.push(`<div class="eda-inspector-header">${it.kind || 'item'} <span class="uuid">${it.uuid || ''}</span></div>`);
            // Type-specific quick rows.
            if (it.kind === 'symbol') {
                parts.push(this._row('lib_id', it.lib_id, false));
                parts.push(this._row('angle', it.angle || 0, true, 'number'));
                parts.push(`<div class="eda-inspector-row">
                    <span class="eda-key">Unit</span>
                    <input type="number" min="1" max="8" value="${it.unit || 1}" data-unit="${it.uuid}">
                </div>`);
                parts.push(`<div class="eda-inspector-row">
                    <span class="eda-key">DNP</span>
                    <input type="checkbox" ${it.dnp ? 'checked' : ''} data-dnp="${it.uuid}">
                </div>`);
                parts.push(`<div class="eda-inspector-row">
                    <span class="eda-key">Mirror</span>
                    <label><input type="checkbox" ${it.mirror_x ? 'checked' : ''} data-mirror-x="${it.uuid}"> X</label>
                    <label><input type="checkbox" ${it.mirror_y ? 'checked' : ''} data-mirror-y="${it.uuid}"> Y</label>
                </div>`);
                parts.push('<div class="eda-inspector-section">Fields</div>');
                for (const f of (it.fields || [])) parts.push(this._fieldRow(it.uuid, f));
                parts.push(`<div class="eda-inspector-add"><button data-add-field="${it.uuid}">+ Field</button></div>`);
            } else if (it.kind === 'wire' || it.kind === 'bus') {
                parts.push(this._row('points', (it.pts || []).map(p => `(${(p.x/1e6).toFixed(3)}, ${(p.y/1e6).toFixed(3)})`).join(' '), false));
            } else if (it.kind === 'junction' || it.kind === 'no_connect') {
                parts.push(this._row('at', `${(it.at.x/1e6).toFixed(3)}, ${(it.at.y/1e6).toFixed(3)}`, false));
            } else if (it.kind === 'label' || it.kind === 'global_label' || it.kind === 'hier_label') {
                parts.push(this._row('text', it.text, false));
                parts.push(this._row('shape', it.shape || '', false));
            }
            this.root.innerHTML = parts.join('');

            this.root.querySelectorAll('input[data-field]').forEach(input => {
                input.addEventListener('change', ev => {
                    const uuid = input.getAttribute('data-uuid');
                    const field = input.getAttribute('data-field');
                    const val = input.value;
                    this.api.editField({ uuid, field, value: val });
                });
            });
            this.root.querySelectorAll('[data-unit]').forEach(input => {
                input.addEventListener('change', ev => {
                    this.api.editField({ uuid: input.dataset.unit, field: 'unit', value: input.value });
                });
            });
            this.root.querySelectorAll('[data-dnp]').forEach(input => {
                input.addEventListener('change', ev => {
                    this.api.editField({ uuid: input.dataset.dnp, field: 'dnp', value: input.checked ? 'yes' : 'no' });
                });
            });
            this.root.querySelectorAll('[data-mirror-x]').forEach(input => {
                input.addEventListener('change', ev => {
                    if (this.api.mirror) this.api.mirror({ uuid: input.dataset.mirrorX, axis: 'x' });
                });
            });
            this.root.querySelectorAll('[data-mirror-y]').forEach(input => {
                input.addEventListener('change', ev => {
                    if (this.api.mirror) this.api.mirror({ uuid: input.dataset.mirrorY, axis: 'y' });
                });
            });
            const addBtn = this.root.querySelector('[data-add-field]');
            if (addBtn) addBtn.addEventListener('click', () => {
                const name = prompt('Field name:');
                if (!name) return;
                const val = prompt('Field value:', '');
                if (val === null) return;
                this.api.editField({ uuid: addBtn.getAttribute('data-add-field'), field: name, value: val });
            });
        }

        _row(label, value, editable, type = 'text') {
            const val = String(value == null ? '' : value);
            const editHtml = editable
                ? `<input type="${type}" value="${escapeAttr(val)}" data-static="1">`
                : `<span class="eda-value">${escapeHtml(val)}</span>`;
            return `<div class="eda-inspector-row"><span class="eda-key">${label}</span>${editHtml}</div>`;
        }

        _fieldRow(uuid, f) {
            return `<div class="eda-inspector-row eda-field-row">
                <span class="eda-key">${escapeHtml(f.name)}</span>
                <input type="text" value="${escapeAttr(f.value || '')}" data-uuid="${uuid}" data-field="${escapeAttr(f.name)}">
            </div>`;
        }
    }

    function escapeHtml(s) {
        return String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
    }
    function escapeAttr(s) {
        return String(s).replace(/&/g, '&amp;').replace(/"/g, '&quot;').replace(/</g, '&lt;');
    }

    window.EDA_Inspector = { Inspector };
})();
