// SPDX-License-Identifier: GPL-3.0-or-later
// EDA frontend controller. Mounts against #eda-root. Talks to the
// /api/eda/* endpoints registered by 340_kicad_bridge/eda_routes.cpp.

(function () {
    'use strict';

    const $  = (sel, root) => (root || document).querySelector(sel);
    const $$ = (sel, root) => Array.from((root || document).querySelectorAll(sel));

    const state = {
        session_id: null,
        activeTab:  'schematic',
        sch_path:   null,
        pcb_path:   null,
        layers:     [],            // [{name, display, css_color, visible, svg_url}]
        violations: [],
        kicad:      { available: false }
    };

    // ---- API helpers ----------------------------------------------------

    async function api(path, opts) {
        opts = opts || {};
        const init = { method: opts.method || 'GET', headers: { 'Accept': 'application/json' } };
        if (opts.body !== undefined) {
            init.headers['Content-Type'] = 'application/json';
            init.body = typeof opts.body === 'string' ? opts.body : JSON.stringify(opts.body);
        }
        const r = await fetch(path, init);
        const j = await r.json().catch(() => ({ ok: false, error: 'bad JSON response' }));
        if (!r.ok || j.ok === false) {
            const e = new Error(j.error || ('HTTP ' + r.status));
            e.detail = j;
            throw e;
        }
        return j;
    }

    // ---- Status probe ---------------------------------------------------

    async function refreshStatus() {
        const dot = $('#eda-status .dot');
        const txt = $('#eda-status .text');
        try {
            const j = await api('/api/eda/kicad_status');
            state.kicad = j;
            if (j.available) {
                dot.className = 'dot ok';
                txt.textContent = 'KiCad ' + (j.version || 'ready');
            } else {
                dot.className = 'dot missing';
                txt.textContent = 'KiCad not found';
            }
            if (!j.libs_ready) {
                dot.className = 'dot warn';
                txt.textContent += ' (no libraries indexed)';
            }
        } catch (e) {
            dot.className = 'dot missing';
            txt.textContent = 'API error: ' + e.message;
        }
    }

    // ---- Library search -------------------------------------------------

    let searchTimer = null;
    async function doLibrarySearch(q) {
        const symUL = $('#eda-symbol-list');
        const fpUL  = $('#eda-footprint-list');
        if (!q) { symUL.innerHTML = ''; fpUL.innerHTML = ''; return; }
        try {
            const [sym, fp] = await Promise.all([
                api('/api/eda/symbol_search?q='   + encodeURIComponent(q) + '&limit=15'),
                api('/api/eda/footprint_search?q=' + encodeURIComponent(q) + '&limit=15')
            ]);
            symUL.innerHTML = sym.hits.map(h => `
                <li data-lib="${h.lib}" data-name="${h.name}" data-kind="symbol">
                    <div class="name">${h.name}</div>
                    <div class="lib">${h.lib}</div>
                    ${h.description ? `<div class="desc">${escapeHtml(h.description)}</div>` : ''}
                </li>`).join('');
            fpUL.innerHTML = fp.hits.map(h => `
                <li data-lib="${h.lib}" data-name="${h.name}" data-kind="footprint">
                    <div class="name">${h.name}</div>
                    <div class="lib">${h.lib} &middot; ${h.pad_count} pads${h.smd ? ' &middot; SMD' : ''}</div>
                    ${h.description ? `<div class="desc">${escapeHtml(h.description)}</div>` : ''}
                </li>`).join('');
        } catch (e) {
            symUL.innerHTML = `<li class="err">${escapeHtml(e.message)}</li>`;
            fpUL.innerHTML  = '';
        }
    }

    function escapeHtml(s) {
        return String(s)
            .replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')
            .replace(/"/g, '&quot;').replace(/'/g, '&#39;');
    }

    // ---- Viewer (pan + zoom for SVG content) ---------------------------

    function attachPanZoom(container) {
        let scale = 1, tx = 0, ty = 0, dragging = false, sx = 0, sy = 0;
        const svg = container.querySelector('svg');
        if (!svg) return;
        svg.style.transformOrigin = '0 0';
        function apply() { svg.style.transform = `translate(${tx}px, ${ty}px) scale(${scale})`; }
        apply();
        container.addEventListener('mousedown', e => {
            dragging = true; sx = e.clientX - tx; sy = e.clientY - ty;
            container.classList.add('dragging');
        });
        window.addEventListener('mouseup', () => {
            dragging = false; container.classList.remove('dragging');
        });
        window.addEventListener('mousemove', e => {
            if (!dragging) return;
            tx = e.clientX - sx; ty = e.clientY - sy; apply();
        });
        container.addEventListener('wheel', e => {
            e.preventDefault();
            const factor = e.deltaY < 0 ? 1.1 : 1/1.1;
            const rect = container.getBoundingClientRect();
            const px = e.clientX - rect.left, py = e.clientY - rect.top;
            tx = (tx - px) * factor + px;
            ty = (ty - py) * factor + py;
            scale *= factor;
            apply();
        }, { passive: false });
    }

    async function loadSVG(url) {
        const container = $('#eda-viewer');
        container.innerHTML = '<div style="padding:24px;color:#8a929c;">Loading&hellip;</div>';
        try {
            const r = await fetch(url);
            if (!r.ok) throw new Error('HTTP ' + r.status);
            const text = await r.text();
            container.innerHTML = text;
            attachPanZoom(container);
        } catch (e) {
            container.innerHTML = `<div style="padding:24px;color:#e05252;">${escapeHtml(e.message)}</div>`;
        }
    }

    // ---- Layer list -----------------------------------------------------

    function renderLayerList() {
        const ul = $('#eda-layer-list');
        ul.innerHTML = state.layers.map((L, i) => `
            <li class="${L.visible ? '' : 'hidden'}" data-idx="${i}">
                <span class="swatch" style="background:${L.css_color}"></span>
                <input type="checkbox" ${L.visible ? 'checked' : ''} />
                <span class="name">${escapeHtml(L.display)}</span>
            </li>`).join('');
        $$('#eda-layer-list li').forEach(li => {
            li.querySelector('input').addEventListener('change', e => {
                const idx = +li.dataset.idx;
                state.layers[idx].visible = e.target.checked;
                li.classList.toggle('hidden', !e.target.checked);
                paintCombinedLayers();
            });
        });
    }

    function paintCombinedLayers() {
        // Show only the SVGs for visible layers, stacked. For MVP the
        // simple approach is to load the combined SVG and toggle its
        // inner <g> visibility by layer id. When kicad-cli emits per-
        // layer SVGs individually, we stack them in DOM order and toggle
        // opacity per checkbox.
        const container = $('#eda-viewer');
        container.innerHTML = '';
        const stack = document.createElement('div');
        stack.style.position = 'relative';
        stack.style.width = 'fit-content';
        stack.style.margin = 'auto';
        container.appendChild(stack);
        state.layers.forEach((L, i) => {
            if (!L.svg_url) return;
            const img = document.createElement('img');
            img.src = L.svg_url;
            img.style.position = i === 0 ? 'static' : 'absolute';
            img.style.top = '0'; img.style.left = '0';
            img.style.mixBlendMode = 'screen';
            img.style.opacity = L.visible ? '1' : '0';
            img.style.pointerEvents = 'none';
            stack.appendChild(img);
        });
        attachPanZoom(container);
    }

    // ---- Violations -----------------------------------------------------

    function renderViolations() {
        const ul = $('#eda-violation-list');
        if (!state.violations.length) {
            ul.innerHTML = '<li style="border:0;background:transparent;color:#8a929c;">Clean.</li>';
            return;
        }
        ul.innerHTML = state.violations.map((v, i) => `
            <li class="${v.severity}" data-idx="${i}">
                <div class="type">${escapeHtml(v.type)}</div>
                <div class="desc">${escapeHtml(v.description)}</div>
            </li>`).join('');
    }

    // ---- Actions --------------------------------------------------------

    async function runERC() {
        if (!state.sch_path) return alert('No schematic loaded');
        const j = await api('/api/eda/erc', { method:'POST', body:{
            session_id: state.session_id, sch_path: state.sch_path
        }});
        state.violations = (j.report && j.report.violations) || [];
        renderViolations();
    }
    async function runDRC() {
        if (!state.pcb_path) return alert('No PCB loaded');
        const j = await api('/api/eda/drc', { method:'POST', body:{
            session_id: state.session_id, pcb_path: state.pcb_path, schematic_parity: false
        }});
        state.violations = ((j.report && j.report.violations) || [])
            .concat((j.report && j.report.unconnected_items) || []);
        renderViolations();
    }
    async function exportGerbers() {
        if (!state.pcb_path) return alert('No PCB loaded');
        const j = await api('/api/eda/export', { method:'POST', body:{
            session_id: state.session_id, pcb_path: state.pcb_path, kind: 'gerbers'
        }});
        alert('Gerbers written to: ' + j.output_path);
    }
    async function exportStep() {
        if (!state.pcb_path) return alert('No PCB loaded');
        const j = await api('/api/eda/export', { method:'POST', body:{
            session_id: state.session_id, pcb_path: state.pcb_path, kind: 'step'
        }});
        alert('STEP written to: ' + j.output_path);
    }
    async function bundleFab() {
        if (!state.pcb_path) return alert('No PCB loaded');
        const j = await api('/api/eda/bundle_fab', { method:'POST', body:{
            session_id: state.session_id, pcb_path: state.pcb_path
        }});
        alert('Fab bundle at: ' + j.bundle_path);
    }

    async function loadPCBLayers() {
        if (!state.pcb_path) return;
        const j = await api('/api/eda/render_layers', { method:'POST', body:{
            session_id: state.session_id, pcb_path: state.pcb_path
        }});
        state.layers = j.layers.map(L => ({
            name: L.name, display: L.display, css_color: L.css_color,
            visible: L.default_visible,
            svg_url: L.svg_path ? '/api/eda/file?path=' + encodeURIComponent(L.svg_path) : null
        }));
        renderLayerList();
        paintCombinedLayers();
    }

    // ---- Tab controller -------------------------------------------------

    function setActiveTab(name) {
        state.activeTab = name;
        $$('#eda-root .eda-tab').forEach(t => t.classList.toggle('active', t.dataset.tab === name));
        $('#eda-viewer').dataset.mode = name;

        if (name === 'schematic' && state.sch_path) {
            // Ask kicad-cli for the schematic SVG (one call, one file).
            api('/api/eda/export', { method:'POST', body:{
                session_id: state.session_id, pcb_path: state.sch_path, kind: 'svg'
            }}).then(j => loadSVG('/api/eda/file?path=' + encodeURIComponent(j.output_path)))
              .catch(() => {});
        } else if (name === 'pcb' && state.pcb_path) {
            loadPCBLayers();
        }
    }

    // ---- Public API -----------------------------------------------------

    async function mount(root) {
        if (!root) throw new Error('EDA.mount: no root element');

        // Load the panel HTML if it hasn't been baked into index.html.
        if (!$('#eda-status')) {
            try {
                const r = await fetch('/modules/010_interface/eda/eda_panels.html');
                if (r.ok) root.innerHTML = await r.text();
            } catch (_) {}
        }
        root.hidden = false;

        state.session_id = (typeof window.currentSessionId === 'function')
            ? window.currentSessionId()
            : (location.hash.slice(1) || 'default');

        // Wire up controls.
        $$('#eda-root .eda-tab').forEach(t => t.addEventListener('click', () => setActiveTab(t.dataset.tab)));
        $('#eda-lib-query').addEventListener('input', e => {
            clearTimeout(searchTimer);
            const q = e.target.value.trim();
            searchTimer = setTimeout(() => doLibrarySearch(q), 200);
        });
        $('#eda-run-erc').addEventListener('click',        () => runERC().catch(alert));
        $('#eda-run-drc').addEventListener('click',        () => runDRC().catch(alert));
        $('#eda-export-gerbers').addEventListener('click', () => exportGerbers().catch(alert));
        $('#eda-export-step').addEventListener('click',    () => exportStep().catch(alert));
        $('#eda-bundle-fab').addEventListener('click',     () => bundleFab().catch(alert));

        await refreshStatus();
        setActiveTab('schematic');
    }

    function openFile(path) {
        if (/\.kicad_sch$/i.test(path)) {
            state.sch_path = path;
            setActiveTab('schematic');
        } else if (/\.kicad_pcb$/i.test(path)) {
            state.pcb_path = path;
            setActiveTab('pcb');
        }
    }

    async function emitFromIntent(intent, title) {
        title = title || 'ac9_generated';
        // Project first (so ERC/DRC severities load correctly).
        await api('/api/eda/emit_project', { method:'POST', body:{
            session_id: state.session_id, title
        }});
        const sch = await api('/api/eda/emit_schematic', { method:'POST', body:{
            session_id: state.session_id, title, intent
        }});
        state.sch_path = sch.sch_path;
        const pcb = await api('/api/eda/emit_pcb', { method:'POST', body:{
            session_id: state.session_id, title, intent
        }});
        state.pcb_path = pcb.pcb_path;
        return { sch_path: state.sch_path, pcb_path: state.pcb_path,
                 sch_diagnostics: sch.diagnostics, pcb_diagnostics: pcb.diagnostics };
    }

    window.EDA = { mount, openFile, emitFromIntent, refreshStatus };
})();
