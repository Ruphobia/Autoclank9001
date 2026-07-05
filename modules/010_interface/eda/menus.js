// SPDX-License-Identifier: GPL-3.0-or-later
// Menu bar + keyboard shortcuts.
// Shortcuts follow KiCad's own defaults where they don't collide with
// browser navigation (browser wins on Ctrl+W, Ctrl+T, Ctrl+N).

(function () {
    'use strict';

    const KEY = {
        undo:     'ctrl+z',
        redo:     'ctrl+y',
        redoAlt:  'ctrl+shift+z',
        save:     'ctrl+s',
        selectAll:'ctrl+a',
        del:      'delete',
        delMac:   'backspace',
        rotate:   'r',
        mirrorX:  'x',
        mirrorY:  'y',
        select:   's',
        wire:     'w',
        label:    'l',
        globalLabel: 'ctrl+l',
        hierLabel: 'h',
        junction: 'j',
        noConnect: 'q',
        placeSymbol: 'a',
        placeFootprint: 'a',
        drawTrack: 'x',
        drawVia:   'v',
        escape:    'escape',
        gridToggle:  'g',
        zoomFit:     '/',
        toggleMode:  'tab'
    };

    class MenuBar {
        constructor(root, api) {
            this.root = root;
            this.api = api;
            this._build();
            this._bindKeys();
        }

        _build() {
            this.root.classList.add('eda-menubar');
            this.root.innerHTML = `
                <div class="eda-menu" data-menu="file">File
                    <div class="eda-menu-list">
                        <div data-action="new-sch">New schematic</div>
                        <div data-action="open">Open&hellip;</div>
                        <div data-action="save">Save (Ctrl+S)</div>
                        <div data-action="save-as">Save as&hellip;</div>
                        <div class="sep"></div>
                        <div data-action="export-gerbers">Export Gerbers</div>
                        <div data-action="export-drill">Export Drill</div>
                        <div data-action="export-pos">Export Pick-and-Place</div>
                    </div>
                </div>
                <div class="eda-menu" data-menu="edit">Edit
                    <div class="eda-menu-list">
                        <div data-action="undo">Undo (Ctrl+Z)</div>
                        <div data-action="redo">Redo (Ctrl+Y)</div>
                        <div class="sep"></div>
                        <div data-action="cut">Cut (Ctrl+X)</div>
                        <div data-action="copy">Copy (Ctrl+C)</div>
                        <div data-action="paste">Paste (Ctrl+V)</div>
                        <div data-action="select-all">Select all (Ctrl+A)</div>
                        <div class="sep"></div>
                        <div data-action="delete">Delete (Del)</div>
                    </div>
                </div>
                <div class="eda-menu" data-menu="place">Place
                    <div class="eda-menu-list">
                        <div data-action="tool-symbol">Symbol (A)</div>
                        <div data-action="tool-wire">Wire (W)</div>
                        <div data-action="tool-label">Label (L)</div>
                        <div data-action="tool-global-label">Global label (Ctrl+L)</div>
                        <div data-action="tool-hier-label">Hierarchical label (H)</div>
                        <div data-action="tool-junction">Junction (J)</div>
                        <div data-action="tool-no-connect">No-connect (Q)</div>
                    </div>
                </div>
                <div class="eda-menu" data-menu="route">Route
                    <div class="eda-menu-list">
                        <div data-action="tool-track">Track (X)</div>
                        <div data-action="tool-via">Via (V)</div>
                        <div data-action="tool-footprint">Footprint (A)</div>
                    </div>
                </div>
                <div class="eda-menu" data-menu="inspect">Inspect
                    <div class="eda-menu-list">
                        <div data-action="run-erc">Run ERC</div>
                        <div data-action="run-drc">Run DRC</div>
                        <div data-action="derive-netlist">Derive netlist</div>
                    </div>
                </div>
                <div class="eda-menu" data-menu="view">View
                    <div class="eda-menu-list">
                        <div data-action="fit">Fit to page (/)</div>
                        <div data-action="grid-toggle">Toggle grid (G)</div>
                        <div data-action="mode-toggle">Toggle sch/pcb (Tab)</div>
                    </div>
                </div>
            `;
            this.root.addEventListener('click', ev => {
                const el = ev.target.closest('[data-action]');
                if (!el) return;
                this._act(el.getAttribute('data-action'));
            });
        }

        _act(action) {
            const a = this.api;
            switch (action) {
                case 'new-sch':          a.newSchematic();                       break;
                case 'open':             a.open();                               break;
                case 'save':             a.save();                               break;
                case 'save-as':          a.saveAs();                             break;
                case 'export-gerbers':   a.exportKind('gerbers');                break;
                case 'export-drill':     a.exportKind('drill');                  break;
                case 'export-pos':       a.exportKind('pos');                    break;
                case 'undo':             a.undo();                               break;
                case 'redo':             a.redo();                               break;
                case 'select-all':       a.selectAll();                          break;
                case 'delete':           a.deleteSelected();                     break;
                case 'tool-symbol':      a.setTool('place_symbol');              break;
                case 'tool-wire':        a.setTool('wire');                      break;
                case 'tool-label':       a.setTool('label');                     break;
                case 'tool-global-label':a.setTool('global_label');              break;
                case 'tool-hier-label':  a.setTool('hier_label');                break;
                case 'tool-junction':    a.setTool('junction');                  break;
                case 'tool-no-connect':  a.setTool('no_connect');                break;
                case 'tool-track':       a.setTool('track');                     break;
                case 'tool-via':         a.setTool('via');                       break;
                case 'tool-footprint':   a.setTool('place_footprint');           break;
                case 'run-erc':          a.runErc();                             break;
                case 'run-drc':          a.runDrc();                             break;
                case 'derive-netlist':   a.deriveNetlist();                      break;
                case 'fit':              a.fit();                                break;
                case 'grid-toggle':      a.toggleGrid();                         break;
                case 'mode-toggle':      a.toggleMode();                         break;
            }
        }

        _bindKeys() {
            document.addEventListener('keydown', ev => {
                // Don't intercept while user is typing in an input.
                const tag = (document.activeElement || {}).tagName;
                if (tag === 'INPUT' || tag === 'TEXTAREA') return;

                const combo = this._combo(ev);
                switch (combo) {
                    case KEY.undo:         this.api.undo();  ev.preventDefault(); break;
                    case KEY.redo:
                    case KEY.redoAlt:      this.api.redo();  ev.preventDefault(); break;
                    case KEY.save:         this.api.save();  ev.preventDefault(); break;
                    case KEY.selectAll:    this.api.selectAll(); ev.preventDefault(); break;
                    case KEY.del:
                    case KEY.delMac:       this.api.deleteSelected(); ev.preventDefault(); break;
                    case KEY.rotate:       this.api.rotateSelected(90);              break;
                    case KEY.mirrorX:      this.api.setTool('track');                break;   // overloaded: pcb mode
                    case KEY.wire:         this.api.setTool('wire');                 break;
                    case KEY.label:        this.api.setTool('label');                break;
                    case KEY.globalLabel:  this.api.setTool('global_label'); ev.preventDefault(); break;
                    case KEY.hierLabel:    this.api.setTool('hier_label');           break;
                    case KEY.junction:     this.api.setTool('junction');             break;
                    case KEY.noConnect:    this.api.setTool('no_connect');           break;
                    case KEY.placeSymbol:  this.api.setTool(this.api.mode() === 'pcb' ? 'place_footprint' : 'place_symbol'); break;
                    case KEY.drawVia:      this.api.setTool('via');                  break;
                    case KEY.escape:       this.api.setTool('select');               break;
                    case KEY.gridToggle:   this.api.toggleGrid();                    break;
                    case KEY.zoomFit:      this.api.fit();                           break;
                    case KEY.toggleMode:   this.api.toggleMode(); ev.preventDefault(); break;
                }
            });
        }

        _combo(ev) {
            const parts = [];
            if (ev.ctrlKey || ev.metaKey) parts.push('ctrl');
            if (ev.shiftKey) parts.push('shift');
            if (ev.altKey) parts.push('alt');
            parts.push(ev.key.toLowerCase());
            return parts.join('+');
        }
    }

    window.EDA_Menus = { MenuBar };
})();
