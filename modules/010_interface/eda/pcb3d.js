// SPDX-License-Identifier: GPL-3.0-or-later
// 3D board viewer scaffold.
// Uses three.js. Loaded on demand; if three.js is not on the page the
// class throws and falls back to the kicad-cli 3D PNG render already
// covered by 340_kicad_bridge::pcb_render_png.
//
// The model:
//   * Board outline extruded from Edge.Cuts polyline
//   * Copper layers (F.Cu / B.Cu) as thin planes
//   * Footprints as textured/plain plates on the copper layer
//   * Pads as small extruded cylinders (drill hole cut)
//   * Vias as full-thickness cylinders

(function () {
    'use strict';

    function ensureThree() {
        if (typeof THREE === 'undefined')
            throw new Error('three.js not loaded; include three.min.js on the page.');
    }

    class Pcb3D {
        constructor(root, board, options) {
            ensureThree();
            this.root = root;
            this.board = board || { items: [], layers: [] };
            this.raytrace = !!(options && options.raytrace);
            this.customMeshes = new Map();  // lib_id -> BufferGeometry from /api/eda/editor/mesh_summary
            this._extraGroup = new THREE.Group();

            const rect = root.getBoundingClientRect();
            this.scene = new THREE.Scene();
            this.scene.background = new THREE.Color('#0f1114');

            this.camera = new THREE.PerspectiveCamera(45, rect.width / rect.height, 0.1, 5000);
            this.camera.position.set(80, 80, 80);
            this.camera.lookAt(0, 0, 0);

            this.renderer = new THREE.WebGLRenderer({ antialias: true });
            this.renderer.setSize(rect.width, rect.height);
            root.appendChild(this.renderer.domElement);

            const amb = new THREE.AmbientLight(0xffffff, 0.6);
            const dir = new THREE.DirectionalLight(0xffffff, 0.7);
            dir.position.set(50, 100, 50);
            this.scene.add(amb, dir);

            this._buildBoardMesh();
            this._bindControls();
            this._loop();
            this.scene.add(this._extraGroup);
        }

        // Toggle raytraced mode (approximate: adds soft shadows and
        // adjusts materials). For a real path tracer, downstream can
        // swap in a WebGL2 render target with an accumulation buffer.
        setRaytrace(v) {
            this.raytrace = v;
            this.renderer.shadowMap.enabled = v;
            this.renderer.shadowMap.type = v ? THREE.PCFSoftShadowMap : THREE.BasicShadowMap;
            this.scene.traverse(o => {
                if (o.isMesh) {
                    o.castShadow    = v;
                    o.receiveShadow = v;
                    if (o.material) o.material.roughness = v ? 0.85 : 0.5;
                }
            });
        }

        // Attach a custom mesh from mesh_load JSON to every footprint
        // whose lib_id matches. Mesh JSON schema: {vertices:[x,y,z,...],
        // indices:[i,...], bbox:{lo:[x,y,z],hi:[x,y,z]}}.
        attachMeshForLibId(lib_id, mesh_json) {
            const geom = new THREE.BufferGeometry();
            const V = mesh_json.vertices || [];
            const I = mesh_json.indices  || [];
            geom.setAttribute('position', new THREE.Float32BufferAttribute(V, 3));
            geom.setIndex(I);
            geom.computeVertexNormals();
            this.customMeshes.set(lib_id, geom);
            const mat = new THREE.MeshStandardMaterial({ color: 0xcccccc, roughness: 0.6 });
            for (const it of this.board.items || []) {
                if (it.kind !== 'footprint' || it.lib_id !== lib_id) continue;
                const m = new THREE.Mesh(geom, mat);
                m.position.set(it.at.x / 1e6, 0.05, it.at.y / 1e6);
                m.rotation.y = (it.angle || 0) * Math.PI / 180;
                this._extraGroup.add(m);
            }
        }

        // Compute a simple bbox in mm from Edge.Cuts + footprint origins.
        _boardBBox() {
            let lo_x = 0, lo_y = 0, hi_x = 50, hi_y = 30;
            let init = false;
            for (const it of this.board.items || []) {
                if (it.kind === 'gr_line' && it.layer === 'Edge.Cuts') {
                    const pts = [it.start, it.end];
                    for (const p of pts) {
                        const x = p.x / 1e6, y = p.y / 1e6;
                        if (!init) { lo_x = hi_x = x; lo_y = hi_y = y; init = true; }
                        lo_x = Math.min(lo_x, x); hi_x = Math.max(hi_x, x);
                        lo_y = Math.min(lo_y, y); hi_y = Math.max(hi_y, y);
                    }
                }
            }
            return { lo_x, lo_y, hi_x, hi_y };
        }

        _buildBoardMesh() {
            const box = this._boardBBox();
            const w = box.hi_x - box.lo_x, h = box.hi_y - box.lo_y;
            const thickness = 1.6;

            // Board substrate (FR4-ish).
            const boardGeom = new THREE.BoxGeometry(w, thickness, h);
            const boardMat  = new THREE.MeshStandardMaterial({ color: 0x2e5a2e, roughness: 0.7 });
            const boardMesh = new THREE.Mesh(boardGeom, boardMat);
            boardMesh.position.set((box.lo_x + box.hi_x) / 2, 0, (box.lo_y + box.hi_y) / 2);
            this.scene.add(boardMesh);

            // Copper layers: thin planes just above/below the substrate.
            const copperTop = new THREE.MeshStandardMaterial({ color: 0xc88a17, metalness: 0.6, roughness: 0.3 });
            const copperBot = new THREE.MeshStandardMaterial({ color: 0xc88a17, metalness: 0.6, roughness: 0.3 });

            // Footprints -> plate on their layer, one thin box per pad.
            for (const it of this.board.items || []) {
                if (it.kind !== 'footprint') continue;
                const cx = it.at.x / 1e6, cy = it.at.y / 1e6;
                const yBase = it.placement_layer === 'B.Cu' ? -thickness / 2 : thickness / 2;
                for (const pad of (it.pads || [])) {
                    const px = pad.at.x / 1e6, py = pad.at.y / 1e6;
                    const sx = pad.size.x / 1e6, sy = pad.size.y / 1e6;
                    const g = new THREE.BoxGeometry(sx, 0.05, sy);
                    const m = new THREE.Mesh(g, it.placement_layer === 'B.Cu' ? copperBot : copperTop);
                    m.position.set(cx + px, yBase + (it.placement_layer === 'B.Cu' ? -0.03 : 0.03), cy + py);
                    this.scene.add(m);
                }
            }

            // Vias: cylinders through the board.
            const viaMat = new THREE.MeshStandardMaterial({ color: 0xc88a17, metalness: 0.7, roughness: 0.3 });
            for (const it of this.board.items || []) {
                if (it.kind !== 'via') continue;
                const g = new THREE.CylinderGeometry(it.size / 2 / 1e6, it.size / 2 / 1e6, thickness + 0.05, 16);
                const m = new THREE.Mesh(g, viaMat);
                m.position.set(it.at.x / 1e6, 0, it.at.y / 1e6);
                this.scene.add(m);
            }

            // Tracks: thin strips on their layer.
            const trackMat = new THREE.MeshStandardMaterial({ color: 0xc88a17, metalness: 0.6, roughness: 0.4 });
            for (const it of this.board.items || []) {
                if (it.kind !== 'track') continue;
                const dx = it.end.x - it.start.x, dy = it.end.y - it.start.y;
                const L = Math.hypot(dx, dy) / 1e6;
                const g = new THREE.BoxGeometry(L, 0.035, it.width / 1e6);
                const m = new THREE.Mesh(g, trackMat);
                const mx = (it.start.x + it.end.x) / 2 / 1e6;
                const my = (it.start.y + it.end.y) / 2 / 1e6;
                m.position.set(mx, it.layer === 'B.Cu' ? -thickness/2 - 0.02 : thickness/2 + 0.02, my);
                m.rotation.y = Math.atan2(dy, dx);
                this.scene.add(m);
            }
        }

        _bindControls() {
            // Minimal orbit + zoom (no OrbitControls dependency).
            let dragging = false, sx = 0, sy = 0, yaw = 0, pitch = 0.6;
            const el = this.renderer.domElement;
            el.addEventListener('mousedown', ev => { dragging = true; sx = ev.clientX; sy = ev.clientY; });
            window.addEventListener('mouseup',   () => dragging = false);
            window.addEventListener('mousemove', ev => {
                if (!dragging) return;
                yaw   += (ev.clientX - sx) * 0.01; sx = ev.clientX;
                pitch += (ev.clientY - sy) * 0.01; sy = ev.clientY;
                this._orbit(yaw, pitch);
            });
            el.addEventListener('wheel', ev => {
                ev.preventDefault();
                const s = ev.deltaY < 0 ? 0.9 : 1.1;
                this.camera.position.multiplyScalar(s);
                this.camera.lookAt(0, 0, 0);
            }, { passive: false });
            this._orbit(yaw, pitch);
        }
        _orbit(yaw, pitch) {
            const r = this.camera.position.length();
            const y = Math.sin(pitch) * r;
            const xz = Math.cos(pitch) * r;
            this.camera.position.set(Math.sin(yaw) * xz, y, Math.cos(yaw) * xz);
            this.camera.lookAt(0, 0, 0);
        }

        _loop() {
            const tick = () => {
                this.renderer.render(this.scene, this.camera);
                this._raf = requestAnimationFrame(tick);
            };
            tick();
        }

        destroy() {
            cancelAnimationFrame(this._raf);
            this.renderer.dispose();
            this.root.innerHTML = '';
        }
    }

    window.EDA_Pcb3D = { Pcb3D };
})();
