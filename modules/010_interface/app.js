// SPDX-License-Identifier: GPL-3.0-or-later
// ac9 web UI - vanilla JS, no framework.

const state = {
  rootDir: null,
  openFile: null,
  pickerCurrent: null,
  terminals: {},      // id -> {cwd, log[]}
  activeTerm: null,
  nextTermId: 1,
};

// ---- session id (fragment-routed) -------------------------------------
// The URL fragment #s=<uuid> picks which server-side session this tab is
// attached to. Every /api/* request carries X-Tool-Session: <uuid> via
// the monkey-patched window.fetch below, so the server knows which
// session's SQLite + UI state to read/write. A missing fragment is
// handled in bootSession() (picker on cold start with >0 sessions; auto-
// create otherwise).
let currentSessionId = (() => {
  const m = /(?:^|&)s=([0-9a-f-]{32,40})/.exec(location.hash.replace(/^#/, ''));
  return m ? m[1] : null;
})();
function setSessionInUrl(id) {
  currentSessionId = id;
  if (id) {
    if (location.hash.replace(/^#/, '') !== 's=' + id) {
      history.replaceState(null, '', '#s=' + id);
    }
  }
}
// Monkey-patch fetch so every /api/* call tags the session id. Done once,
// before any other code runs, so existing fetch('/api/...') call sites
// don't need to change.
{
  const origFetch = window.fetch.bind(window);
  window.fetch = function(input, init) {
    const url = (typeof input === 'string') ? input :
                (input && input.url)        ? input.url : '';
    if (url.startsWith('/api/') && currentSessionId) {
      init = init || {};
      const h = new Headers(init.headers || {});
      h.set('X-Tool-Session', currentSessionId);
      init.headers = h;
    }
    return origFetch(input, init);
  };
}

// ---- status polling ---------------------------------------------------
async function pollStatus() {
  try {
    const r = await fetch('/api/status');
    const s = await r.json();
    document.title = s.ready ? 'AutoClank 9001 — ready' : `AutoClank 9001 — ${s.headline}`;
    document.getElementById('status').textContent = s.headline;
  } catch {
    document.title = 'AutoClank 9001 — (server unreachable)';
  }
}
setInterval(pollStatus, 1500);
pollStatus();

// ---- menubar ----------------------------------------------------------
document.querySelectorAll('#menubar .menu-item').forEach(item => {
  item.addEventListener('click', e => {
    e.stopPropagation();
    document.querySelectorAll('#menubar .menu-item').forEach(o => {
      if (o !== item) o.classList.remove('open');
    });
    item.classList.toggle('open');
  });
});
document.addEventListener('click', e => {
  if (!e.target.closest('#menubar .menu-item')) {
    document.querySelectorAll('#menubar .menu-item').forEach(o => o.classList.remove('open'));
  }
});
document.querySelectorAll('[data-action]').forEach(el => {
  el.addEventListener('click', e => {
    e.stopPropagation();
    el.closest('.menu-item')?.classList.remove('open');
    handleMenuAction(el.dataset.action);
  });
});

function handleMenuAction(action) {
  switch (action) {
    case 'open-folder':     openFolderPicker(); break;
    case 'new-file':        createInRoot('file');   break;
    case 'new-folder':      createInRoot('folder'); break;
    case 'delete':          deleteSelected();   break;
    case 'quit':            fetch('/api/quit', {method:'POST'}); break;
    case 'clear-context':   clearContext(); break;
    case 'api-credentials': openCredsModal(); break;
    case 'switch-session':  openSessionPicker({ allowClose: true }); break;
    case 'rename-session':  promptRenameSession(); break;
    case 'new-session':     newSessionThenReload(); break;
    case 'forget-session':  forgetCurrentSession(); break;
    case 'open-tickets':    openTicketsBoard(); break;
  }
}

async function openTicketsBoard() {
  if (!state.rootDir) { alert('Open a project first.'); return; }
  try {
    const r = await fetch('/api/tickets/init', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ cwd: state.rootDir }),
    });
    if (!r.ok) {
      let msg = 'HTTP ' + r.status;
      try { msg = (await r.json()).error || msg; } catch {}
      alert(msg);
      return;
    }
  } catch (err) {
    alert('tickets init failed: ' + err.message);
    return;
  }
  await openFile(state.rootDir + '/.tickets.agile');
}

async function newSessionThenReload() {
  const m = await createSessionOnServer({});
  if (!m) return;
  setSessionInUrl(m.id);
  location.reload();
}

async function createSessionOnServer({ name = '', root_dir = '' }) {
  try {
    const r = await fetch('/api/sessions', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ name, root_dir }),
    });
    if (!r.ok) return null;
    return await r.json();
  } catch { return null; }
}

async function promptRenameSession() {
  if (!currentSessionId) return;
  const name = prompt('Session name:', '');
  if (name === null) return;
  try {
    await fetch('/api/sessions/' + currentSessionId, {
      method: 'PATCH',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ name }),
    });
  } catch {}
}

async function forgetCurrentSession() {
  if (!currentSessionId) return;
  if (!confirm('Forget this session? Its chat history and tab state are deleted.'))
    return;
  try {
    await fetch('/api/sessions/' + currentSessionId, { method: 'DELETE' });
  } catch {}
  // After forgetting, drop fragment and reload — bootSession() takes over.
  currentSessionId = null;
  history.replaceState(null, '', location.pathname);
  location.reload();
}

// ---- API credentials modal -------------------------------------------
const credsModal  = document.getElementById('creds-modal');
const credMouser  = document.getElementById('cred-mouser');
document.getElementById('creds-close') .addEventListener('click', () => credsModal.classList.add('hidden'));
document.getElementById('creds-cancel').addEventListener('click', () => credsModal.classList.add('hidden'));
document.getElementById('creds-save')  .addEventListener('click', saveCreds);
credsModal.addEventListener('click', e => { if (e.target === credsModal) credsModal.classList.add('hidden'); });
document.querySelectorAll('.creds-toggle').forEach(btn => {
  btn.addEventListener('click', () => {
    const target = document.getElementById(btn.dataset.target);
    target.type = (target.type === 'password') ? 'text' : 'password';
  });
});

async function openCredsModal() {
  try {
    const r = await fetch('/api/settings');
    if (r.ok) {
      const s = await r.json();
      credMouser.value = (s.mouser && s.mouser.api_key) || '';
    }
  } catch {}
  credsModal.classList.remove('hidden');
}

async function saveCreds() {
  const body = {
    mouser: { api_key: credMouser.value.trim() },
  };
  try {
    const r = await fetch('/api/settings', {
      method: 'POST', headers: {'Content-Type': 'application/json'},
      body: JSON.stringify(body),
    });
    if (!r.ok) {
      alert('save failed: ' + r.status);
      return;
    }
    credsModal.classList.add('hidden');
  } catch (err) {
    alert('save failed: ' + err.message);
  }
}

// Also wire the AI pane's brain-icon dropdown the same way as the top menu.
document.querySelectorAll('.ai-menu').forEach(item => {
  item.addEventListener('click', e => {
    e.stopPropagation();
    document.querySelectorAll('.menu-item').forEach(o => {
      if (o !== item) o.classList.remove('open');
    });
    item.classList.toggle('open');
  });
});

async function clearContext() {
  try {
    const r = await fetch('/api/context/clear', { method: 'POST' });
    if (r.ok) {
      const j = await r.json().catch(() => ({}));
      // The server hands us back a fresh session id; ride the wave so
      // subsequent /api/chat calls write to the new SQLite.
      if (j.id) setSessionInUrl(j.id);
    }
  } catch {}
  chatLog.innerHTML = '';
}

// ---- session picker modal --------------------------------------------
const sessionModal = document.getElementById('session-modal');
const sessionList  = document.getElementById('session-list');
document.getElementById('session-close') ?.addEventListener('click', closeSessionPicker);
document.getElementById('session-new')   ?.addEventListener('click', async () => {
  const m = await createSessionOnServer({});
  if (!m) return;
  setSessionInUrl(m.id);
  location.reload();
});
sessionModal?.addEventListener('click', e => {
  if (e.target === sessionModal && sessionModal.dataset.allowClose === '1') {
    closeSessionPicker();
  }
});

function closeSessionPicker() {
  sessionModal.classList.add('hidden');
}

async function openSessionPicker({ allowClose = true, sessions = null } = {}) {
  sessionModal.dataset.allowClose = allowClose ? '1' : '0';
  document.getElementById('session-close').style.display =
    allowClose ? '' : 'none';

  if (!sessions) {
    try {
      const r = await fetch('/api/sessions');
      if (r.ok) sessions = (await r.json()).sessions || [];
      else sessions = [];
    } catch { sessions = []; }
  }

  sessionList.innerHTML = '';
  if (!sessions.length) {
    const empty = document.createElement('div');
    empty.className = 'session-empty hint';
    empty.textContent = 'No sessions yet. Click "+ New session" to start one.';
    sessionList.appendChild(empty);
  }
  for (const s of sessions) {
    const row = document.createElement('div');
    row.className = 'session-row' + (s.active ? ' active' : '');
    row.innerHTML =
      `<div class="meta">` +
        `<div class="name"></div>` +
        `<div class="folder"></div>` +
      `</div>` +
      `<div class="stats"></div>` +
      `<div class="last"></div>` +
      `<button class="forget-btn" title="Forget this session">×</button>`;
    row.querySelector('.name').textContent  =
      s.name || abbrPath(s.root_dir) || 'untitled';
    row.querySelector('.folder').textContent = abbrPath(s.root_dir) || '(no folder)';
    row.querySelector('.stats').textContent  =
      `${s.message_count || 0} msgs`;
    row.querySelector('.last').textContent   = humanAgo(s.last_active);
    row.title = s.id;
    row.addEventListener('click', e => {
      if (e.target.classList.contains('forget-btn')) return;
      setSessionInUrl(s.id);
      location.reload();
    });
    row.querySelector('.forget-btn').addEventListener('click', async e => {
      e.stopPropagation();
      if (!confirm(`Forget "${row.querySelector('.name').textContent}"? ` +
                   `Chat history and tab state are deleted.`)) return;
      try { await fetch('/api/sessions/' + s.id, { method: 'DELETE' }); } catch {}
      row.remove();
      // If we just nuked the active session and the picker was modal, refresh.
      if (s.active) {
        currentSessionId = null;
        history.replaceState(null, '', location.pathname);
        location.reload();
      }
    });
    sessionList.appendChild(row);
  }
  sessionModal.classList.remove('hidden');
}

function humanAgo(ts) {
  if (!ts) return '';
  const d = Date.now() - ts;
  if (d < 60_000)        return 'just now';
  if (d < 3_600_000)     return Math.round(d / 60_000) + ' min ago';
  if (d < 86_400_000)    return Math.round(d / 3_600_000) + ' hr ago';
  if (d < 86_400_000 * 7) return Math.round(d / 86_400_000) + ' day' +
                                 (Math.round(d / 86_400_000) === 1 ? '' : 's') + ' ago';
  return new Date(ts).toLocaleDateString();
}
function abbrPath(p) {
  if (!p) return '';
  const home = ['/home/', '/Users/'].find(prefix => p.startsWith(prefix));
  if (home) {
    const rest = p.slice(home.length).split('/');
    if (rest.length > 1) return '~/' + rest.slice(1).join('/');
  }
  return p;
}

// ---- pane hide/show + drag resize -------------------------------------
const paneToggles = document.querySelectorAll('.pane-toggle');
paneToggles.forEach(btn => {
  btn.classList.add('active');
  btn.addEventListener('click', () => {
    const targetId = btn.dataset.toggle;
    const target = document.getElementById(targetId);
    if (!target) return;
    const collapsed = target.classList.toggle('collapsed');
    btn.classList.toggle('active', !collapsed);
    if (targetId === 'pane-files') {
      document.querySelector('.resizer[data-target="pane-files"]')?.classList.toggle('collapsed', collapsed);
    } else if (targetId === 'pane-chat') {
      document.querySelector('.resizer[data-target="pane-chat"]')?.classList.toggle('collapsed', collapsed);
    } else if (targetId === 'terminal-bar') {
      document.getElementById('resizer-terminal')?.classList.toggle('collapsed', collapsed);
      document.body.style.gridTemplateRows = collapsed
        ? '28px 1fr 0 0'
        : `28px 1fr 4px var(--term-h)`;
    }
    updateLayoutColumns();
  });
});

function updateLayoutColumns() {
  const filesHidden = document.getElementById('pane-files').classList.contains('collapsed');
  const chatHidden  = document.getElementById('pane-chat').classList.contains('collapsed');
  const layout = document.getElementById('layout');
  // Build a five-track grid (files, resizer, editor, resizer, chat).
  // Collapsed panes/resizers contribute 0 width.
  const fw = filesHidden ? '0' : 'var(--files-w)';
  const fr = filesHidden ? '0' : '4px';
  const cw = chatHidden  ? '0' : 'var(--chat-w)';
  const cr = chatHidden  ? '0' : '4px';
  layout.style.gridTemplateColumns = `${fw} ${fr} 1fr ${cr} ${cw}`;
}
updateLayoutColumns();

document.querySelectorAll('.resizer').forEach(rz => {
  rz.addEventListener('mousedown', startColResize);
});
document.getElementById('resizer-terminal').addEventListener('mousedown', startRowResize);

function startColResize(e) {
  const target = e.currentTarget.dataset.target;     // 'pane-files' or 'pane-chat'
  const isLeft = target === 'pane-files';
  e.preventDefault();
  const startX = e.clientX;
  const root = document.documentElement;
  const startW = parseInt(getComputedStyle(root).getPropertyValue(isLeft ? '--files-w' : '--chat-w'));
  function move(ev) {
    const dx = ev.clientX - startX;
    const nw = Math.max(120, Math.min(800, startW + (isLeft ? dx : -dx)));
    root.style.setProperty(isLeft ? '--files-w' : '--chat-w', nw + 'px');
    updateLayoutColumns();
  }
  function up() {
    document.removeEventListener('mousemove', move);
    document.removeEventListener('mouseup', up);
  }
  document.addEventListener('mousemove', move);
  document.addEventListener('mouseup', up);
}
function startRowResize(e) {
  e.preventDefault();
  const startY = e.clientY;
  const root = document.documentElement;
  const startH = parseInt(getComputedStyle(root).getPropertyValue('--term-h'));
  function move(ev) {
    const dy = startY - ev.clientY;
    const nh = Math.max(80, Math.min(window.innerHeight - 200, startH + dy));
    root.style.setProperty('--term-h', nh + 'px');
    document.body.style.gridTemplateRows = `28px 1fr 4px ${nh}px`;
  }
  function up() {
    document.removeEventListener('mousemove', move);
    document.removeEventListener('mouseup', up);
  }
  document.addEventListener('mousemove', move);
  document.addEventListener('mouseup', up);
}

// ---- folder picker modal ----------------------------------------------
const picker      = document.getElementById('folder-picker');
const pickerList  = document.getElementById('picker-list');
const pickerPath  = document.getElementById('picker-path');

function openFolderPicker() {
  picker.classList.remove('hidden');
  loadPickerPath(state.rootDir || '~');
}
function closeFolderPicker() { picker.classList.add('hidden'); }
document.getElementById('picker-close').addEventListener('click', closeFolderPicker);
document.getElementById('picker-cancel').addEventListener('click', closeFolderPicker);
document.getElementById('picker-up').addEventListener('click', async () => {
  const data = await fsList(state.pickerCurrent);
  if (data) loadPickerPath(data.parent);
});
document.getElementById('picker-go').addEventListener('click', () => loadPickerPath(pickerPath.value.trim()));
pickerPath.addEventListener('keydown', e => {
  if (e.key === 'Enter') { e.preventDefault(); loadPickerPath(pickerPath.value.trim()); }
});
document.getElementById('picker-open').addEventListener('click', () => {
  if (state.pickerCurrent) commitOpenFolder(state.pickerCurrent);
});
picker.addEventListener('click', e => { if (e.target === picker) closeFolderPicker(); });

async function fsList(path) {
  try {
    const r = await fetch('/api/fs/list?path=' + encodeURIComponent(path));
    if (!r.ok) return null;
    return await r.json();
  } catch { return null; }
}

async function loadPickerPath(path) {
  const data = await fsList(path);
  if (!data) {
    pickerList.innerHTML = `<li class="is-file">cannot read ${escapeHTML(path)}</li>`;
    return;
  }
  state.pickerCurrent = data.path;
  pickerPath.value = data.path;
  pickerList.innerHTML = '';
  for (const e of data.entries) {
    const li = document.createElement('li');
    li.textContent = e.name;
    if (!e.is_dir) li.classList.add('is-file');
    else li.addEventListener('click', () => loadPickerPath(joinPath(data.path, e.name)));
    pickerList.appendChild(li);
  }
}
function joinPath(base, name) {
  return base.endsWith('/') ? base + name : base + '/' + name;
}
async function commitOpenFolder(path) {
  // Dirty-aware close of all open files before switching folders.
  const dirtyPaths = Object.entries(state.files)
    .filter(([_, f]) => f.dirty)
    .map(([p]) => p);
  if (dirtyPaths.length) {
    const list = dirtyPaths.map(p => '  • ' + p.split('/').pop()).join('\n');
    const choice = window.prompt(
      `Unsaved changes in ${dirtyPaths.length} file(s):\n${list}\n\n` +
      `Type:\n  s  -> save all & switch folder\n  d  -> discard all & switch\n  c  -> cancel folder change`,
      's'
    );
    if (choice === null || choice.toLowerCase().startsWith('c')) {
      closeFolderPicker();
      return;  // cancel
    }
    if (choice.toLowerCase().startsWith('s')) {
      for (const p of dirtyPaths) {
        const f = state.files[p];
        if (!f) continue;
        await saveFile(p, f.getContent());
      }
    }
    // 'd' (discard) falls through, files are closed without saving below
  }

  // Close every open file (destroy editors, remove tabs).
  for (const p of Object.keys(state.files)) {
    const f = state.files[p];
    if (f) {
      try { f.destroy(); } catch {}
      f.surface.remove();
      f.tab.remove();
    }
    delete state.files[p];
  }
  state.activeFilePath = null;
  // Re-show the placeholder.
  if (!editorBody.querySelector('.editor-empty')) {
    const empty = document.createElement('div');
    empty.className = 'editor-empty hint';
    empty.textContent = 'No file open.';
    editorBody.appendChild(empty);
  }

  state.rootDir = path;
  document.getElementById('files-root').textContent = path;
  refreshWebLookupBtn();
  // Close every existing terminal and open a fresh one rooted at the new folder.
  for (const id of Object.keys(state.terminals)) closeTerminal(id);
  newTerminal();
  closeFolderPicker();
  refreshFileTree();
}

// ---- file tree (left pane) --------------------------------------------
const filesTree = document.getElementById('files-tree');
async function refreshFileTree() {
  if (!state.rootDir) {
    filesTree.innerHTML = '<em class="hint">Open a folder to begin (File → Open Folder)</em>';
    return;
  }
  const data = await fsList(state.rootDir);
  if (!data) { filesTree.innerHTML = '<em class="hint">cannot read folder</em>'; return; }
  filesTree.innerHTML = '';
  renderEntries(filesTree, data.path, data.entries);
}
function renderEntries(into, parentPath, entries) {
  for (const e of entries) {
    const node = document.createElement('div');
    node.className = 'fs-node ' + (e.is_dir ? 'fs-dir' : 'fs-file');
    node.dataset.path = joinPath(parentPath, e.name);
    node.innerHTML = `<div class="fs-name">${escapeHTML(e.name)}</div>`;
    if (e.is_dir) {
      const kids = document.createElement('div');
      kids.className = 'fs-children hidden';
      node.appendChild(kids);
      node.querySelector('.fs-name').addEventListener('click', async ev => {
        ev.stopPropagation();
        if (handleFsSelectClick(ev, node)) return;
        setFsSingleSelection(node.dataset.path);
        kids.classList.toggle('hidden');
        if (!kids.classList.contains('hidden') && kids.children.length === 0) {
          const sub = await fsList(node.dataset.path);
          if (sub) renderEntries(kids, sub.path, sub.entries);
        }
      });
      // Folders are also draggable so a user can grab a whole subtree
      // and drop it into a sibling / parent folder to move it.
      wireFsDragSource(node);
      // ... and they're drop targets: another draggable node landed on
      // this folder means "move source into this folder".
      wireFsDropTarget(node);
    } else {
      node.querySelector('.fs-name').addEventListener('click', ev => {
        if (handleFsSelectClick(ev, node)) return;
        setFsSingleSelection(node.dataset.path);
        openFile(node.dataset.path);
      });
      // Make file rows draggable so the user can drop an image onto the AI
      // pane to trigger a vision analysis, or drag them onto a folder in
      // the tree to MOVE them (the drop target uses effectAllowed=copyMove
      // to differentiate the two intents by cursor style).
      wireFsDragSource(node);
    }
    into.appendChild(node);
  }
  applyFsSelectionClasses();
}

// Attach the common drag-source wiring used by both file and folder
// rows. Emits the path via 'text/x-tool-path' (the same MIME the AI
// pane's vision drop-target listens on) plus a plain-text fallback for
// external drop targets. effectAllowed=copyMove so the browser can
// show "move" cursor over the file tree and "copy" over the AI pane.
function wireFsDragSource(node) {
  node.draggable = true;
  node.addEventListener('dragstart', ev => {
    ev.stopPropagation();
    ev.dataTransfer.setData('text/x-tool-path', node.dataset.path);
    ev.dataTransfer.setData('text/plain',       node.dataset.path);
    ev.dataTransfer.effectAllowed = 'copyMove';
  });
}

// Make a folder node a drop target for file-tree move-into-folder.
// Only accepts drops carrying our internal 'text/x-tool-path' payload,
// so drops from the OS (e.g. dragging an image out of the file manager)
// don't accidentally trigger a move.
function wireFsDropTarget(folderNode) {
  const label = folderNode.querySelector('.fs-name');
  const srcPathOf = ev => {
    // dragover doesn't expose the payload for security reasons, so we
    // gate on the MIME being present and defer path resolution to drop.
    return ev.dataTransfer.getData('text/x-tool-path');
  };
  const isSameOrSelfMove = (source, dest) => {
    if (!source || !dest) return true;
    if (source === dest) return true;                     // dropped onto self
    if (dest.startsWith(source + '/')) return true;       // folder into its descendant
    // dropping A onto its own parent is a no-op; catch it before the
    // server 409s us with "target already exists".
    const parent = source.substring(0, source.lastIndexOf('/'));
    if (parent === dest) return true;
    return false;
  };
  folderNode.addEventListener('dragover', ev => {
    if (!ev.dataTransfer.types.includes('text/x-tool-path')) return;
    ev.preventDefault();                                  // required to enable drop
    ev.stopPropagation();                                 // outer folders don't steal the drop
    ev.dataTransfer.dropEffect = 'move';
    folderNode.classList.add('fs-drop-hover');
  });
  folderNode.addEventListener('dragleave', ev => {
    // Only clear the highlight if the pointer actually left this
    // folder's own subtree — dragleave fires whenever the pointer
    // crosses into a child element too.
    if (!folderNode.contains(ev.relatedTarget)) {
      folderNode.classList.remove('fs-drop-hover');
    }
  });
  folderNode.addEventListener('drop', async ev => {
    folderNode.classList.remove('fs-drop-hover');
    const source = srcPathOf(ev);
    if (!source) return;                                  // not our payload
    ev.preventDefault();
    ev.stopPropagation();
    const dest = folderNode.dataset.path;
    if (isSameOrSelfMove(source, dest)) return;           // silent no-op
    const basename = source.split('/').pop();
    const to = dest.endsWith('/') ? dest + basename : dest + '/' + basename;
    try {
      const r = await fetch('/api/fs/rename', {
        method: 'POST', headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({from: source, to}),
      });
      const j = await r.json().catch(() => ({}));
      if (!r.ok) {
        alert('Move failed: ' + (j.error || r.status));
        return;
      }
    } catch (err) {
      alert('Move failed: ' + err.message);
      return;
    }
    // Clear selection so the vanished path doesn't linger as "selected"
    // in the tree state, then re-render from disk.
    fsSel.paths.delete(source);
    if (fsSel.anchor === source) fsSel.anchor = null;
    refreshFileTree();
  });
  // Also stop drop bubbling on the label itself so drops on nested
  // labels get consumed by the nearest folder, not an ancestor.
  label.addEventListener('drop', ev => ev.stopPropagation());
}

// --- File-tree multi-select ---
// Plain click: select one (and open/expand as before). Ctrl/Cmd+click:
// toggle items in and out individually. Shift+click: select the visual
// range from the last non-shift click. Click on empty tree space clears.
const fsSel = { paths: new Set(), anchor: null };

function visibleFsNodes() {
  return [...filesTree.querySelectorAll('.fs-node')]
    .filter(n => !n.closest('.fs-children.hidden'));
}

function applyFsSelectionClasses() {
  for (const n of filesTree.querySelectorAll('.fs-node')) {
    n.classList.toggle('selected', fsSel.paths.has(n.dataset.path));
  }
}

function setFsSingleSelection(path) {
  fsSel.paths = new Set([path]);
  fsSel.anchor = path;
  applyFsSelectionClasses();
}

function handleFsSelectClick(ev, node) {
  const path = node.dataset.path;
  if (ev.ctrlKey || ev.metaKey) {
    if (fsSel.paths.has(path)) fsSel.paths.delete(path);
    else                       fsSel.paths.add(path);
    fsSel.anchor = path;
    applyFsSelectionClasses();
    return true;
  }
  if (ev.shiftKey) {
    const paths = visibleFsNodes().map(n => n.dataset.path);
    const a = paths.indexOf(fsSel.anchor);
    const b = paths.indexOf(path);
    if (a === -1 || b === -1) { setFsSingleSelection(path); return true; }
    fsSel.paths = new Set(paths.slice(Math.min(a, b), Math.max(a, b) + 1));
    applyFsSelectionClasses();
    return true;
  }
  return false;
}

filesTree.addEventListener('click', e => {
  if (!e.target.closest('.fs-node')) {
    fsSel.paths.clear();
    fsSel.anchor = null;
    applyFsSelectionClasses();
  }
});

// Delete / Backspace with items selected in the file tree = delete them.
// Skip when a text input has focus (Backspace there is normal editing).
// Also swallow the default Backspace-goes-back behaviour once we act on it.
document.addEventListener('keydown', e => {
  if (e.key !== 'Delete' && e.key !== 'Backspace') return;
  const ae = document.activeElement;
  if (ae) {
    const tag = ae.tagName;
    if (tag === 'INPUT' || tag === 'TEXTAREA' || tag === 'SELECT') return;
    if (ae.isContentEditable) return;
  }
  if (fsSel.paths.size === 0) return;
  e.preventDefault();
  const paths = [...fsSel.paths];
  if (paths.length === 1) {
    const node = filesTree.querySelector(
      `.fs-node[data-path="${CSS.escape(paths[0])}"]`);
    const isDir = node ? node.classList.contains('fs-dir') : false;
    deleteFsEntry(paths[0], isDir);
  } else {
    deleteFsEntries(paths);
  }
});

// --- File-tree context menu (right-click: rename / delete) ---
const fsMenu = document.createElement('div');
fsMenu.className = 'ctx-menu hidden';
document.body.appendChild(fsMenu);

function hideFsMenu() { fsMenu.classList.add('hidden'); }
document.addEventListener('click', hideFsMenu);
document.addEventListener('keydown', e => { if (e.key === 'Escape') hideFsMenu(); });
window.addEventListener('blur', hideFsMenu);

function showFsMenu(x, y, path, isDir) {
  fsMenu.innerHTML = '';
  const item = (label, fn) => {
    const el = document.createElement('div');
    el.className = 'ctx-item';
    el.textContent = label;
    el.addEventListener('click', ev => { ev.stopPropagation(); hideFsMenu(); fn(); });
    fsMenu.appendChild(el);
  };
  const sel = [...fsSel.paths];
  if (sel.length > 1) {
    item(`Delete ${sel.length} items`, () => deleteFsEntries(sel));
  } else {
    item('Rename…', () => renameFsEntry(path));
    item('Delete',  () => deleteFsEntry(path, isDir));
  }
  fsMenu.classList.remove('hidden');
  // Clamp to the viewport so the menu never opens half off-screen.
  const mw = fsMenu.offsetWidth, mh = fsMenu.offsetHeight;
  fsMenu.style.left = Math.min(x, window.innerWidth  - mw - 4) + 'px';
  fsMenu.style.top  = Math.min(y, window.innerHeight - mh - 4) + 'px';
}

async function renameFsEntry(path) {
  const oldName = path.split('/').pop();
  const newName = window.prompt('Rename to:', oldName);
  if (!newName || newName === oldName) return;
  if (newName.includes('/')) { alert('Name cannot contain "/"'); return; }
  try {
    const r = await fetch('/api/fs/rename', {
      method: 'POST', headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({from: path, to: newName}),
    });
    const j = await r.json();
    if (!r.ok) { alert('Rename failed: ' + (j.error || r.status)); return; }
    // An open editor tab still points at the old path; close it so the
    // user re-opens the file under its new name instead of saving to the
    // old (now recreated) path.
    if (state.files && state.files[path]) closeFile(path);
  } catch (err) {
    alert('Rename failed: ' + err.message);
  }
  refreshFileTreeIfOpen();
}

async function deleteFsEntry(path, isDir) {
  const name = path.split('/').pop();
  if (!window.confirm(`Delete ${isDir ? 'folder' : 'file'} "${name}"${isDir ? ' and everything in it' : ''}?`)) return;
  try {
    const r = await fetch('/api/fs/delete?path=' + encodeURIComponent(path), {method: 'DELETE'});
    const j = await r.json();
    if (!r.ok) { alert('Delete failed: ' + (j.error || r.status)); return; }
    if (state.files && state.files[path]) closeFile(path);
  } catch (err) {
    alert('Delete failed: ' + err.message);
  }
  refreshFileTreeIfOpen();
}

// Delete a batch of paths. A selected folder may contain other selected
// entries; the server treats deleting an already-gone path as success, so
// the order doesn't matter.
async function deleteFsEntries(paths) {
  const names = paths.map(p => p.split('/').pop());
  const preview = names.slice(0, 5).join(', ') + (names.length > 5 ? `, … (${names.length - 5} more)` : '');
  if (!window.confirm(`Delete ${paths.length} items?\n${preview}\nFolders are deleted with everything in them.`)) return;
  const failed = [];
  for (const p of paths) {
    try {
      const r = await fetch('/api/fs/delete?path=' + encodeURIComponent(p), {method: 'DELETE'});
      if (!r.ok) { failed.push(p); continue; }
      for (const open of Object.keys(state.files || {})) {
        if (open === p || open.startsWith(p + '/')) closeFile(open);
      }
    } catch (err) {
      failed.push(p);
    }
  }
  fsSel.paths.clear();
  fsSel.anchor = null;
  if (failed.length) alert('Failed to delete:\n' + failed.join('\n'));
  refreshFileTreeIfOpen();
}

filesTree.addEventListener('contextmenu', e => {
  const node = e.target.closest('.fs-node');
  if (!node || !node.dataset.path) return;
  e.preventDefault();
  e.stopPropagation();
  // Right-click on something outside the current selection retargets the
  // selection to just that item, like every desktop file manager.
  if (!fsSel.paths.has(node.dataset.path)) setFsSingleSelection(node.dataset.path);
  showFsMenu(e.clientX, e.clientY, node.dataset.path, node.classList.contains('fs-dir'));
});

const editorBody  = document.getElementById('editor-body');
const editorTabs  = document.getElementById('editor-tabs');

// Per-open-file state:
//   state.files[path] = {
//     mode:          'markdown'|'prose'|'code'|'unknown',
//     savedContent:  string (last saved content),
//     surface:       <div.editor-surface> DOM element
//     tab:           <div.editor-tab> DOM element
//     getContent():  () -> current content string
//     destroy():     () -> cleanup (e.g. mdEditor.destroy())
//   }
state.files = state.files || {};
state.activeFilePath = state.activeFilePath || null;

const EDITOR_MODES = {
  markdown: new Set(['md','markdown']),
  prose:    new Set(['txt','rst','text']),
  code:     new Set(['c','cc','cpp','cxx','h','hpp','hxx',
                     'js','mjs','ts','tsx','jsx','html','htm','css','json',
                     'py','sh','bash','zsh','rb','go','rs','java','kt',
                     'lua','pl','php','sql','yml','yaml','toml','ini','cfg',
                     'xml','svg','log','cmake','make','makefile','dockerfile']),
  pdf:      new Set(['pdf']),
  image:    new Set(['png','jpg','jpeg','gif','webp','ico','bmp']),
};

// Map a file path to a Prism grammar id for the editor pane; 'none'
// keeps the pane editable but unhighlighted.
function prismLangForPath(path) {
  const base = path.split('/').pop().toLowerCase();
  if (base === 'cmakelists.txt') return 'cmake';
  if (base === 'makefile' || base === 'gnumakefile') return 'makefile';
  if (base === 'dockerfile') return 'docker';
  const ext = (base.split('.').pop() || '').toLowerCase();
  const map = {
    c: 'c', h: 'cpp', cpp: 'cpp', cc: 'cpp', cxx: 'cpp', hpp: 'cpp',
    hh: 'cpp', hxx: 'cpp', cu: 'cpp', ino: 'cpp',
    py: 'python', js: 'javascript', mjs: 'javascript', jsx: 'jsx',
    ts: 'typescript', tsx: 'tsx', json: 'json',
    html: 'markup', htm: 'markup', xml: 'markup', svg: 'markup',
    css: 'css', scss: 'scss', less: 'less',
    md: 'markdown', sh: 'bash', bash: 'bash', zsh: 'bash',
    cmake: 'cmake', rs: 'rust', go: 'go', java: 'java', rb: 'ruby',
    php: 'php', sql: 'sql', yaml: 'yaml', yml: 'yaml', toml: 'toml',
    ini: 'ini', cfg: 'ini', conf: 'ini', lua: 'lua', pl: 'perl',
    swift: 'swift', kt: 'kotlin', cs: 'csharp', vue: 'markup',
    tex: 'latex', r: 'r', dart: 'dart', zig: 'zig', asm: 'nasm',
    s: 'armasm', proto: 'protobuf', graphql: 'graphql', diff: 'diff',
    patch: 'diff', bat: 'batch', ps1: 'powershell',
  };
  const lang = map[ext];
  return (lang && typeof Prism !== 'undefined' && Prism.languages[lang])
    ? lang : 'none';
}

function detectMode(path) {
  const fname = path.split('/').pop().toLowerCase();
  if (fname.endsWith('.tickets.agile')) return 'tickets';
  if (fname === 'makefile' || fname === 'dockerfile' || fname === 'cmakelists.txt') return 'code';
  const ext = fname.includes('.') ? fname.split('.').pop() : '';
  if (EDITOR_MODES.markdown.has(ext)) return 'markdown';
  if (EDITOR_MODES.prose.has(ext))    return 'prose';
  if (EDITOR_MODES.code.has(ext))     return 'code';
  if (EDITOR_MODES.pdf.has(ext))      return 'pdf';
  if (EDITOR_MODES.image.has(ext))    return 'image';
  return 'unknown';
}

async function openFile(path) {
  // Already open? Just switch to that tab.
  if (state.files[path]) {
    activateFile(path);
    return;
  }

  // Hide the "no file open" placeholder once we have content.
  const empty = editorBody.querySelector('.editor-empty');
  if (empty) empty.remove();

  // Tickets board: bypasses /api/fs/read entirely (schema lives behind
  // /api/tickets, not raw bytes). Detected by filename ".tickets.agile".
  if (detectMode(path) === 'tickets') {
    await openTicketsFile(path);
    return;
  }

  const r = await fetch('/api/fs/read?path=' + encodeURIComponent(path));
  if (!r.ok) {
    const w = document.createElement('div');
    w.className = 'editor-readonly';
    w.textContent = 'cannot read ' + path;
    editorBody.appendChild(w);
    return;
  }
  const j = await r.json();
  const mode = detectMode(path);

  // Build surface (the per-file editor container, absolutely positioned).
  const surface = document.createElement('div');
  surface.className = 'editor-surface';
  surface.dataset.path = path;
  editorBody.appendChild(surface);

  // CRITICAL: make this surface visible BEFORE constructing the editor.
  // Toast UI Editor (and to a lesser extent <textarea>) measures container
  // dimensions on instantiation; a `display:none` container yields zero
  // dimensions and the editor renders blank. Hide existing surfaces first,
  // show this new one, THEN construct.
  for (const ff of Object.values(state.files)) ff.surface.classList.remove('active');
  surface.classList.add('active');

  // ---- Binary modes ----
  // PDF: read-only iframe embed.
  if (mode === 'pdf') {
    state.files[path] = {
      mode, savedContent: '',
      surface, tab: null,
      getContent: () => '',
      destroy: () => {},
      dirty: false,
    };
    const iframe = document.createElement('iframe');
    iframe.className = 'pdf-embed';
    iframe.src = '/api/fs/raw?path=' + encodeURIComponent(path);
    surface.appendChild(iframe);
    buildEditorTab(path, mode);
    activateFile(path);
    saveState();
    return;
  }

  // Image: view (img tag) OR edit (canvas paint UI). Toggle via tab button.
  if (mode === 'image') {
    state.files[path] = {
      mode, savedContent: '',
      surface, tab: null,
      getContent: () => '',
      destroy: () => {},
      dirty: false,
      imgMode: 'view',
      paint: null,     // populated when entering edit mode
    };

    const viewWrap = document.createElement('div');
    viewWrap.className = 'image-view-wrap active';
    const img = document.createElement('img');
    img.className = 'image-embed';
    img.src = '/api/fs/raw?path=' + encodeURIComponent(path) +
              '&_t=' + Date.now();    // cache-bust so re-opens see edits
    img.draggable = true;
    img.addEventListener('dragstart', ev => {
      ev.dataTransfer.setData('text/x-tool-path', path);
      ev.dataTransfer.setData('text/plain',       path);
      ev.dataTransfer.effectAllowed = 'copy';
    });
    viewWrap.appendChild(img);
    surface.appendChild(viewWrap);

    // Edit surface (canvas + paint toolbar) is lazy — only built when the
    // user toggles to edit mode for the first time.
    state.files[path].buildPaint = () => {
      if (state.files[path].paint) return;
      const paintWrap = document.createElement('div');
      paintWrap.className = 'image-edit-wrap';
      surface.appendChild(paintWrap);
      const paint = buildPaintEditor(paintWrap, img, path);
      state.files[path].paint = paint;
    };

    state.files[path].setImgMode = (m) => {
      const f = state.files[path];
      if (!f) return;
      if (m === 'edit') f.buildPaint();
      // Toggling back to view: refresh the <img> from the live canvas so
      // unsaved edits are still visible. (When saved, the file is also
      // re-fetched fresh — both paths converge on showing current state.)
      if (m === 'view' && f.paint) {
        const v = f.surface.querySelector('.image-view-wrap img');
        if (v) v.src = f.paint.snapshotDataURL();
      }
      f.imgMode = m;
      const v = f.surface.querySelector('.image-view-wrap');
      const e = f.surface.querySelector('.image-edit-wrap');
      if (v) v.classList.toggle('active', m === 'view');
      if (e) e.classList.toggle('active', m === 'edit');
    };

    buildEditorTab(path, mode);
    activateFile(path);
    saveState();
    return;
  }

  // Register the file in state EARLY (before any editor construction) so
  // change handlers that fire during init don't NPE on state.files[path].
  state.files[path] = {
    mode, savedContent: j.content,
    surface, tab: null,
    getContent: () => j.content,
    setContent: () => {},
    destroy: () => {},
    dirty: false,
    mtimeNs: (typeof j.mtime_ns === 'string') ? j.mtime_ns : '0',
  };

  let getContent;
  let setContent = () => {};
  let destroy = () => {};

  if (mode === 'markdown' && typeof toastui !== 'undefined') {
    const host = document.createElement('div');
    host.className = 'md-editor-host';
    surface.appendChild(host);
    // Force a layout pass so the host has real dimensions BEFORE Toast UI
    // measures it. Otherwise Toast UI sometimes throws or renders blank.
    await new Promise(r => requestAnimationFrame(r));
    void host.offsetHeight;
    try {
      const editor = new toastui.Editor({
        el: host,
        initialValue: j.content,
        // Hide Toast UI's bottom-right mode switch; we put our own toggle
        // ON the tab (see addMdModeToggle below).
        previewStyle: 'vertical',
        initialEditType: 'wysiwyg',
        previewHighlight: true,
        theme: 'dark',
        height: '100%',
        usageStatistics: false,
        hideModeSwitch: true,
        // Code blocks in the preview/WYSIWYG highlight through the same
        // vendored Prism (297 languages) the chat pane uses.
        plugins: (typeof toastuiCodeSyntaxHighlight === 'function' &&
                  typeof Prism !== 'undefined')
          ? [[toastuiCodeSyntaxHighlight, { highlighter: Prism }]]
          : [],
        toolbarItems: [
          ['heading','bold','italic','strike'],
          ['hr','quote'],
          ['ul','ol','task','indent','outdent'],
          ['table','link','image'],
          ['code','codeblock'],
        ],
      });
      editor.on('change', () => {
        const v = editor.getMarkdown();
        const f = state.files[path];
        if (f) setFileDirty(path, v !== f.savedContent);
      });
      getContent = () => editor.getMarkdown();
      setContent = (v) => { try { editor.setMarkdown(v); } catch {} };
      destroy    = () => { try { editor.destroy(); } catch {} };
      // Stash a mode-switch callback so the tab toggle can drive it.
      state.files[path].changeMode = (m) => {
        try { editor.changeMode(m); } catch {}
        state.files[path].mdMode = m;
      };
      state.files[path].mdMode = 'wysiwyg';
    } catch (err) {
      // Toast UI blew up. Fall back to a plain textarea showing the source
      // so the user can at least see + edit the markdown. Surface the
      // error visibly above the textarea so we can diagnose.
      console.error('Toast UI editor failed:', err);
      host.remove();
      const errBar = document.createElement('div');
      errBar.style.cssText = 'background:#7a3030;color:#fff;padding:4px 8px;font-size:11px;';
      errBar.textContent = 'Markdown engine failed (' + (err && err.message ? err.message : 'unknown') + ') — falling back to plain text editor.';
      surface.appendChild(errBar);
      const ta = document.createElement('textarea');
      ta.className = 'editor-textarea prose';
      ta.spellcheck = true;
      ta.value = j.content;
      ta.addEventListener('input', () => {
        const f = state.files[path];
        if (f) setFileDirty(path, ta.value !== f.savedContent);
      });
      ta.addEventListener('keydown', e => {
        if ((e.ctrlKey || e.metaKey) && e.key === 's') {
          e.preventDefault();
          saveFile(path, ta.value);
        }
      });
      surface.appendChild(ta);
      getContent = () => ta.value;
      setContent = (v) => { ta.value = v; };
    }
  } else if (mode === 'code' && typeof CodeJar === 'function' &&
             typeof Prism !== 'undefined' && j.content.length < 300 * 1024) {
    // Highlighted, editable code pane: CodeJar (contenteditable) with the
    // vendored Prism as the tokenizer. CodeJar re-highlights the whole
    // document per keystroke, so very large files (300KB+) fall through
    // to the plain textarea below instead.
    const lang = prismLangForPath(path);
    const wrap = document.createElement('div');
    wrap.className = 'editor-codejar';
    const pre = document.createElement('pre');
    pre.className = 'editor-code language-' + lang;
    wrap.appendChild(pre);
    surface.appendChild(wrap);
    const jar = CodeJar(pre, el => {
      try { Prism.highlightElement(el); } catch { /* leave plain */ }
    }, { tab: '    ' });
    jar.updateCode(j.content);
    jar.onUpdate(code => {
      setFileDirty(path, code !== state.files[path].savedContent);
    });
    pre.addEventListener('keydown', e => {
      if ((e.ctrlKey || e.metaKey) && e.key === 's') {
        e.preventDefault();
        saveFile(path, jar.toString());
      }
    });
    getContent = () => jar.toString();
    setContent = (v) => { try { jar.updateCode(v); } catch {} };
    destroy    = () => { try { jar.destroy(); } catch {} };
  } else if (mode === 'prose' || mode === 'code') {
    const ta = document.createElement('textarea');
    ta.className = 'editor-textarea' + (mode === 'prose' ? ' prose' : '');
    ta.spellcheck = (mode === 'prose');
    ta.value = j.content;
    ta.addEventListener('input', () => {
      setFileDirty(path, ta.value !== state.files[path].savedContent);
    });
    ta.addEventListener('keydown', e => {
      if ((e.ctrlKey || e.metaKey) && e.key === 's') {
        e.preventDefault();
        saveFile(path, ta.value);
      }
    });
    surface.appendChild(ta);
    getContent = () => ta.value;
    setContent = (v) => { ta.value = v; };
  } else {
    const wrap = document.createElement('div');
    wrap.className = 'editor-readonly';
    const pre = document.createElement('pre');
    pre.textContent = j.content;
    wrap.appendChild(pre);
    surface.appendChild(wrap);
    let stashed = j.content;
    getContent = () => stashed;
    setContent = (v) => { stashed = v; pre.textContent = v; };
  }

  const tab = buildEditorTab(path, mode);

  // Finalize the state entry — overwrite the early placeholder we wrote
  // before editor construction with the real getContent / destroy + tab.
  state.files[path].tab        = tab;
  state.files[path].getContent = getContent;
  state.files[path].setContent = setContent;
  state.files[path].destroy    = destroy;

  activateFile(path);
  saveState();
}

function buildEditorTab(path, mode) {
  const tab = document.createElement('div');
  tab.className = 'editor-tab';
  tab.dataset.path = path;
  let extra = '';
  if (mode === 'markdown') {
    extra = `<button class="md-toggle" title="Switch markdown source / rendered">Source</button>`;
    // (label is updated after construction below to reflect actual mode)
  } else if (mode === 'image') {
    extra =
      `<button class="img-toggle" title="Toggle view / edit">Edit</button>` +
      `<button class="img-describe" title="Ask the vision AI to describe this image">🧠</button>`;
  }
  tab.innerHTML =
    `<span class="dirty">●</span>` +
    `<span class="label"></span>` +
    extra +
    `<span class="x" title="Close">×</span>`;
  if (mode === 'browser' && path.startsWith('__browser:')) {
    tab.querySelector('.label').textContent =
      '🌐 ' + shortenUrlForTab(path.slice('__browser:'.length));
    tab.title = path.slice('__browser:'.length);
  } else {
    tab.querySelector('.label').textContent = path.split('/').pop();
    tab.title = path;
  }
  tab.addEventListener('click', e => {
    if (e.target.classList.contains('x'))            return;
    if (e.target.classList.contains('md-toggle'))    return;
    if (e.target.classList.contains('img-toggle'))   return;
    if (e.target.classList.contains('img-describe')) return;
    activateFile(path);
  });
  tab.querySelector('.x').addEventListener('click', e => {
    e.stopPropagation();
    closeFile(path);
  });
  if (mode === 'markdown') {
    const btn = tab.querySelector('.md-toggle');
    // Label always shows the OTHER mode (what you'll switch to on click).
    const labelFor = (m) => m === 'markdown' ? 'Rendered' : 'Source';
    // Sync to current state (default we set is 'wysiwyg' so this reads 'Source').
    const f0 = state.files[path];
    if (f0 && f0.mdMode) btn.textContent = labelFor(f0.mdMode);
    btn.addEventListener('click', e => {
      e.stopPropagation();
      const f = state.files[path];
      if (!f) return;
      if (!f.changeMode) {
        btn.textContent = '(no md engine)';
        return;
      }
      const next = (f.mdMode === 'markdown') ? 'wysiwyg' : 'markdown';
      f.changeMode(next);
      btn.textContent = labelFor(next);
    });
  } else if (mode === 'image') {
    const tbtn = tab.querySelector('.img-toggle');
    tbtn.addEventListener('click', e => {
      e.stopPropagation();
      const f = state.files[path];
      if (!f || !f.setImgMode) return;
      const next = (f.imgMode === 'view') ? 'edit' : 'view';
      f.setImgMode(next);
      tbtn.textContent = (next === 'view') ? 'Edit' : 'View';
    });
    tab.querySelector('.img-describe').addEventListener('click', e => {
      e.stopPropagation();
      analyzeImage(path);
    });
  }
  editorTabs.appendChild(tab);

  // For binary modes the state entry needs the tab linked too.
  if (state.files[path]) state.files[path].tab = tab;

  return tab;
}

// ===== Image paint editor (MS-Paint-style) ==============================
// Builds a canvas + toolbar inside `host`. Returns an object exposing
// .save(), .undo(), .redo(), .copy(), .cut(), .paste(), .pasteBlob(),
// .deleteSelection(), .hasSelection(), .getCanvas(), .zoom(direction).
// Saving is keyboard-only (Ctrl+S); the toolbar has tools / color / size
// / zoom — no save button.
function buildPaintEditor(host, srcImg, path) {
  const toolbar = document.createElement('div');
  toolbar.className = 'paint-toolbar';
  toolbar.innerHTML =
    `<button data-tool="brush"  class="paint-tool active" title="Brush">✎</button>` +
    `<button data-tool="eraser" class="paint-tool"        title="Eraser">⌫</button>` +
    `<button data-tool="line"   class="paint-tool"        title="Line">／</button>` +
    `<button data-tool="rect"   class="paint-tool"        title="Rectangle">▭</button>` +
    `<button data-tool="select" class="paint-tool"        title="Select (drag a rectangle, then Ctrl+C / Ctrl+X / Ctrl+V / Del)">⛶</button>` +
    `<span class="paint-sep"></span>` +
    `<input  type="color" class="paint-color" value="#ff2222" title="Color">` +
    `<input  type="range" class="paint-size"  min="1" max="60" value="4" title="Brush size">` +
    `<span   class="paint-size-label">4px</span>` +
    `<span class="paint-sep"></span>` +
    `<button class="paint-undo"     title="Undo (Ctrl+Z)">↶</button>` +
    `<button class="paint-redo"     title="Redo (Ctrl+Y)">↷</button>` +
    `<span class="paint-sep"></span>` +
    `<button class="paint-zoom-out" title="Zoom out (Ctrl+Wheel)">−</button>` +
    `<span class="paint-zoom-label" title="Zoom (click to reset)">100%</span>` +
    `<button class="paint-zoom-in"  title="Zoom in (Ctrl+Wheel)">+</button>`;
  host.appendChild(toolbar);

  // Canvas stack: base (committed pixels) + overlay (preview / selection).
  // .paint-canvases wraps both and grows / shrinks with zoom.
  const stack = document.createElement('div');
  stack.className = 'paint-stack';
  host.appendChild(stack);
  const inner = document.createElement('div');
  inner.className = 'paint-canvases';
  stack.appendChild(inner);
  const base    = document.createElement('canvas');
  const overlay = document.createElement('canvas');
  base.className    = 'paint-canvas base';
  overlay.className = 'paint-canvas overlay';
  inner.appendChild(base);
  inner.appendChild(overlay);

  const st = {
    tool: 'brush',
    color: '#ff2222',
    size: 4,
    zoom: 1,
    drawing: false,
    startX: 0, startY: 0,
    lastX: 0,  lastY: 0,
    selection: null,         // {x, y, w, h} in canvas pixels
    selecting: false,        // mid-drag for select tool
    clipboard: null,         // HTMLCanvasElement holding copied pixels
  };

  const bctx = base.getContext('2d');
  const octx = overlay.getContext('2d');

  // ----- canvas init + zoom ---------------------------------------------
  const applyZoom = () => {
    const w = base.width  * st.zoom;
    const h = base.height * st.zoom;
    inner.style.width    = w + 'px';
    inner.style.height   = h + 'px';
    base.style.width     = w + 'px';
    base.style.height    = h + 'px';
    overlay.style.width  = w + 'px';
    overlay.style.height = h + 'px';
    const pct = Math.round(st.zoom * 100);
    toolbar.querySelector('.paint-zoom-label').textContent = pct + '%';
  };
  const initCanvases = () => {
    const w = srcImg.naturalWidth  || srcImg.width  || 600;
    const h = srcImg.naturalHeight || srcImg.height || 400;
    base.width    = w; base.height    = h;
    overlay.width = w; overlay.height = h;
    bctx.drawImage(srcImg, 0, 0, w, h);
    applyZoom();
  };
  if (srcImg.complete && srcImg.naturalWidth) initCanvases();
  else srcImg.addEventListener('load', initCanvases, { once: true });

  // ----- toolbar wiring -------------------------------------------------
  toolbar.querySelectorAll('.paint-tool').forEach(btn => {
    btn.addEventListener('click', () => {
      st.tool = btn.dataset.tool;
      toolbar.querySelectorAll('.paint-tool').forEach(b =>
        b.classList.toggle('active', b === btn));
      // Switching tools clears any drag-in-progress preview.
      if (st.tool !== 'select') { clearSelection(); }
      redrawOverlay();
    });
  });
  toolbar.querySelector('.paint-color').addEventListener('input', e => {
    st.color = e.target.value;
  });
  const sizeInput = toolbar.querySelector('.paint-size');
  const sizeLabel = toolbar.querySelector('.paint-size-label');
  sizeInput.addEventListener('input', e => {
    st.size = parseInt(e.target.value, 10);
    sizeLabel.textContent = st.size + 'px';
  });
  toolbar.querySelector('.paint-undo').addEventListener('click', () => undo());
  toolbar.querySelector('.paint-redo').addEventListener('click', () => redo());
  toolbar.querySelector('.paint-zoom-in').addEventListener('click',
    () => zoom(+1));
  toolbar.querySelector('.paint-zoom-out').addEventListener('click',
    () => zoom(-1));
  toolbar.querySelector('.paint-zoom-label').addEventListener('click',
    () => zoomReset());

  // ----- pointer helpers ------------------------------------------------
  // Map a pointer event to canvas-pixel coordinates (handles CSS scaling).
  const ptFromEvent = e => {
    const r = base.getBoundingClientRect();
    return {
      x: (e.clientX - r.left) * (base.width  / r.width),
      y: (e.clientY - r.top)  * (base.height / r.height),
    };
  };

  const drawStroke = (ctx, x0, y0, x1, y1) => {
    ctx.lineCap   = 'round';
    ctx.lineJoin  = 'round';
    ctx.strokeStyle = st.color;
    ctx.lineWidth = st.size;
    ctx.globalCompositeOperation = (st.tool === 'eraser')
      ? 'destination-out' : 'source-over';
    ctx.beginPath();
    ctx.moveTo(x0, y0);
    ctx.lineTo(x1, y1);
    ctx.stroke();
    ctx.globalCompositeOperation = 'source-over';
  };

  // ----- undo / redo ----------------------------------------------------
  const undoStack = [];
  const redoStack = [];
  const MAX_UNDO = 30;
  const snapshot = () => {
    const c = document.createElement('canvas');
    c.width = base.width; c.height = base.height;
    c.getContext('2d').drawImage(base, 0, 0);
    return c;
  };
  const pushUndo = () => {
    undoStack.push(snapshot());
    if (undoStack.length > MAX_UNDO) undoStack.shift();
    redoStack.length = 0;
  };
  const restoreSnapshot = src => {
    bctx.globalCompositeOperation = 'source-over';
    bctx.clearRect(0, 0, base.width, base.height);
    bctx.drawImage(src, 0, 0);
  };
  const undo = () => {
    if (!undoStack.length) return;
    redoStack.push(snapshot());
    restoreSnapshot(undoStack.pop());
    markImageDirty(path);
  };
  const redo = () => {
    if (!redoStack.length) return;
    undoStack.push(snapshot());
    restoreSnapshot(redoStack.pop());
    markImageDirty(path);
  };

  // ----- selection ------------------------------------------------------
  const clearSelection = () => { st.selection = null; };
  let antsTimer = null;
  let antsOffset = 0;
  const startAnts = () => {
    if (antsTimer) return;
    antsTimer = setInterval(() => {
      antsOffset = (antsOffset + 1) % 8;
      redrawOverlay();
    }, 100);
  };
  const stopAnts = () => { if (antsTimer) { clearInterval(antsTimer); antsTimer = null; } };
  const redrawOverlay = () => {
    octx.clearRect(0, 0, overlay.width, overlay.height);
    if (st.selection) {
      const { x, y, w, h } = st.selection;
      octx.lineWidth = Math.max(1, 1 / st.zoom);
      octx.setLineDash([4, 4]);
      octx.lineDashOffset = -antsOffset;
      octx.strokeStyle = '#000';
      octx.strokeRect(x + 0.5, y + 0.5, w - 1, h - 1);
      octx.strokeStyle = '#fff';
      octx.lineDashOffset = -antsOffset + 4;
      octx.strokeRect(x + 0.5, y + 0.5, w - 1, h - 1);
      octx.setLineDash([]);
      startAnts();
    } else {
      stopAnts();
    }
  };

  // ----- pointer events on overlay -------------------------------------
  overlay.addEventListener('pointerdown', e => {
    if (e.button !== 0) return;
    overlay.setPointerCapture(e.pointerId);
    const p = ptFromEvent(e);
    st.drawing = true;
    st.startX = p.x; st.startY = p.y;
    st.lastX  = p.x; st.lastY  = p.y;
    if (st.tool === 'select') {
      st.selecting = true;
      clearSelection();
      stopAnts();
      octx.clearRect(0, 0, overlay.width, overlay.height);
      return;
    }
    // Any non-select drawing tool: snapshot for undo, then start.
    pushUndo();
    if (st.tool === 'brush' || st.tool === 'eraser') {
      drawStroke(bctx, p.x, p.y, p.x + 0.01, p.y + 0.01);  // dot
    }
  });
  overlay.addEventListener('pointermove', e => {
    if (!st.drawing) return;
    const p = ptFromEvent(e);
    if (st.tool === 'brush' || st.tool === 'eraser') {
      drawStroke(bctx, st.lastX, st.lastY, p.x, p.y);
      st.lastX = p.x; st.lastY = p.y;
      markImageDirty(path);
    } else if (st.tool === 'select') {
      const sx = Math.min(st.startX, p.x);
      const sy = Math.min(st.startY, p.y);
      const sw = Math.abs(p.x - st.startX);
      const sh = Math.abs(p.y - st.startY);
      st.selection = { x: sx, y: sy, w: sw, h: sh };
      redrawOverlay();
    } else {
      // Live shape preview on overlay.
      octx.clearRect(0, 0, overlay.width, overlay.height);
      octx.strokeStyle = st.color;
      octx.lineWidth   = st.size;
      octx.lineCap     = 'round';
      if (st.tool === 'line') {
        octx.beginPath();
        octx.moveTo(st.startX, st.startY);
        octx.lineTo(p.x, p.y);
        octx.stroke();
      } else if (st.tool === 'rect') {
        const x = Math.min(st.startX, p.x);
        const y = Math.min(st.startY, p.y);
        const w = Math.abs(p.x - st.startX);
        const h = Math.abs(p.y - st.startY);
        octx.strokeRect(x, y, w, h);
      }
    }
  });
  overlay.addEventListener('pointerup', e => {
    if (!st.drawing) return;
    st.drawing = false;
    const p = ptFromEvent(e);
    if (st.tool === 'select') {
      st.selecting = false;
      if (st.selection && (st.selection.w < 2 || st.selection.h < 2)) {
        clearSelection();
        octx.clearRect(0, 0, overlay.width, overlay.height);
      } else {
        redrawOverlay();
      }
      return;
    }
    if (st.tool === 'line') {
      drawStroke(bctx, st.startX, st.startY, p.x, p.y);
    } else if (st.tool === 'rect') {
      bctx.strokeStyle = st.color;
      bctx.lineWidth   = st.size;
      const x = Math.min(st.startX, p.x);
      const y = Math.min(st.startY, p.y);
      const w = Math.abs(p.x - st.startX);
      const h = Math.abs(p.y - st.startY);
      bctx.strokeRect(x, y, w, h);
    }
    octx.clearRect(0, 0, overlay.width, overlay.height);
    markImageDirty(path);
  });

  // ----- zoom -----------------------------------------------------------
  const ZOOM_STEPS = [0.1, 0.25, 0.5, 0.75, 1, 1.5, 2, 3, 4, 6, 8, 12, 16];
  const zoom = dir => {
    const i = ZOOM_STEPS.findIndex(z => Math.abs(z - st.zoom) < 0.001);
    const cur = (i >= 0) ? i : ZOOM_STEPS.findIndex(z => z >= st.zoom);
    const next = Math.max(0, Math.min(ZOOM_STEPS.length - 1,
                                      (cur < 0 ? ZOOM_STEPS.length - 1 : cur) + dir));
    st.zoom = ZOOM_STEPS[next];
    applyZoom();
    redrawOverlay();
  };
  const zoomReset = () => { st.zoom = 1; applyZoom(); redrawOverlay(); };

  // Ctrl+Wheel anywhere in the stack zooms (centered on cursor).
  stack.addEventListener('wheel', e => {
    if (!(e.ctrlKey || e.metaKey)) return;
    e.preventDefault();
    zoom(e.deltaY < 0 ? +1 : -1);
  }, { passive: false });

  // ----- copy / cut / paste / delete -----------------------------------
  const copyRegion = () => {
    if (!st.selection) return null;
    const { x, y, w, h } = st.selection;
    const tmp = document.createElement('canvas');
    tmp.width = Math.max(1, Math.round(w));
    tmp.height = Math.max(1, Math.round(h));
    tmp.getContext('2d').drawImage(
      base, Math.round(x), Math.round(y),
      tmp.width, tmp.height, 0, 0, tmp.width, tmp.height);
    return tmp;
  };
  const writeToSystemClipboard = canvas => {
    if (!navigator.clipboard || !window.ClipboardItem) return;
    canvas.toBlob(blob => {
      if (!blob) return;
      try {
        navigator.clipboard.write([new ClipboardItem({ [blob.type]: blob })])
          .catch(() => {});
      } catch {}
    }, 'image/png');
  };

  const copy = () => {
    const c = copyRegion();
    if (!c) return;
    st.clipboard = c;
    writeToSystemClipboard(c);
  };
  const cut = () => {
    const c = copyRegion();
    if (!c) return;
    st.clipboard = c;
    writeToSystemClipboard(c);
    pushUndo();
    const { x, y, w, h } = st.selection;
    bctx.clearRect(Math.round(x), Math.round(y), Math.round(w), Math.round(h));
    markImageDirty(path);
    redrawOverlay();
  };
  const deleteSelection = () => {
    if (!st.selection) return;
    pushUndo();
    const { x, y, w, h } = st.selection;
    bctx.clearRect(Math.round(x), Math.round(y), Math.round(w), Math.round(h));
    clearSelection();
    octx.clearRect(0, 0, overlay.width, overlay.height);
    markImageDirty(path);
  };
  const pasteCanvas = pasted => {
    if (!pasted) return;
    pushUndo();
    const px = st.selection ? Math.round(st.selection.x) : 0;
    const py = st.selection ? Math.round(st.selection.y) : 0;
    bctx.drawImage(pasted, px, py);
    // Make the pasted region the new selection so the user can move/edit it.
    st.selection = { x: px, y: py, w: pasted.width, h: pasted.height };
    st.tool = 'select';
    toolbar.querySelectorAll('.paint-tool').forEach(b =>
      b.classList.toggle('active', b.dataset.tool === 'select'));
    markImageDirty(path);
    redrawOverlay();
  };
  const paste = () => pasteCanvas(st.clipboard);
  const pasteBlob = async blob => {
    const bmp = await createImageBitmap(blob);
    const tmp = document.createElement('canvas');
    tmp.width = bmp.width; tmp.height = bmp.height;
    tmp.getContext('2d').drawImage(bmp, 0, 0);
    pasteCanvas(tmp);
  };

  return {
    save:            () => saveImageFile(path),
    getCanvas:       () => base,
    undo, redo,
    copy, cut, paste, pasteBlob,
    deleteSelection,
    hasSelection:    () => !!st.selection,
    zoom, zoomReset,
    snapshotDataURL: () => base.toDataURL(),
    destroy:         () => { stopAnts(); },
  };
}

function markImageDirty(path) {
  const f = state.files[path];
  if (!f) return;
  setFileDirty(path, true);
}

async function saveImageFile(path) {
  const f = state.files[path];
  if (!f || !f.paint) return;
  const canvas = f.paint.getCanvas();
  // Pick a sensible mime from the file extension.
  const ext = (path.split('.').pop() || 'png').toLowerCase();
  const mime = (ext === 'jpg' || ext === 'jpeg') ? 'image/jpeg' :
               (ext === 'webp') ? 'image/webp' : 'image/png';
  const blob = await new Promise(res => canvas.toBlob(res, mime, 0.92));
  if (!blob) { alert('save failed: blob conversion'); return; }
  try {
    const r = await fetch('/api/fs/write_raw?path=' + encodeURIComponent(path), {
      method: 'POST',
      headers: { 'Content-Type': mime },
      body: blob,
    });
    if (!r.ok) {
      const j = await r.json().catch(() => ({}));
      alert('save failed: ' + (j.error || r.status));
      return;
    }
    setFileDirty(path, false);
    // Refresh the view <img> so it picks up the saved pixels.
    const v = f.surface.querySelector('.image-view-wrap img');
    if (v) v.src = '/api/fs/raw?path=' + encodeURIComponent(path) + '&_t=' + Date.now();
  } catch (err) {
    alert('save failed: ' + err.message);
  }
}

// ===== Vision: send an image to the AI for description ==================
async function analyzeImage(path, prompt) {
  const aiEl = pushMsg('user', '[image] ' + path);
  const resp = pushMsg('ai', 'looking at image…');
  try {
    const r = await fetch('/api/vision', {
      method: 'POST', headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        path,
        prompt: prompt || 'Describe what you see in this image. Be specific and concrete.',
      }),
    });
    const j = await r.json().catch(() => ({}));
    if (!r.ok) {
      resp.querySelector('.body').textContent = 'vision error: ' + (j.error || r.status);
      return;
    }
    resp.querySelector('.body').textContent = j.text || '(empty response)';
  } catch (err) {
    resp.querySelector('.body').textContent = 'vision error: ' + err.message;
  }
}

function activateFile(path) {
  const f = state.files[path];
  if (!f) return;
  state.activeFilePath = path;
  for (const [p, ff] of Object.entries(state.files)) {
    ff.surface.classList.toggle('active', p === path);
    ff.tab.classList.toggle('active', p === path);
  }
  // Focus the editable surface for immediate typing / Ctrl+S.
  const ta = f.surface.querySelector('textarea');
  if (ta) ta.focus();
  saveState();
}

function setFileDirty(path, dirty) {
  const f = state.files[path];
  if (!f) return;
  f.dirty = dirty;
  f.tab.classList.toggle('dirty', dirty);
}

async function saveFile(path, content) {
  try {
    const r = await fetch('/api/fs/write', {
      method:'POST', headers:{'Content-Type':'application/json'},
      body: JSON.stringify({path, content}),
    });
    if (!r.ok) {
      const j = await r.json().catch(()=>({}));
      alert('save failed: ' + (j.error || r.status));
      return;
    }
    const j = await r.json().catch(() => ({}));
    const f = state.files[path];
    if (f) {
      f.savedContent = content;
      // Adopt the post-write mtime so the external-change poller doesn't
      // mistake our own save as a foreign edit and immediately reload.
      if (typeof j.mtime_ns === 'string') f.mtimeNs = j.mtime_ns;
      setFileDirty(path, false);
    }
  } catch (err) {
    alert('save failed: ' + err.message);
  }
}

function closeFile(path) {
  const f = state.files[path];
  if (!f) return;
  if (f.dirty && !confirm(`Discard unsaved changes to ${path.split('/').pop()}?`)) return;
  try { f.destroy(); } catch {}
  f.surface.remove();
  f.tab.remove();
  delete state.files[path];
  if (state.activeFilePath === path) {
    const next = Object.keys(state.files)[0];
    if (next) activateFile(next);
    else {
      state.activeFilePath = null;
      const empty = document.createElement('div');
      empty.className = 'editor-empty hint';
      empty.textContent = 'No file open. Use File → Open Folder, then click a file.';
      editorBody.appendChild(empty);
    }
  }
  saveState();
}

// Global Ctrl+S: save the currently active file.
document.addEventListener('keydown', e => {
  if ((e.ctrlKey || e.metaKey) && e.key === 's') {
    const path = state.activeFilePath;
    if (!path) return;
    const f = state.files[path];
    if (!f) return;
    e.preventDefault();
    if (f.mode === 'image' && f.imgMode === 'edit') {
      saveImageFile(path);
    } else {
      saveFile(path, f.getContent());
    }
  }
});

// Image-edit keyboard shortcuts: Ctrl+Z / Ctrl+Y (or Ctrl+Shift+Z) /
// Ctrl+C / Ctrl+X / Ctrl+V / Delete. Only active when an image tab is in
// edit mode. We don't intercept browser copy/cut/paste inside text inputs.
document.addEventListener('keydown', e => {
  const path = state.activeFilePath;
  if (!path) return;
  const f = state.files[path];
  if (!f || f.mode !== 'image' || f.imgMode !== 'edit' || !f.paint) return;
  // Don't hijack keys while typing into an input/textarea anywhere.
  const t = e.target;
  if (t && (t.tagName === 'INPUT' || t.tagName === 'TEXTAREA' ||
            t.isContentEditable)) return;
  const meta = e.ctrlKey || e.metaKey;
  if (meta && e.key.toLowerCase() === 'z' && !e.shiftKey) {
    e.preventDefault(); f.paint.undo();
  } else if (meta && (e.key.toLowerCase() === 'y' ||
                      (e.key.toLowerCase() === 'z' && e.shiftKey))) {
    e.preventDefault(); f.paint.redo();
  } else if (meta && e.key.toLowerCase() === 'c') {
    if (!f.paint.hasSelection()) return;
    e.preventDefault(); f.paint.copy();
  } else if (meta && e.key.toLowerCase() === 'x') {
    if (!f.paint.hasSelection()) return;
    e.preventDefault(); f.paint.cut();
  } else if (meta && e.key.toLowerCase() === 'v') {
    // OS-clipboard paste flows through the 'paste' event below; this only
    // fires for internal-only-clipboard fallback. preventDefault keeps the
    // browser from inserting text somewhere unhelpful.
    e.preventDefault(); f.paint.paste();
  } else if ((e.key === 'Delete' || e.key === 'Backspace') &&
             f.paint.hasSelection()) {
    e.preventDefault(); f.paint.deleteSelection();
  } else if (meta && (e.key === '+' || e.key === '=')) {
    e.preventDefault(); f.paint.zoom(+1);
  } else if (meta && e.key === '-') {
    e.preventDefault(); f.paint.zoom(-1);
  } else if (meta && e.key === '0') {
    e.preventDefault(); f.paint.zoomReset();
  }
});

// Paste an image from the OS clipboard into the active image-edit canvas.
document.addEventListener('paste', e => {
  const path = state.activeFilePath;
  if (!path) return;
  const f = state.files[path];
  if (!f || f.mode !== 'image' || f.imgMode !== 'edit' || !f.paint) return;
  const items = e.clipboardData ? e.clipboardData.items : [];
  for (const item of items) {
    if (item.type && item.type.startsWith('image/')) {
      e.preventDefault();
      const blob = item.getAsFile();
      if (blob) f.paint.pasteBlob(blob);
      return;
    }
  }
});

function createInRoot(kind) {
  if (!state.rootDir) {
    // Quiet message in the chat panel; no browser popup.
    pushMsg('ai', 'Open a folder first (File → Open Folder).');
    return;
  }
  // Remove any prior inline editor.
  filesTree.querySelector('.fs-new-inline')?.remove();
  const wrap = document.createElement('div');
  wrap.className = 'fs-new-inline';
  wrap.innerHTML =
    `<input type="text" placeholder="${kind === 'folder' ? 'new folder name…' : 'new file name…'}">`;
  filesTree.insertBefore(wrap, filesTree.firstChild);
  const input = wrap.querySelector('input');
  input.focus();

  let committed = false;
  const commit = async () => {
    if (committed) return;
    committed = true;
    const name = input.value.trim();
    wrap.remove();
    if (!name) return;
    const path = joinPath(state.rootDir, name);
    if (kind === 'folder') {
      await fetch('/api/fs/mkdir', {method:'POST', headers:{'Content-Type':'application/json'},
        body: JSON.stringify({path})});
    } else {
      await fetch('/api/fs/write', {method:'POST', headers:{'Content-Type':'application/json'},
        body: JSON.stringify({path, content:''})});
    }
    refreshFileTree();
    if (kind === 'file') openFile(path);
  };
  input.addEventListener('keydown', e => {
    if (e.key === 'Escape') { committed = true; wrap.remove(); }
    if (e.key === 'Enter')  { e.preventDefault(); commit(); }
  });
  input.addEventListener('blur', () => commit());
}
async function deleteSelected() {
  const path = state.activeFilePath;
  if (!path) { alert('No file selected'); return; }
  if (!confirm('Delete ' + path + '?')) return;
  await fetch('/api/fs/delete?path=' + encodeURIComponent(path), {method:'DELETE'});
  const f = state.files[path];
  if (f) {
    try { f.destroy(); } catch {}
    f.surface.remove();
    f.tab.remove();
    delete state.files[path];
  }
  state.activeFilePath = Object.keys(state.files)[0] || null;
  if (state.activeFilePath) activateFile(state.activeFilePath);
  else {
    const empty = document.createElement('div');
    empty.className = 'editor-empty hint';
    empty.textContent = 'No file open.';
    editorBody.appendChild(empty);
  }
  refreshFileTree();
  saveState();
}

// ---- AI chat ----------------------------------------------------------
const chatForm  = document.getElementById('chat-form');
const chatInput = document.getElementById('chat-input');
const chatLog   = document.getElementById('chat-log');

// Drop an image from the file tree onto the AI pane to trigger vision
// analysis. Both the log and the input field accept the drop.
const chatPane  = document.getElementById('pane-chat');
const IMG_EXTS  = new Set(['png','jpg','jpeg','gif','webp','bmp']);
const isImagePath = p => {
  const e = (p.split('.').pop() || '').toLowerCase();
  return IMG_EXTS.has(e);
};
chatPane.addEventListener('dragover', e => {
  if (e.dataTransfer.types.includes('text/x-tool-path')) {
    e.preventDefault();
    e.dataTransfer.dropEffect = 'copy';
    chatPane.classList.add('drop-target');
  }
});
chatPane.addEventListener('dragleave', e => {
  if (e.target === chatPane) chatPane.classList.remove('drop-target');
});
chatPane.addEventListener('drop', e => {
  chatPane.classList.remove('drop-target');
  const path = e.dataTransfer.getData('text/x-tool-path');
  if (!path) return;
  e.preventDefault();
  if (!isImagePath(path)) {
    pushMsg('ai', 'drop an image (png/jpg/webp/…) to analyse.');
    return;
  }
  analyzeImage(path);
});

// Delegated click handler for links and download buttons emitted by
// formatChatMarkdown. Keeps the rest of the rendered markdown inert.
chatLog.addEventListener('click', e => {
  const dl = e.target.closest('.chat-dl');
  if (dl) {
    e.preventDefault();
    triggerDownload(dl.dataset.url);
    return;
  }
  const lnk = e.target.closest('.chat-link');
  if (lnk) {
    e.preventDefault();
    openBrowserTab(lnk.dataset.url);
  }
});

// Server-side fetch into <project>/Downloads/. We report the saved path
// back into the chat so the user can confirm and so a subsequent file-tree
// refresh shows the new file.
async function triggerDownload(url, suggestedName) {
  const tag = pushMsg('ai', '↓ downloading: ' + url);
  try {
    const r = await fetch('/api/download', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        url,
        cwd:      state.rootDir || '',
        filename: suggestedName || '',
      }),
    });
    const j = await r.json().catch(() => ({}));
    if (!r.ok) {
      tag.querySelector('.body').textContent =
        'download failed: ' + (j.error || r.status);
      return;
    }
    const kb = j.size ? ' (' + Math.round(j.size / 1024) + ' KB)' : '';
    tag.querySelector('.body').textContent = '↓ saved: ' + j.path + kb;
    refreshFileTreeIfOpen();
  } catch (err) {
    tag.querySelector('.body').textContent = 'download error: ' + err.message;
  }
}

// ===== In-tool browser tab =============================================
// Mouser-style sites usually block iframing via X-Frame-Options, in which
// case the iframe just stays blank. The toolbar's download button still
// works because it pulls the URL server-side. For iframable sites
// (datasheet PDFs on CDNs, archive.org, wikipedia, …) this gives a real
// in-app navigation experience.
function shortenUrlForTab(url) {
  try {
    const u = new URL(url);
    let s = u.host;
    if (u.pathname && u.pathname !== '/') {
      const seg = u.pathname.split('/').filter(Boolean).pop();
      if (seg) s += '/…/' + seg;
    }
    return s.length > 40 ? s.slice(0, 39) + '…' : s;
  } catch { return url.slice(0, 40); }
}

function openBrowserTab(url) {
  const tabKey = '__browser:' + url;
  if (state.files[tabKey]) { activateFile(tabKey); return; }

  const empty = editorBody.querySelector('.editor-empty');
  if (empty) empty.remove();

  const surface = document.createElement('div');
  surface.className = 'editor-surface';
  surface.dataset.path = tabKey;
  editorBody.appendChild(surface);
  for (const ff of Object.values(state.files)) ff.surface.classList.remove('active');
  surface.classList.add('active');

  const wrap = document.createElement('div');
  wrap.className = 'browser-wrap';
  wrap.innerHTML =
    `<div class="browser-toolbar">
       <button class="browser-back"     title="Back">◀</button>
       <button class="browser-fwd"      title="Forward">▶</button>
       <button class="browser-refresh"  title="Reload">⟲</button>
       <input  class="browser-url" type="text" spellcheck="false">
       <button class="browser-go"       title="Go">Go</button>
       <button class="browser-download" title="Save current URL to project /Downloads/">📥</button>
       <button class="browser-extern"   title="Open in workstation browser">↗</button>
     </div>
     <div class="browser-body"></div>`;
  surface.appendChild(wrap);

  const urlInput = wrap.querySelector('.browser-url');
  const body     = wrap.querySelector('.browser-body');
  urlInput.value = url;

  const history  = [];
  let   histIdx  = -1;

  const render = () => {
    body.innerHTML = '';
    const iframe = document.createElement('iframe');
    iframe.className = 'browser-iframe';
    // sandbox keeps the embedded page from navigating the top window or
    // popping new tabs without us seeing it. allow-scripts/forms so it
    // remains interactive; allow-same-origin so it can fetch its own
    // resources where the host permits.
    iframe.setAttribute('sandbox',
      'allow-scripts allow-forms allow-popups allow-same-origin');
    iframe.src = history[histIdx];
    body.appendChild(iframe);
  };
  const navigate = (newUrl) => {
    if (!newUrl) return;
    if (!/^https?:\/\//i.test(newUrl)) newUrl = 'https://' + newUrl;
    // Truncate forward history if we navigated mid-history.
    if (histIdx < history.length - 1) history.length = histIdx + 1;
    history.push(newUrl);
    histIdx = history.length - 1;
    urlInput.value = newUrl;
    state.files[tabKey].url = newUrl;
    render();
  };

  wrap.querySelector('.browser-back').addEventListener('click', () => {
    if (histIdx > 0) {
      histIdx--;
      urlInput.value = history[histIdx];
      state.files[tabKey].url = history[histIdx];
      render();
    }
  });
  wrap.querySelector('.browser-fwd').addEventListener('click', () => {
    if (histIdx < history.length - 1) {
      histIdx++;
      urlInput.value = history[histIdx];
      state.files[tabKey].url = history[histIdx];
      render();
    }
  });
  wrap.querySelector('.browser-refresh').addEventListener('click', render);
  wrap.querySelector('.browser-go').addEventListener('click',
    () => navigate(urlInput.value));
  urlInput.addEventListener('keydown', e => {
    if (e.key === 'Enter') { e.preventDefault(); navigate(urlInput.value); }
  });
  wrap.querySelector('.browser-download').addEventListener('click',
    () => triggerDownload(urlInput.value));
  wrap.querySelector('.browser-extern').addEventListener('click',
    () => window.open(urlInput.value, '_blank', 'noopener'));

  state.files[tabKey] = {
    mode:    'browser',
    surface, tab: null,
    savedContent: '',
    getContent:   () => '',
    destroy:      () => {},
    dirty:        false,
    url,
  };
  buildEditorTab(tabKey, 'browser');
  activateFile(tabKey);
  navigate(url);
  saveState();
}

// ---- task-plan checklist -------------------------------------------------
// Visible in the AI bubble, OUTSIDE the collapsed thinking chain. The
// pipeline emits a "tasks" layer holding the numbered plan and one
// "task i/N" layer as each step completes; this widget mirrors them live
// (○ pending, ▸ running, ✓ done, ✗ failed) and re-materializes the final
// state when a session replays.
function makeTaskList(container, beforeEl) {
  const box = document.createElement('div');
  box.className = 'task-list';
  container.insertBefore(box, beforeEl || null);
  const rows = [];
  const setActive = i => {
    if (i < rows.length) {
      rows[i].classList.add('active');
      rows[i].querySelector('.task-mark').textContent = '▸';
      // The list scrolls once it exceeds its max-height; keep the row
      // that is actually running visible.
      rows[i].scrollIntoView({block: 'nearest'});
    }
  };
  return {
    plan(content) {
      box.innerHTML = '';
      rows.length = 0;
      for (const line of String(content).split('\n')) {
        const m = line.match(/^\s*\d+\.\s+(.*\S)/);
        if (!m) continue;
        const row = document.createElement('div');
        row.className = 'task-row pending';
        row.innerHTML =
          '<span class="task-mark">○</span><span class="task-text"></span>';
        row.querySelector('.task-text').textContent = m[1];
        box.appendChild(row);
        rows.push(row);
      }
      setActive(0);
    },
    complete(i, ok) {
      if (i < 0 || i >= rows.length) return;
      const row = rows[i];
      row.classList.remove('pending', 'active');
      row.classList.add(ok ? 'done' : 'failed');
      row.querySelector('.task-mark').textContent = ok ? '✓' : '✗';
      setActive(i + 1);
    },
  };
}
const TASK_LAYER_RE = /^task (\d+)\/\d+$/;
const taskLayerOk = content => /\[exit 0\]\s*$/.test(String(content).trim());

// Ticket runner and future scripting hooks share this callable so they
// drive the same UI-visible chat flow. Returns the final JSON payload
// from the pipeline, or null if the turn was cancelled or errored out.
// One turn at a time: overlapping calls resolve immediately to null.
// `cwd` overrides state.rootDir; leave null to use the current project.
window.__chatBusy = false;
async function runChatTurn(text, cwd) {
  if (window.__chatBusy) return null;
  window.__chatBusy = true;
  try {
    return await _runChatTurnImpl(text, cwd);
  } finally {
    window.__chatBusy = false;
  }
}
window.runChatTurn = runChatTurn;

chatForm.addEventListener('submit', async e => {
  e.preventDefault();
  const text = chatInput.value.trim();
  if (!text) return;
  chatInput.value = '';
  await runChatTurn(text);
});

async function _runChatTurnImpl(text, cwd) {
  const effectiveCwd = (typeof cwd === 'string' && cwd) ? cwd :
                       (state.rootDir || '');
  // Model map is what turns raw role slugs ("coder-big") into the
  // human-readable "thinking (Qwen3-Coder-30B)" the ticket pane shows.
  // The ticket flow awaits this same call before subscribing; without
  // it the first layers land while shorts is empty and the headline
  // stays as "thinking…".
  await loadModelsMap();
  const userEl = pushMsg('user', text);

  // Set up the AI message with a streaming "thinking" expander up-front.
  // Layers append into the expander as SSE events arrive; the final event
  // sets the visible headline (handler output).
  const aiEl = pushMsg('ai', '');
  const body = aiEl.querySelector('.body');
  body.innerHTML = '';
  // Three-ring node-status widget across the top of the ai reply:
  // system (RAM/CPU/temp) + one per GPU (VRAM/util/temp). Same widget
  // the ticket runner puts on its ai bubble.
  const gpuStrip = startAiGpuStrip(body);
  // Thin progress bar under the strip, driven by heartbeat token count.
  const progWrap = document.createElement('div');
  progWrap.className = 'ai-progbar';
  const progFill = document.createElement('div');
  progFill.className = 'ai-progbar-fill';
  progWrap.appendChild(progFill);
  body.appendChild(progWrap);
  // Header row: "thinking (model)" left, elapsed clock right.
  const headRow = document.createElement('div');
  headRow.className = 'ai-headrow';
  const headlinePre = document.createElement('pre');
  headlinePre.className = 'ai-headline';
  headlinePre.textContent = 'thinking…';
  const clockEl = document.createElement('div');
  clockEl.className = 'ai-clock';
  clockEl.textContent = '0:00';
  headRow.appendChild(headlinePre);
  headRow.appendChild(clockEl);
  body.appendChild(headRow);

  const chain = document.createElement('details');
  chain.className = 'chain';
  chain.innerHTML = '<summary></summary><div class="layers"></div>';
  const summary  = chain.querySelector('summary');
  const layersEl = chain.querySelector('.layers');
  // The label lives in its own span: setting summary.textContent would
  // wipe the stop button appended next to it.
  const summaryText = document.createElement('span');
  summaryText.textContent = 'thinking… (0 layers)';
  summary.appendChild(summaryText);
  body.appendChild(chain);
  // Running elapsed clock, ticks every 250ms.
  const t0 = Date.now();
  const clockTimer = setInterval(() => {
    const s = Math.floor((Date.now() - t0) / 1000);
    clockEl.textContent = Math.floor(s / 60) + ':' +
      String(s % 60).padStart(2, '0');
  }, 250);
  // A tiny curCtx handle so applyHeartbeatProgress and the role
  // headline logic work exactly like they do for tickets. layerCount
  // is bumped by appendLayer below (not by the ticket handler).
  const curCtx = {
    headlinePre, layersEl, summaryText, layerCount: 0,
    progFill, clockEl, clockTimer, t0, gpuStrip, activeRole: null,
  };

  // The rainbow "thinking" animation runs ONLY while server heartbeats
  // arrive (one per second of real token generation). No heartbeat for
  // 2.5s means the pipeline isn't visibly producing anything, and the
  // animation freezes rather than lying about progress.
  let hbTimer = null;
  const hbStop = () => {
    if (hbTimer) { clearTimeout(hbTimer); hbTimer = null; }
    headlinePre.classList.remove('thinking-live');
    summary.classList.remove('thinking-live');
  };
  const noteHeartbeat = () => {
    headlinePre.classList.add('thinking-live');
    summary.classList.add('thinking-live');
    if (hbTimer) clearTimeout(hbTimer);
    hbTimer = setTimeout(hbStop, 12000);
  };

  // Stop button: halts the turn server-side (the pipeline erases the
  // turn from the session), drops this exchange from the log, and puts
  // the prompt back in the entry field for editing and resubmission.
  const controller = new AbortController();
  let stoppedByUser = false;
  // Shared teardown so stop / final / error / abort all leave the pane
  // in a consistent state — no orphan timers, no rings still polling,
  // no progress bar. The clock element stays in place (frozen at the
  // total elapsed time) so the finished bubble displays how long the
  // whole turn took.
  const teardownWidgets = () => {
    if (curCtx.clockTimer) {
      // Last tick so the frozen clock value reflects the exact moment
      // the turn ended, not the interval sample from up to 250 ms ago.
      const s = Math.floor((Date.now() - curCtx.t0) / 1000);
      curCtx.clockEl.textContent =
        Math.floor(s / 60) + ':' + String(s % 60).padStart(2, '0');
      clearInterval(curCtx.clockTimer);
      curCtx.clockTimer = null;
    }
    if (curCtx.gpuStrip) { stopAiGpuStrip(curCtx.gpuStrip); curCtx.gpuStrip = null; }
    // Remove the progress bar entirely. Its parent is progWrap, which
    // was appended directly to the AI bubble body.
    if (curCtx.progFill && curCtx.progFill.parentNode) {
      curCtx.progFill.parentNode.remove();
      curCtx.progFill = null;
    }
  };
  const stopBtn = document.createElement('button');
  stopBtn.type = 'button';
  stopBtn.className = 'chat-stop';
  stopBtn.textContent = '◼ stop';
  stopBtn.title = 'Stop this turn and put the prompt back for editing';
  summary.appendChild(stopBtn);
  const removeStop = () => stopBtn.remove();
  stopBtn.addEventListener('click', e => {
    e.preventDefault();
    e.stopPropagation();               // don't toggle the <details>
    stoppedByUser = true;
    hbStop();
    teardownWidgets();
    fetch('/api/chat/stop', {method: 'POST'}).catch(() => {});
    try { controller.abort(); } catch {}
    userEl.remove();
    aiEl.remove();
    chatInput.value = text;
    chatInput.focus();
    chatInput.setSelectionRange(text.length, text.length);
  });

  let layerCount = 0;
  let taskList = null;
  const appendLayer = (name, content) => {
    // Task-plan turns get a live checklist in the bubble itself, outside
    // the collapsed chain (the raw layers still land in the chain too).
    if (name === 'tasks') {
      if (!taskList) taskList = makeTaskList(body, chain);
      taskList.plan(content);
    } else if (taskList) {
      const tm = name.match(TASK_LAYER_RE);
      if (tm) taskList.complete(parseInt(tm[1], 10) - 1, taskLayerOk(content));
    }
    const div = document.createElement('div');
    div.className = 'layer';
    div.innerHTML = '<div class="lab"></div><div class="payload"></div>';
    div.querySelector('.lab').textContent = name;
    div.querySelector('.payload').textContent = content;
    layersEl.appendChild(div);
    ++layerCount;
    curCtx.layerCount = layerCount;
    // Update headline text with the model doing this layer, e.g.
    // "thinking (Q3 Think 30)". Same wiring as the ticket flow at
    // ticketsSubscribeEvents' layer handler.
    const active = (g_modelsMap && g_modelsMap.active) || {};
    const role   = layerToRole(name, active);
    const shorts = (g_modelsMap && g_modelsMap.shorts) || {};
    const short  = role ? shorts[role] : null;
    const roleGpu = (window.__ac9RoleGpu || {})[role];
    const suffix  = (typeof roleGpu === 'number') ? ' on gpu ' + roleGpu : '';
    if (short) {
      headlinePre.textContent = 'thinking (' + short + suffix + ')';
      summaryText.textContent =
        `thinking (${short}${suffix}) — ${layerCount} layers`;
    } else {
      summaryText.textContent =
        `thinking… (${layerCount} layer${layerCount === 1 ? '' : 's'})`;
    }
    noteHeartbeat();
    chatLog.scrollTop = chatLog.scrollHeight;
  };

  let capturedFinal = null;
  const onFinal = j => {
    capturedFinal = j;
    hbStop();
    removeStop();
    teardownWidgets();
    if (progFill) {
      progFill.classList.remove('indeterminate');
      progFill.style.width = '100%';
    }
    // Record the last-seen token count as the rolling avg for
    // whatever role was actively producing when we hit final.
    if (curCtx.activeRole && j && j.handler && j.handler.stdout) {
      const est = Math.max(50, j.handler.stdout.length >> 2);
      writeRoleAvg(curCtx.activeRole, est);
    }
    summaryText.textContent = `thinking (${layerCount} layers)`;
    headlinePre.innerHTML = formatChatMarkdown(computeHeadline(j));
    highlightCodeIn(headlinePre);
    // Tag info goes INSIDE the chain, not the visible headline.
    const tag = document.createElement('div');
    tag.className = 'layer';
    tag.innerHTML = '<div class="lab">tags</div><div class="payload"></div>';
    const act = j.act || {};
    const tags = (act.tags || []).join(',');
    tag.querySelector('.payload').textContent =
      `[act=${act.act || '?'}${act.subtype ? ' subtype=' + act.subtype : ''}` +
      `${tags ? ' tags=' + tags : ''}] [${j.expertise || '?'}]`;
    layersEl.appendChild(tag);
    if (j.handler && j.handler.kind === 'shell') refreshFileTreeIfOpen();
    // When the components tool wrote a markdown file, refresh the tree
    // and pop the file open in a new editor tab so the user can see /
    // edit the result directly.
    if (j.handler && j.handler.kind === 'components_answer' &&
        j.handler.file_path) {
      refreshFileTreeIfOpen();
      openFile(j.handler.file_path);
    }
    chatLog.scrollTop = chatLog.scrollHeight;
  };

  try {
    const r = await fetch('/api/chat', {
      method: 'POST', headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({message: text, cwd: effectiveCwd}),
      signal: controller.signal,
    });
    if (!r.ok || !r.body) {
      const j = await r.json().catch(()=>({error: 'request failed'}));
      headlinePre.textContent = 'error: ' + (j.error || r.status);
      return;
    }
    const reader  = r.body.getReader();
    const decoder = new TextDecoder();
    let buf = '';
    while (true) {
      const { value, done } = await reader.read();
      if (done) break;
      buf += decoder.decode(value, { stream: true });

      let sep;
      while ((sep = buf.indexOf('\n\n')) !== -1) {
        const eventText = buf.slice(0, sep);
        buf = buf.slice(sep + 2);
        let evtName = '';
        let dataStr = '';
        for (const line of eventText.split('\n')) {
          if (line.startsWith('event: ')) evtName = line.slice(7).trim();
          else if (line.startsWith('data: ')) {
            dataStr += (dataStr ? '\n' : '') + line.slice(6);
          }
        }
        if (!evtName) continue;
        let payload;
        try { payload = JSON.parse(dataStr); } catch { continue; }
        if (evtName === 'layer')          appendLayer(payload.name, payload.content);
        else if (evtName === 'heartbeat') {
          noteHeartbeat();
          // Parse the enriched heartbeat payload (role/tokens/max/loading)
          // and drive the progress bar + "loading X"/"thinking X" headline.
          applyHeartbeatProgress(curCtx, payload);
        }
        else if (evtName === 'final')     onFinal(payload);
        else if (evtName === 'stopped') {
          // Server confirmed the stop and erased the turn; the click
          // handler already cleaned up the log and restored the prompt.
          hbStop();
          removeStop();
          teardownWidgets();
        }
        else if (evtName === 'error') {
          hbStop();
          removeStop();
          teardownWidgets();
          headlinePre.textContent = 'error: ' + (payload.error || 'unknown');
        }
      }
    }
    hbStop();
    removeStop();
    teardownWidgets();
  } catch (err) {
    hbStop();
    removeStop();
    teardownWidgets();
    if (stoppedByUser) return null;   // aborted on purpose; nothing to report
    headlinePre.textContent = 'network error: ' + err.message;
    return null;
  }
  return capturedFinal;
}

// Light, safe markdown rendering for AI headlines. We escape ALL HTML first,
// then re-introduce the tiny subset we care about:
//   ```lang … ```         → Prism-highlighted code block
//   `code`                → inline code
//   [label](http(s)://…) → in-tool browser link + download icon
//   **bold**             → <strong>
// Anything else stays as escaped text so the Mouser links the components
// tool emits become clickable without exposing us to script injection.
function escHtml(s) {
  return String(s).replace(/[&<>"']/g, c => ({
    '&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'
  }[c]));
}

function formatChatProse(text) {
  let out = escHtml(text).replace(
    /\[([^\]\n]+)\]\((https?:\/\/[^\s)]+)\)/g,
    (_m, label, url) =>
      `<a href="#" data-url="${url}" class="chat-link" ` +
        `title="Open in AutoClank 9001 browser">${label}</a>` +
      ` <button class="chat-dl" data-url="${url}" type="button" ` +
        `title="Download to project /Downloads/">📥</button>`
  );
  out = out.replace(/`([^`\n]+)`/g, '<code class="inline-code">$1</code>');
  out = out.replace(/\*\*([^*\n][^*]*?)\*\*/g, '<strong>$1</strong>');
  return out;
}

// Normalize a fence-tag or extension to a Prism grammar id. Prism ships
// only a subset of aliases (js, py, ino, …) but users routinely fence
// with sh, rb, c++, ts, yml, jl, etc. Returns '' if no grammar matches.
function normalizePrismLang(raw) {
  if (!raw || typeof Prism === 'undefined') return '';
  const s = String(raw).toLowerCase().trim();
  const aliases = {
    'sh': 'bash', 'shell': 'bash', 'zsh': 'bash', 'ksh': 'bash',
    'rb': 'ruby', 'ru': 'ruby',
    'py3': 'python', 'python3': 'python', 'ipython': 'python',
    'c++': 'cpp', 'cxx': 'cpp', 'h': 'cpp', 'hpp': 'cpp', 'hxx': 'cpp',
    'cc': 'cpp',
    'jl': 'julia',
    'ts': 'typescript', 'mts': 'typescript', 'cts': 'typescript',
    'mjs': 'javascript', 'cjs': 'javascript',
    'yml': 'yaml',
    'md': 'markdown',
    'html': 'markup', 'htm': 'markup', 'xml': 'markup', 'svg': 'markup',
    'vue': 'markup',
    'dockerfile': 'docker',
    'makefile': 'makefile', 'gnumakefile': 'makefile',
    'cmakelists': 'cmake',
    'ps': 'powershell', 'ps1': 'powershell',
    'kts': 'kotlin', 'kt':  'kotlin',
    'cs':  'csharp', 'c#':  'csharp',
    'f#':  'fsharp',
    'proto': 'protobuf',
    'pl': 'perl',
    'tex': 'latex', 'sty': 'latex',
    'diff': 'diff', 'patch': 'diff',
    'gv': 'graphviz', 'dot': 'graphviz',
    'plaintext': '', 'text': '', 'txt': '', 'none': '',
  };
  const mapped = Object.prototype.hasOwnProperty.call(aliases, s) ? aliases[s] : s;
  return (mapped && Prism.languages[mapped]) ? mapped : '';
}

function formatChatMarkdown(text) {
  const s = String(text);
  const fence = /```([\w+#-]*)[ \t]*\n([\s\S]*?)```/g;
  let out = '', last = 0, m;
  while ((m = fence.exec(s)) !== null) {
    out += formatChatProse(s.slice(last, m.index));
    const lang = normalizePrismLang(m[1]);
    const cls = 'language-' + (lang || 'none');
    out += `<pre class="chat-code ${cls}"><code class="${cls}">` +
           escHtml(m[2]) + '</code></pre>';
    last = m.index + m[0].length;
  }
  out += formatChatProse(s.slice(last));
  return out;
}

// Run Prism over any fenced code blocks inside `root`. Called after
// innerHTML is set on live chat headlines and on session replay.
function highlightCodeIn(root) {
  if (typeof Prism === 'undefined' || !root) return;
  for (const el of root.querySelectorAll('pre > code[class*="language-"]')) {
    try { Prism.highlightElement(el); } catch { /* bad grammar: leave plain */ }
  }
}

function computeHeadline(j) {
  const h = j.handler || {};
  if (h.kind === 'shell') {
    const out = (h.stdout || '').trim();
    if (h.exit_code === 0) return out ? out : 'done';
    return (out ? out + '\n' : '') + '[exit ' + h.exit_code + ']';
  }
  // answer / physics_answer / chemistry_answer / future *_answer handlers
  if (h.kind && h.kind.endsWith('answer')) {
    return h.answer || '';
  }
  // image_gen / image_edit — the model isn't wired yet, so the server
  // returns a routed-to notice; render it plainly and mention the file
  // path if a placeholder image was written.
  if (h.kind === 'image_gen' || h.kind === 'image_edit') {
    let msg = h.answer || h.message || '(image routed)';
    if (h.file_path) msg += '\n\nfile: ' + h.file_path;
    return msg;
  }
  if (h.kind === 'statement' || h.kind === 'noted')   return h.message || '(noted)';
  return j.final || '(no handler)';
}

function pushMsg(role, body) {
  const el = document.createElement('div');
  el.className = `chat-msg ${role}`;
  el.innerHTML = `<div class="role"></div><div class="body"></div>`;
  el.querySelector('.role').textContent = role;
  el.querySelector('.body').textContent = body;
  chatLog.appendChild(el);
  chatLog.scrollTop = chatLog.scrollHeight;
  return el;
}

function renderAIResponse(msgEl, j) {
  const body = msgEl.querySelector('.body');
  body.innerHTML = '';

  // Headline: handler output, same rules as the live streaming path.
  const h = j.handler || {};
  const pre = document.createElement('pre');
  pre.className = 'ai-headline';
  pre.style.margin = '0';
  pre.style.whiteSpace = 'pre-wrap';
  pre.innerHTML = formatChatMarkdown(computeHeadline(j));
  highlightCodeIn(pre);
  body.appendChild(pre);

  // Tag line
  const tag = document.createElement('div');
  tag.className = 'tag-line';
  const act = j.act || {};
  const tags = (act.tags || []).join(',');
  tag.textContent =
    `[act=${act.act||'?'}${act.subtype?' subtype='+act.subtype:''}${tags?' tags='+tags:''}] ` +
    `[${j.expertise || '?'}]`;
  body.appendChild(tag);

  // Collapsible "thinking" chain. Includes the shell command if applicable
  // so the user can see what the model actually ran.
  const allLayers = (j.layers || []).slice();
  if (h.kind === 'shell' && h.command && !allLayers.some(l => l.name === 'shell')) {
    allLayers.push({name: 'shell command', content: h.command});
  }
  // Re-materialize the task checklist for replayed plan turns, outside
  // the chain, same as the live view.
  const planLayer = allLayers.find(l => l.name === 'tasks');
  if (planLayer) {
    const tl = makeTaskList(body, null);
    tl.plan(planLayer.content);
    for (const l of allLayers) {
      const m = (l.name || '').match(TASK_LAYER_RE);
      if (m) tl.complete(parseInt(m[1], 10) - 1, taskLayerOk(l.content));
    }
  }
  if (allLayers.length) {
    const det = document.createElement('details');
    det.className = 'chain';
    const sum = document.createElement('summary');
    sum.textContent = `thinking (${allLayers.length} layers)`;
    det.appendChild(sum);
    const wrap = document.createElement('div');
    wrap.className = 'layers';
    for (const l of allLayers) {
      const div = document.createElement('div');
      div.className = 'layer';
      div.innerHTML = `<div class="lab"></div><div class="payload"></div>`;
      div.querySelector('.lab').textContent = l.name;
      div.querySelector('.payload').textContent = l.content;
      wrap.appendChild(div);
    }
    det.appendChild(wrap);
    body.appendChild(det);
  }
  chatLog.scrollTop = chatLog.scrollHeight;
}

// ---- terminals: tabs + VS-Code-style single-pane (no separate input) --
const tabsList  = document.getElementById('term-tabs-list');
const termStack = document.getElementById('term-stack');
document.getElementById('term-new').addEventListener('click', () => newTerminal());

function newTerminal() {
  const id = 'T' + (state.nextTermId++);
  newTerminalAtCwd(id, state.rootDir || '~');
  activateTerminal(id);
  saveState();
}

function activateTerminal(id) {
  state.activeTerm = id;
  for (const el of tabsList.children) el.classList.toggle('active', el.dataset.id === id);
  for (const el of termStack.children) el.classList.toggle('active', el.dataset.id === id);
  const scr = termStack.querySelector(`.term-instance[data-id="${id}"] .term-screen`);
  if (scr) { scr.focus(); scrollToBottom(scr); }
}

function closeTerminal(id) {
  const t = state.terminals[id];
  if (t) {
    try { t.streamAbort?.abort(); } catch {}
    try { t.term?.dispose(); } catch {}
    try { t.resizeObserver?.disconnect(); } catch {}
    if (t.tid) {
      fetch('/api/terminal/close', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ tid: t.tid }),
      }).catch(() => {});
    }
  }
  delete state.terminals[id];
  tabsList.querySelector(`.term-tab[data-id="${id}"]`)?.remove();
  termStack.querySelector(`.term-instance[data-id="${id}"]`)?.remove();
  if (state.activeTerm === id) {
    const next = Object.keys(state.terminals)[0];
    if (next) activateTerminal(next);
    else state.activeTerm = null;
  }
}

function scrollToBottom(el) {
  el.scrollTop = el.scrollHeight;
}

// Render the active-input "prompt line" at the bottom of a screen.
function renderPromptLine(scr, cwd, current) {
  let promptLine = scr.querySelector('.term-prompt-line');
  if (!promptLine) {
    promptLine = document.createElement('div');
    promptLine.className = 'term-prompt-line';
    promptLine.innerHTML =
      `<span class="term-prompt"></span><span class="term-cmd"></span><span class="term-cursor"></span>`;
    scr.appendChild(promptLine);
  }
  promptLine.querySelector('.term-prompt').textContent = shortCwd(cwd) + '$ ';
  promptLine.querySelector('.term-cmd').textContent    = current;
  scrollToBottom(scr);
}

// Tab completion for the web terminal. Completes the current word via the
// server's compgen endpoint: fills the longest common prefix, or lists the
// candidates (like bash) when the prefix can't be extended.
function longestCommonPrefix(strs) {
  if (!strs.length) return '';
  let p = strs[0];
  for (const s of strs) {
    let i = 0;
    while (i < p.length && i < s.length && p[i] === s[i]) i++;
    p = p.slice(0, i);
    if (!p) break;
  }
  return p;
}

function printTerminalBlock(scr, text) {
  const promptLine = scr.querySelector('.term-prompt-line');
  const block = document.createElement('pre');
  block.className = 'term-out-block';
  block.textContent = text;
  scr.insertBefore(block, promptLine);
}

async function completeTerminalInput(id, scr) {
  const t = state.terminals[id];
  if (!t) return;
  const line = t.input;
  const token = (line.match(/(\S*)$/) || [''])[0];
  const before = line.slice(0, line.length - token.length);
  const first = before.trim() === '';
  let candidates = [];
  try {
    const r = await fetch('/api/terminal/complete', {
      method: 'POST', headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ cwd: t.cwd, token, first }),
    });
    if (r.ok) candidates = (await r.json()).candidates || [];
  } catch { return; }

  if (candidates.length === 0) return;                 // nothing to do
  if (candidates.length === 1) {
    const c = candidates[0];
    t.input = before + c + (c.endsWith('/') ? '' : ' '); // dirs stay open
  } else {
    const prefix = longestCommonPrefix(candidates);
    if (prefix.length > token.length) {
      t.input = before + prefix;                        // extend as far as shared
    } else {
      // Can't extend: show the options, keep the line (bash behaviour).
      const shown = candidates.slice(0, 200)
        .map(c => c.split('/').filter(Boolean).pop() + (c.endsWith('/') ? '/' : ''))
        .join('   ');
      printTerminalBlock(scr, shown +
        (candidates.length > 200 ? `\n… ${candidates.length - 200} more` : ''));
    }
  }
  renderPromptLine(scr, t.cwd, t.input);
}

// Approximate the terminal's width in monospace columns, so PTY-side tools
// like `ls` lay out their columns to the visible area.
let _charW = 0;
function termCols(scr) {
  if (!_charW) {
    const probe = document.createElement('span');
    probe.textContent = '0'.repeat(100);
    probe.style.cssText = 'position:absolute;visibility:hidden;white-space:pre;font:inherit';
    scr.appendChild(probe);
    _charW = probe.getBoundingClientRect().width / 100 || 8;
    probe.remove();
  }
  const w = scr.clientWidth || 800;
  return Math.max(20, Math.min(400, Math.floor((w - 12) / _charW)));
}

// The 16 ANSI colours, tuned to Ubuntu's default terminal palette.
const ANSI_COLORS = [
  '#2e3436', '#cc0000', '#4e9a06', '#c4a000', '#3465a4', '#75507b', '#06989a', '#d3d7cf',
  '#555753', '#ef2929', '#8ae234', '#fce94f', '#729fcf', '#ad7fa8', '#34e2e2', '#eeeeec',
];
// xterm 256-colour cube + greyscale, for 38;5;N / 48;5;N codes.
function xterm256(n) {
  if (n < 16) return ANSI_COLORS[n];
  if (n < 232) {
    n -= 16;
    const l = [0, 95, 135, 175, 215, 255];
    return `rgb(${l[Math.floor(n / 36) % 6]},${l[Math.floor(n / 6) % 6]},${l[n % 6]})`;
  }
  const v = 8 + (n - 232) * 10;
  return `rgb(${v},${v},${v})`;
}

function escHtml(s) {
  return s.replace(/[&<>"]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));
}

// Render text containing ANSI SGR escape sequences as safe coloured HTML.
// All literal text is HTML-escaped; only SGR (colour/style) codes are
// interpreted, other escape sequences are dropped.
function ansiToHtml(input) {
  let fg = null, bg = null, bold = false, italic = false, underline = false;
  let out = '', open = false;
  const closeSpan = () => { if (open) { out += '</span>'; open = false; } };
  const openSpan = () => {
    closeSpan();
    let style = '';
    let f = fg;
    if (bold && f !== null && f < 8) f += 8;         // bold brightens basic colours
    if (f !== null) style += `color:${typeof f === 'string' ? f : xterm256(f)};`;
    if (bg !== null) style += `background:${typeof bg === 'string' ? bg : xterm256(bg)};`;
    if (bold) style += 'font-weight:bold;';
    if (italic) style += 'font-style:italic;';
    if (underline) style += 'text-decoration:underline;';
    if (style) { out += `<span style="${style}">`; open = true; }
  };
  const applySGR = (codes) => {
    for (let i = 0; i < codes.length; i++) {
      const c = codes[i];
      if (c === 0) { fg = bg = null; bold = italic = underline = false; }
      else if (c === 1) bold = true;
      else if (c === 3) italic = true;
      else if (c === 4) underline = true;
      else if (c === 22) bold = false;
      else if (c === 23) italic = false;
      else if (c === 24) underline = false;
      else if (c >= 30 && c <= 37) fg = c - 30;
      else if (c === 39) fg = null;
      else if (c >= 90 && c <= 97) fg = c - 90 + 8;
      else if (c >= 40 && c <= 47) bg = c - 40;
      else if (c === 49) bg = null;
      else if (c >= 100 && c <= 107) bg = c - 100 + 8;
      else if (c === 38 || c === 48) {
        const mode = codes[i + 1];
        if (mode === 5) { const v = codes[i + 2]; (c === 38 ? (fg = v) : (bg = v)); i += 2; }
        else if (mode === 2) { const col = `rgb(${codes[i+2]||0},${codes[i+3]||0},${codes[i+4]||0})`; (c === 38 ? (fg = col) : (bg = col)); i += 4; }
      }
    }
    openSpan();
  };

  // Escape shapes we consume:
  //   \x1B[<params>m                    SGR (colors / styles)
  //   \x1B]<any>(BEL | ST)              OSC (window title, hyperlink, ...)
  //   \x1B[<>?]<params>[final]          CSI other (cursor move, DEC modes, alt screen)
  //   \x1B[()*+][A-Za-z0-9]             ISO 2022 charset designation
  //                                     (this is the `(B` that leaks otherwise)
  //   \x1B[=>7-9DEHMc]                  DEC keypad / save cursor / reset
  const re = /\x1b\[([0-9;]*)m|\x1b\][^\x07\x1b]*(?:\x07|\x1b\\)|\x1b[\[\]][0-9;?]*[A-Za-z]|\x1b[()*+][A-Za-z0-9]|\x1b[=>7-9DEHMc]/g;
  let last = 0, m;
  while ((m = re.exec(input)) !== null) {
    if (m.index > last) out += escHtml(input.slice(last, m.index));
    last = re.lastIndex;
    if (m[1] !== undefined) {                        // SGR colour/style
      const codes = m[1] === '' ? [0] : m[1].split(';').map(Number);
      applySGR(codes);
    }
    // other matched escape sequences are consumed and discarded
  }
  if (last < input.length) out += escHtml(input.slice(last));
  closeSpan();
  return out;
}

function shortCwd(cwd) {
  if (!cwd) return '~';
  const home = '/home/';
  if (cwd === '~') return '~';
  // Heuristic: replace /home/<user>/ with ~/
  const m = cwd.match(/^\/home\/[^/]+(.*)$/);
  if (m) return '~' + m[1];
  return cwd;
}

async function submitTerminalCommand(id, raw) {
  const t = state.terminals[id];
  if (!t) return;
  const scr = termStack.querySelector(`.term-instance[data-id="${id}"] .term-screen`);
  const promptLine = scr.querySelector('.term-prompt-line');

  // Freeze the prompt line as part of transcript.
  const frozen = document.createElement('div');
  frozen.className = 'term-entry';
  frozen.innerHTML = `<div class="frozen-line"><span class="term-prompt"></span><span class="term-cmd"></span></div>` +
                     `<pre class="term-out-block"></pre><div class="term-exit"></div>`;
  frozen.querySelector('.term-prompt').textContent = shortCwd(t.cwd) + '$ ';
  frozen.querySelector('.term-cmd').textContent    = raw;
  scr.insertBefore(frozen, promptLine);

  // Empty command: just new prompt line.
  if (!raw.trim()) {
    t.input = '';
    renderPromptLine(scr, t.cwd, '');
    return;
  }

  // Client-side `clear` so we don't emit raw ANSI escape codes.
  if (raw.trim() === 'clear') {
    frozen.remove();           // don't show the issued `clear`
    scr.innerHTML = '';
    t.input = '';              // ← critical: reset input buffer
    renderPromptLine(scr, t.cwd, '');
    return;
  }

  // Handle cd client-side so cwd survives.
  const cdMatch = raw.trim().match(/^cd(?:\s+(.+))?$/);
  if (cdMatch) {
    const dest = (cdMatch[1] || '~').trim();
    try {
      const r = await fetch('/api/terminal/exec', {
        method:'POST', headers:{'Content-Type':'application/json'},
        body: JSON.stringify({command: `cd ${dest} && pwd`, cwd: t.cwd}),
      });
      const j = await r.json();
      if (j.exit_code === 0) {
        t.cwd = (j.stdout || '').trim();
      } else {
        frozen.querySelector('.term-out-block').textContent = (j.stdout || '').trim();
        const ex = frozen.querySelector('.term-exit');
        ex.textContent = '[exit ' + j.exit_code + ']';
        ex.classList.add('fail');
      }
    } catch (err) {
      frozen.querySelector('.term-out-block').textContent = 'network error: ' + err.message;
    }
    t.input = '';
    renderPromptLine(scr, t.cwd, '');
    saveState();
    return;
  }

  // Normal exec: stream SSE so a long-running program (./quantiprize,
  // `watch nvidia-smi`, tail -f) flows output live instead of buffering
  // for 45 seconds and looking dead. AbortController lets the tab close
  // (or a future stop button) tear the fetch, at which point the server
  // notices its next sink.write fail and SIGKILLs the child.
  const controller = new AbortController();
  t.abort = controller;
  t.exBlockLive = frozen.querySelector('.term-exit');
  t.frozenLive = frozen;
  const outBlock = frozen.querySelector('.term-out-block');
  const exBlock  = t.exBlockLive;
  const appendChunk = (raw) => {
    // Fold \r\n → \n like the buffered path did; leave other CRs to
    // ansiToHtml (some programs use CR to redraw a line).
    outBlock.innerHTML += ansiToHtml(raw.replace(/\r\n/g, '\n'));
    scr.scrollTop = scr.scrollHeight;
  };
  // We only render a new prompt when the server explicitly reports the
  // child exited (or the user aborts). If the stream closes for any
  // other reason (browser tab background throttling, network hiccup),
  // we assume the child is still running and leave the terminal locked:
  // a returning prompt while `watch` is still going in the kernel is
  // worse than a stuck stream the user can fix by reopening the tab.
  let sawExit  = false;
  let userAbort = false;
  try {
    const r = await fetch('/api/terminal/exec_stream', {
      method:'POST', headers:{'Content-Type':'application/json'},
      body: JSON.stringify({command: raw, cwd: t.cwd, cols: termCols(scr), rows: 40}),
      signal: controller.signal,
    });
    if (!r.ok || !r.body) {
      outBlock.textContent = 'terminal error: HTTP ' + r.status;
      sawExit = true;                      // give up, show a prompt again
    } else {
      const reader  = r.body.getReader();
      const decoder = new TextDecoder('utf-8');
      let buf = '';
      while (true) {
        const { value, done } = await reader.read();
        if (done) break;
        buf += decoder.decode(value, { stream: true });
        // SSE frames end at \n\n. Split, parse each, keep the tail.
        let sep;
        while ((sep = buf.indexOf('\n\n')) >= 0) {
          const frame = buf.slice(0, sep);
          buf = buf.slice(sep + 2);
          let evt = 'message', payload = '';
          for (const line of frame.split('\n')) {
            if (line.startsWith('event: ')) evt = line.slice(7).trim();
            else if (line.startsWith('data: ')) payload += line.slice(6);
          }
          if (!payload) continue;
          let j;
          try { j = JSON.parse(payload); } catch { continue; }
          if (evt === 'chunk' && typeof j.data === 'string') {
            try { appendChunk(j.data); } catch (e) {
              console.error('term appendChunk failed', e);
            }
          } else if (evt === 'exit') {
            sawExit = true;
            if (j.code !== 0) {
              exBlock.textContent = '[exit ' + j.code + ']';
              exBlock.classList.add('fail');
            }
          } else if (evt === 'error') {
            outBlock.innerHTML += '\n[terminal error: ' +
              String(j.error || '').replace(/[<>&]/g, '') + ']';
          }
        }
      }
    }
  } catch (err) {
    if (err && err.name === 'AbortError') {
      userAbort = true;
      exBlock.textContent = '[aborted]';
      exBlock.classList.add('fail');
    } else {
      // Network error is treated as "server gave up" so the user isn't
      // stuck; log to console so we can see what actually happened.
      console.error('term stream error', err);
      outBlock.innerHTML += '\n[stream ended: ' +
        String((err && err.message) || err).replace(/[<>&]/g, '') + ']';
      sawExit = true;
    }
  } finally {
    t.abort = null;
  }
  if (sawExit || userAbort) {
    t.input = '';
    renderPromptLine(scr, t.cwd, '');
    saveState();
  } else {
    // The read loop ended but the server never sent `event: exit`.
    // Some browsers close a background-tab SSE stream after a while; the
    // command is likely still running in the kernel. Show a note and
    // add a click-to-recover row so the user is not stranded.
    const note = document.createElement('div');
    note.className = 'term-exit';
    note.textContent = '[stream closed; command may still be running server-side. click to resume prompt]';
    note.style.cursor = 'pointer';
    note.addEventListener('click', () => {
      note.remove();
      t.input = '';
      renderPromptLine(scr, t.cwd, '');
      saveState();
    });
    frozen.appendChild(note);
  }
}

// Real terminal per tab, backed by xterm.js and a persistent server-side
// PTY. bash prints its own prompt inside the emulator; every keystroke
// (Ctrl-C, arrows, tab completion) rides through to the master. No more
// client-side line editing.
function newTerminalAtCwd(id, cwd) {
  state.terminals[id] = { cwd, tid: null, term: null };

  const tab = document.createElement('div');
  tab.className = 'term-tab';
  tab.dataset.id = id;
  tab.innerHTML = `<span class="label"></span><span class="x" title="Close">×</span>`;
  tab.querySelector('.label').textContent = id;
  tab.addEventListener('click', e => {
    if (e.target.classList.contains('x')) return;
    activateTerminal(id);
  });
  tab.querySelector('.x').addEventListener('click', e => {
    e.stopPropagation();
    closeTerminal(id);
    saveState();
  });
  tabsList.appendChild(tab);

  const body = document.createElement('div');
  body.className = 'term-instance';
  body.dataset.id = id;
  body.innerHTML = `<div class="term-xterm-host"></div>`;
  termStack.appendChild(body);
  const host = body.querySelector('.term-xterm-host');

  if (typeof Terminal !== 'function') {
    host.textContent = 'xterm.js failed to load';
    return;
  }

  const term = new Terminal({
    fontFamily: '"Cascadia Mono", "JetBrains Mono", ui-monospace, Menlo, Consolas, monospace',
    fontSize: 13,
    lineHeight: 1.2,
    cursorBlink: true,
    scrollback: 5000,
    theme: {
      background:  '#1e1e1e',
      foreground:  '#d4d4d4',
      cursor:      '#d4d4d4',
      selectionBackground: '#264f78',
      black:   '#000000', red:     '#cd3131', green:   '#0dbc79',
      yellow:  '#e5e510', blue:    '#2472c8', magenta: '#bc3fbc',
      cyan:    '#11a8cd', white:   '#e5e5e5',
      brightBlack:   '#666666', brightRed:     '#f14c4c',
      brightGreen:   '#23d18b', brightYellow:  '#f5f543',
      brightBlue:    '#3b8eea', brightMagenta: '#d670d6',
      brightCyan:    '#29b8db', brightWhite:   '#e5e5e5',
    },
  });
  const fit = (typeof FitAddon === 'object' && FitAddon.FitAddon)
    ? new FitAddon.FitAddon()
    : (typeof FitAddon === 'function' ? new FitAddon() : null);
  if (fit) { try { term.loadAddon(fit); } catch {} }
  term.open(host);
  try { fit && fit.fit(); } catch {}

  // Real-terminal key handling: Ctrl+C is ALWAYS SIGINT (like a real
  // xterm/gnome-terminal in default config), never "copy". Ctrl+Shift+C
  // is chrome-hijacked so it isn't available; use Ctrl+Insert to copy or
  // just select text (auto-copies on mouseup). Ctrl+V pastes; Ctrl+Shift+V
  // also pastes for muscle memory. Everything else falls through to
  // xterm's default handler.
  term.attachCustomKeyEventHandler((e) => {
    if (e.type !== 'keydown') return true;
    const key = e.key;
    // Ctrl+C -> SIGINT unconditionally. If there was a selection, clear
    // it so the default handler's copy-branch never fires; xterm will
    // then send \x03 through onData like a real terminal.
    if (e.ctrlKey && !e.shiftKey && !e.altKey && !e.metaKey &&
        (key === 'c' || key === 'C')) {
      try { term.clearSelection(); } catch {}
      // Send SIGINT explicitly so it happens even if the default handler
      // wants to route it elsewhere.
      fetch('/api/terminal/write', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ tid: t.tid, data: '\x03' }),
      }).catch(() => {});
      e.preventDefault();
      return false;
    }
    // Ctrl+Insert -> copy (browser-safe, unix-standard).
    if (e.ctrlKey && !e.shiftKey && !e.altKey && key === 'Insert') {
      const sel = term.getSelection();
      if (sel) navigator.clipboard.writeText(sel).catch(() => {});
      e.preventDefault();
      return false;
    }
    // Shift+Insert or Ctrl+Shift+V -> paste.
    if ((e.shiftKey && key === 'Insert') ||
        (e.ctrlKey && e.shiftKey && (key === 'v' || key === 'V'))) {
      (async () => {
        try {
          const txt = await navigator.clipboard.readText();
          if (txt) fetch('/api/terminal/write', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ tid: t.tid, data: txt }),
          }).catch(() => {});
        } catch {}
      })();
      e.preventDefault();
      return false;
    }
    return true;
  });

  // Auto-copy on selection release (gnome-terminal / tmux behaviour) so
  // "highlight and paste-with-Ctrl+V-elsewhere" works without any hotkey.
  host.addEventListener('mouseup', () => {
    try {
      const sel = term.getSelection();
      if (sel) navigator.clipboard.writeText(sel).catch(() => {});
    } catch {}
  });

  const t = state.terminals[id];
  t.term  = term;
  t.fit   = fit;
  t.host  = host;

  (async () => {
    // Ask the server for a persistent PTY.
    let cols = term.cols || 80, rows = term.rows || 40;
    let resp;
    try {
      const r = await fetch('/api/terminal/open', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ cwd, cols, rows }),
      });
      resp = await r.json();
    } catch (e) {
      term.write('\r\n[terminal open failed]\r\n');
      return;
    }
    if (!resp || !resp.tid) {
      term.write('\r\n[terminal open failed: ' + (resp && resp.error || 'unknown') + ']\r\n');
      return;
    }
    t.tid = resp.tid;

    // Keystrokes → server. onData delivers already-encoded bytes
    // (arrow keys are already CSI sequences, Ctrl-C is \x03, etc.).
    term.onData(data => {
      if (!t.tid) return;
      fetch('/api/terminal/write', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ tid: t.tid, data }),
      }).catch(() => {});
    });
    // Terminal size changes → server (TIOCSWINSZ + SIGWINCH).
    term.onResize(({ cols, rows }) => {
      if (!t.tid) return;
      fetch('/api/terminal/resize', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ tid: t.tid, cols, rows }),
      }).catch(() => {});
    });
    // ResizeObserver refits xterm when the terminal pane resizes.
    if (fit && 'ResizeObserver' in window) {
      const ro = new ResizeObserver(() => {
        try { fit.fit(); } catch {}
      });
      ro.observe(host);
      t.resizeObserver = ro;
    }

    // Subscribe to output. Reconnect on close (with a small backoff).
    let backoff = 500;
    const subscribe = async () => {
      const ac = new AbortController();
      t.streamAbort = ac;
      let r;
      try {
        r = await fetch('/api/terminal/stream?tid=' + encodeURIComponent(t.tid), {
          signal: ac.signal,
        });
      } catch (e) {
        if (e && e.name === 'AbortError') return;
        setTimeout(subscribe, backoff);
        backoff = Math.min(backoff * 2, 5000);
        return;
      }
      if (!r.ok || !r.body) {
        term.write('\r\n[stream error: HTTP ' + r.status + ']\r\n');
        return;
      }
      backoff = 500;
      const reader = r.body.getReader();
      const decoder = new TextDecoder('utf-8');
      let buf = '';
      let ptyClosed = false;
      while (true) {
        const { value, done } = await reader.read();
        if (done) break;
        buf += decoder.decode(value, { stream: true });
        let sep;
        while ((sep = buf.indexOf('\n\n')) >= 0) {
          const frame = buf.slice(0, sep);
          buf = buf.slice(sep + 2);
          let evt = 'message', payload = '';
          for (const line of frame.split('\n')) {
            if (line.startsWith('event: ')) evt = line.slice(7).trim();
            else if (line.startsWith('data: ')) payload += line.slice(6);
          }
          if (!payload) continue;
          let j;
          try { j = JSON.parse(payload); } catch { continue; }
          if (evt === 'data' && typeof j.data === 'string') {
            term.write(j.data);
          } else if (evt === 'closed') {
            ptyClosed = true;
          }
        }
      }
      if (ac.signal.aborted) return;
      if (ptyClosed) {
        // bash exited (user typed `exit` or Ctrl-D): close the tab
        // like a real terminal emulator would.
        closeTerminal(id);
        saveState();
        return;
      }
      // Server closed the stream but the PTY may still be alive;
      // reconnect and pick up any output we missed.
      setTimeout(subscribe, backoff);
      backoff = Math.min(backoff * 2, 5000);
    };
    subscribe();
  })();
}

// ---- state persistence (per-session) ----------------------------------
// Two-tier persistence: sessionStorage is a fast-paint cache (per tab),
// the server is authoritative (survives close-browser, restart-machine,
// and is the source the picker reads). Session id is in the URL fragment.
const STATE_KEY_BASE = 'tool_state_v1';
const stateKey = () =>
  currentSessionId ? STATE_KEY_BASE + ':' + currentSessionId : STATE_KEY_BASE;

let serverSaveTimer = null;
function scheduleServerSave() {
  if (serverSaveTimer) clearTimeout(serverSaveTimer);
  serverSaveTimer = setTimeout(syncStateToServer, 500);
}
async function syncStateToServer() {
  if (!currentSessionId) return;
  const blob = JSON.parse(sessionStorage.getItem(stateKey()) || '{}');
  try {
    await fetch('/api/sessions/' + currentSessionId, {
      method: 'PUT',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ ui: blob }),
    });
  } catch {}
}

function saveState() {
  const root = document.documentElement;
  const s = {
    rootDir: state.rootDir,
    openFiles: Object.keys(state.files || {}),
    activeFilePath: state.activeFilePath,
    panes: {
      filesHidden: document.getElementById('pane-files').classList.contains('collapsed'),
      chatHidden:  document.getElementById('pane-chat').classList.contains('collapsed'),
      termHidden:  document.getElementById('terminal-bar').classList.contains('collapsed'),
      filesW: getComputedStyle(root).getPropertyValue('--files-w').trim(),
      chatW:  getComputedStyle(root).getPropertyValue('--chat-w').trim(),
      termH:  getComputedStyle(root).getPropertyValue('--term-h').trim(),
    },
    terminals:  Object.entries(state.terminals).map(([id, t]) => ({id, cwd: t.cwd})),
    activeTerm: state.activeTerm,
    nextTermId: state.nextTermId,
  };
  try { sessionStorage.setItem(stateKey(), JSON.stringify(s)); } catch {}
  scheduleServerSave();
}

// Replay one server-stored chat row (role + text) into the chat log
// — used by the boot path so refresh restores the visible conversation.
function replayChatMessage(role, text) {
  if (!text) return;
  pushMsg(role === 'user' ? 'user' : 'ai', text);
}

async function restoreState() {
  // 1. Fast paint from sessionStorage cache (if any).
  let s = null;
  try { s = JSON.parse(sessionStorage.getItem(stateKey()) || 'null'); } catch {}

  // 2. Pull authoritative state from the server.
  let serverPayload = null;
  if (currentSessionId) {
    try {
      const r = await fetch('/api/sessions/' + currentSessionId);
      if (r.ok) serverPayload = await r.json();
    } catch {}
  }
  if (serverPayload && serverPayload.ui && Object.keys(serverPayload.ui).length) {
    s = serverPayload.ui;
    try { sessionStorage.setItem(stateKey(), JSON.stringify(s)); } catch {}
  }

  // 3. Render the chat log from the server (so refresh shows the convo).
  // AI entries arrive as structured turns (layers + handler) and go
  // through the same renderer the live SSE path uses, so a reload shows
  // the headline + thinking chain, not raw text.
  if (serverPayload && Array.isArray(serverPayload.chat)) {
    for (const m of serverPayload.chat) {
      if (m.role !== 'ai') { replayChatMessage(m.role, m.text); continue; }
      if ((m.layers && m.layers.length) || (m.handler && m.handler.kind)) {
        renderAIResponse(pushMsg('ai', ''), m, {replay: true});
      } else {
        replayChatMessage(m.role, m.text);
      }
    }
  }

  if (!s) {
    // Fresh session: open one terminal so the bar isn't empty.
    newTerminal();
    return;
  }

  // pane sizes / collapse state
  if (s.panes) {
    if (s.panes.filesW) document.documentElement.style.setProperty('--files-w', s.panes.filesW);
    if (s.panes.chatW)  document.documentElement.style.setProperty('--chat-w',  s.panes.chatW);
    if (s.panes.termH)  document.documentElement.style.setProperty('--term-h',  s.panes.termH);
    for (const [id, hide, key] of [
      ['pane-files',   s.panes.filesHidden, 'pane-files'],
      ['pane-chat',    s.panes.chatHidden,  'pane-chat'],
      // Do NOT restore termHidden: always show the terminal on load so the
      // user can't end up with a perpetually-hidden terminal they forgot
      // about. (They can still toggle it off this session via the ⌨ icon.)
    ]) {
      if (!hide) continue;
      const target = document.getElementById(id);
      const btn = document.querySelector(`.pane-toggle[data-toggle="${key}"]`);
      if (target) target.classList.add('collapsed');
      if (btn) btn.classList.remove('active');
      if (id === 'pane-files')
        document.querySelector('.resizer[data-target="pane-files"]')?.classList.add('collapsed');
      if (id === 'pane-chat')
        document.querySelector('.resizer[data-target="pane-chat"]')?.classList.add('collapsed');
      if (id === 'terminal-bar') {
        document.getElementById('resizer-terminal')?.classList.add('collapsed');
        document.body.style.gridTemplateRows = '28px 1fr 0 0';
      }
    }
    updateLayoutColumns();
    if (!s.panes.termHidden && s.panes.termH) {
      document.body.style.gridTemplateRows = `28px 1fr 4px ${s.panes.termH}`;
    }
  }

  // restore terminals BEFORE file/folder so cwd is consistent
  if (Array.isArray(s.terminals) && s.terminals.length) {
    state.nextTermId = s.nextTermId || 1;
    for (const t of s.terminals) {
      newTerminalAtCwd(t.id, t.cwd);
    }
    activateTerminal(s.activeTerm || s.terminals[0].id);
  } else {
    newTerminal();
  }

  // root folder + open files (multi-tab)
  if (s.rootDir) {
    commitOpenFolder(s.rootDir);
    if (Array.isArray(s.openFiles)) {
      for (const path of s.openFiles) {
        if (path.startsWith('__browser:')) {
          openBrowserTab(path.slice('__browser:'.length));
        } else {
          await openFile(path);
        }
      }
      if (s.activeFilePath && state.files[s.activeFilePath]) {
        activateFile(s.activeFilePath);
      }
    }
  }

  // Reattach to an in-flight chat turn if this session has one running
  // server-side. Handles the "reload the page mid-turn" case: the
  // original POST /api/chat fetch was killed by the browser but the
  // pipeline is still running (and, for long image_gen / image_edit
  // subprocesses, may still have minutes to go). This rebuilds the AI
  // bubble widgets and subscribes to /api/chat/events for the replay
  // ring plus every future frame.
  reattachInFlightChat().catch(err =>
    console.warn('reattach chat: ' + (err && err.message)));
}

// Reattach to a chat turn already in flight for the current session.
// See restoreState() above for context. No-ops if no turn is running.
async function reattachInFlightChat() {
  if (!currentSessionId) return;
  let running = false;
  try {
    const r = await fetch('/api/chat/status?sid=' + encodeURIComponent(currentSessionId));
    if (r.ok) {
      const j = await r.json();
      running = !!j.running;
    }
  } catch { return; }
  if (!running) return;
  await loadModelsMap();

  // Build the same widget set as _runChatTurnImpl. The user prompt row
  // was already re-rendered from the session store by restoreState; we
  // just need the AI reply bubble.
  const aiEl = pushMsg('ai', '');
  const body = aiEl.querySelector('.body');
  body.innerHTML = '';
  const gpuStrip = startAiGpuStrip(body);
  const progWrap = document.createElement('div');
  progWrap.className = 'ai-progbar';
  const progFill = document.createElement('div');
  progFill.className = 'ai-progbar-fill';
  progWrap.appendChild(progFill);
  body.appendChild(progWrap);
  const headRow = document.createElement('div');
  headRow.className = 'ai-headrow';
  const headlinePre = document.createElement('pre');
  headlinePre.className = 'ai-headline';
  headlinePre.textContent = 'reattaching…';
  const clockEl = document.createElement('div');
  clockEl.className = 'ai-clock';
  clockEl.textContent = '+0:00';
  headRow.appendChild(headlinePre);
  headRow.appendChild(clockEl);
  body.appendChild(headRow);
  const chain = document.createElement('details');
  chain.className = 'chain';
  chain.innerHTML = '<summary></summary><div class="layers"></div>';
  const summary  = chain.querySelector('summary');
  const layersEl = chain.querySelector('.layers');
  const summaryText = document.createElement('span');
  summaryText.textContent = 'reattached… (0 layers)';
  summary.appendChild(summaryText);
  body.appendChild(chain);

  // t0 is set to the reattach moment — we can't reconstruct the original
  // turn's start time, so the clock counts up from now with a leading
  // '+' to make it obvious this is elapsed-since-reload, not total.
  const t0 = Date.now();
  const clockTimer = setInterval(() => {
    const s = Math.floor((Date.now() - t0) / 1000);
    clockEl.textContent = '+' + Math.floor(s / 60) + ':' +
      String(s % 60).padStart(2, '0');
  }, 250);
  const curCtx = {
    headlinePre, layersEl, summaryText, layerCount: 0,
    progFill, clockEl, clockTimer, t0, gpuStrip, activeRole: null,
  };

  let hbTimer = null;
  const hbStop = () => {
    if (hbTimer) { clearTimeout(hbTimer); hbTimer = null; }
    headlinePre.classList.remove('thinking-live');
    summary.classList.remove('thinking-live');
  };
  const noteHeartbeat = () => {
    headlinePre.classList.add('thinking-live');
    summary.classList.add('thinking-live');
    if (hbTimer) clearTimeout(hbTimer);
    hbTimer = setTimeout(hbStop, 12000);
  };
  noteHeartbeat();

  const teardown = () => {
    if (curCtx.clockTimer) {
      const s = Math.floor((Date.now() - curCtx.t0) / 1000);
      curCtx.clockEl.textContent = '+' + Math.floor(s / 60) + ':' +
        String(s % 60).padStart(2, '0');
      clearInterval(curCtx.clockTimer); curCtx.clockTimer = null;
    }
    if (curCtx.gpuStrip) { stopAiGpuStrip(curCtx.gpuStrip); curCtx.gpuStrip = null; }
    if (curCtx.progFill && curCtx.progFill.parentNode) {
      curCtx.progFill.parentNode.remove();
      curCtx.progFill = null;
    }
  };

  const appendLayer = (name, content) => {
    const div = document.createElement('div');
    div.className = 'layer';
    div.innerHTML = '<div class="lab"></div><div class="payload"></div>';
    div.querySelector('.lab').textContent = name;
    div.querySelector('.payload').textContent = content;
    layersEl.appendChild(div);
    ++curCtx.layerCount;
    const active = (g_modelsMap && g_modelsMap.active) || {};
    const role   = layerToRole(name, active);
    const shorts = (g_modelsMap && g_modelsMap.shorts) || {};
    const short  = role ? shorts[role] : null;
    const roleGpu = (window.__ac9RoleGpu || {})[role];
    const suffix  = (typeof roleGpu === 'number') ? ' on gpu ' + roleGpu : '';
    if (short) {
      headlinePre.textContent = 'thinking (' + short + suffix + ')';
      summaryText.textContent =
        `thinking (${short}${suffix}) — ${curCtx.layerCount} layers`;
    } else {
      summaryText.textContent =
        `reattached… (${curCtx.layerCount} layers)`;
    }
    noteHeartbeat();
    chatLog.scrollTop = chatLog.scrollHeight;
  };

  const onFinal = j => {
    hbStop();
    teardown();
    if (curCtx.activeRole && j && j.handler && j.handler.stdout) {
      const est = Math.max(50, j.handler.stdout.length >> 2);
      writeRoleAvg(curCtx.activeRole, est);
    }
    summaryText.textContent = `thinking (${curCtx.layerCount} layers)`;
    headlinePre.innerHTML = formatChatMarkdown(computeHeadline(j));
    highlightCodeIn(headlinePre);
    const tag = document.createElement('div');
    tag.className = 'layer';
    tag.innerHTML = '<div class="lab">tags</div><div class="payload"></div>';
    const act = j.act || {};
    const tags = (act.tags || []).join(',');
    tag.querySelector('.payload').textContent =
      `[act=${act.act || '?'}${act.subtype ? ' subtype=' + act.subtype : ''}` +
      `${tags ? ' tags=' + tags : ''}] [${j.expertise || '?'}]`;
    layersEl.appendChild(tag);
    if (j.handler && j.handler.kind === 'shell') refreshFileTreeIfOpen();
    if (j.handler && j.handler.kind === 'components_answer' &&
        j.handler.file_path) {
      refreshFileTreeIfOpen();
      openFile(j.handler.file_path);
    }
    chatLog.scrollTop = chatLog.scrollHeight;
  };

  const url = '/api/chat/events?sid=' + encodeURIComponent(currentSessionId) +
              '&since=0-0';
  try {
    const r = await fetch(url);
    if (!r.ok || !r.body) { teardown(); return; }
    const reader  = r.body.getReader();
    const decoder = new TextDecoder();
    let buf = '';
    while (true) {
      const { value, done } = await reader.read();
      if (done) break;
      buf += decoder.decode(value, { stream: true });
      let sep;
      while ((sep = buf.indexOf('\n\n')) !== -1) {
        const frame = buf.slice(0, sep);
        buf = buf.slice(sep + 2);
        let evt = '', dataStr = '';
        for (const line of frame.split('\n')) {
          if (line.startsWith('event: ')) evt = line.slice(7).trim();
          else if (line.startsWith('data: ')) {
            dataStr += (dataStr ? '\n' : '') + line.slice(6);
          }
        }
        if (!evt || evt === 'session') continue;
        let payload;
        try { payload = JSON.parse(dataStr); } catch { continue; }
        if (evt === 'layer')          appendLayer(payload.name, payload.content);
        else if (evt === 'heartbeat') {
          noteHeartbeat();
          applyHeartbeatProgress(curCtx, payload);
        }
        else if (evt === 'final')     onFinal(payload);
        else if (evt === 'stopped')   { hbStop(); teardown(); }
        else if (evt === 'error')     {
          hbStop(); teardown();
          headlinePre.textContent = 'error: ' + (payload.error || 'unknown');
        }
      }
    }
    teardown();
  } catch { teardown(); }
}

// ---- hooks: save state on relevant events -----------------------------
const origCommitOpenFolder = commitOpenFolder;
commitOpenFolder = function(p) { origCommitOpenFolder(p); saveState(); };
const origOpenFile = openFile;
openFile = async function(p) { await origOpenFile(p); saveState(); };
const origActivate = activateTerminal;
activateTerminal = function(id) { origActivate(id); saveState(); };
const origClose = closeTerminal;
closeTerminal = function(id) { origClose(id); saveState(); };
const origNew = newTerminal;
newTerminal = function() { origNew(); saveState(); };
// (the actual terminal-submit wrapper lives further down, near the file-tree
// refresh hook — that's where cd-driven cwd changes get a saveState.)
// Pane toggles
paneToggles.forEach(btn => btn.addEventListener('click', () => saveState()));
// Resize: debounce save
let resizeSaveT = null;
['mousemove','mouseup'].forEach(ev => document.addEventListener(ev, () => {
  if (resizeSaveT) clearTimeout(resizeSaveT);
  resizeSaveT = setTimeout(saveState, 250);
}));

// Warn on close if any open file has unsaved changes.
window.addEventListener('beforeunload', e => {
  for (const f of Object.values(state.files || {})) {
    if (f.dirty) { e.preventDefault(); e.returnValue = ''; return; }
  }
});

// ---- file tree auto-refresh after any shell/term action --------------
function refreshFileTreeIfOpen() { if (state.rootDir) refreshFileTree(); }

// Poll for external file changes. Recurses into every currently-expanded
// subfolder so changes inside open folders also trigger refresh. Preserves
// expansion state across the re-render so the user's open folders survive.
let __lastTreeHash = '';
async function snapshotTree(rootPath, expanded) {
  // Returns a hash string covering root + all expanded subfolders.
  const stack = [rootPath];
  const parts = [];
  while (stack.length) {
    const p = stack.shift();
    const d = await fsList(p);
    if (!d) continue;
    parts.push(p + ':' + d.entries.map(e => e.name + (e.is_dir ? '/' : '')).join(','));
    for (const e of d.entries) {
      if (e.is_dir && expanded.has(joinPath(p, e.name))) {
        stack.push(joinPath(p, e.name));
      }
    }
  }
  return parts.join('|');
}

function getExpandedPaths() {
  const s = new Set();
  filesTree.querySelectorAll('.fs-dir').forEach(node => {
    const kids = node.querySelector('.fs-children');
    if (kids && !kids.classList.contains('hidden')) s.add(node.dataset.path);
  });
  return s;
}

async function reExpand(expanded) {
  for (const path of expanded) {
    const node = filesTree.querySelector(`.fs-dir[data-path="${CSS.escape(path)}"]`);
    if (!node) continue;
    const kids = node.querySelector('.fs-children');
    if (!kids) continue;
    kids.classList.remove('hidden');
    if (kids.children.length === 0) {
      const sub = await fsList(path);
      if (sub) renderEntries(kids, sub.path, sub.entries);
    }
  }
}

async function pollFileTree() {
  if (!state.rootDir) return;
  const expanded = getExpandedPaths();
  const h = await snapshotTree(state.rootDir, expanded);
  if (h === __lastTreeHash) return;
  __lastTreeHash = h;
  const data = await fsList(state.rootDir);
  if (!data) return;
  filesTree.innerHTML = '';
  renderEntries(filesTree, data.path, data.entries);
  await reExpand(expanded);
}
setInterval(pollFileTree, 1500);

// Poll each currently-open editor tab for external file changes and reload
// its content when the on-disk mtime advances past what the tab was showing.
// Clean tabs reload silently (VSCode behaviour); dirty tabs get a one-line
// warning bar with Reload / Keep buttons so the user's unsaved edits are
// never blown away. Binary tabs (images, PDFs) opt out; their content is
// served by /api/fs/raw and refreshed on re-open.
async function pollOpenFiles() {
  const paths = [];
  for (const [p, f] of Object.entries(state.files || {})) {
    if (!f) continue;
    if (f.mode === 'pdf' || f.mode === 'image' || f.mode === 'browser') continue;
    if (f.mode === 'tickets') continue;   // has its own board-shape refresh loop
    if (typeof f.setContent !== 'function') continue;
    paths.push(p);
  }
  if (paths.length === 0) return;
  let scan;
  try {
    const r = await fetch('/api/fs/scan', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ paths }),
    });
    if (!r.ok) return;
    scan = await r.json();
  } catch { return; }
  if (!scan || !Array.isArray(scan.entries)) return;
  for (const ent of scan.entries) {
    const f = state.files[ent.path];
    if (!f) continue;
    if (!ent.exists) continue;                 // deleted-externally: leave the buffer alone
    if (!f.mtimeNs || f.mtimeNs === '0' || f.mtimeNs === ent.mtime_ns) continue;
    // Something touched the file since the tab loaded / last saved. Fetch
    // the fresh bytes and either splice them in (clean) or offer the user
    // the choice (dirty).
    let fresh;
    try {
      const rr = await fetch('/api/fs/read?path=' + encodeURIComponent(ent.path));
      if (!rr.ok) continue;
      fresh = await rr.json();
    } catch { continue; }
    if (typeof fresh.content !== 'string') continue;
    if (f.dirty) {
      showExternalChangeBar(ent.path, fresh.content, fresh.mtime_ns);
    } else {
      f.setContent(fresh.content);
      f.savedContent = fresh.content;
      if (typeof fresh.mtime_ns === 'string') f.mtimeNs = fresh.mtime_ns;
      setFileDirty(ent.path, false);
    }
  }
}
setInterval(pollOpenFiles, 1500);

// Non-modal warning bar shown at the top of a dirty file's editor surface
// when the on-disk copy has moved past its buffer. Reload adopts disk
// content and drops in-editor edits; Keep marks the current buffer as the
// authoritative version (until the user saves or closes) and stops the
// poller from re-prompting for this same disk revision.
function showExternalChangeBar(path, diskContent, diskMtimeNs) {
  const f = state.files[path];
  if (!f || !f.surface) return;
  let bar = f.surface.querySelector('.editor-extern-bar');
  if (bar) return;                              // already prompting
  bar = document.createElement('div');
  bar.className = 'editor-extern-bar';
  bar.innerHTML =
    `<span>${escapeHTML(path.split('/').pop())} changed on disk. ` +
    `You have unsaved edits.</span>` +
    `<button class="ext-reload">Reload from disk</button>` +
    `<button class="ext-keep">Keep my edits</button>`;
  f.surface.insertBefore(bar, f.surface.firstChild);
  bar.querySelector('.ext-reload').addEventListener('click', () => {
    if (typeof f.setContent === 'function') f.setContent(diskContent);
    f.savedContent = diskContent;
    if (typeof diskMtimeNs === 'string') f.mtimeNs = diskMtimeNs;
    setFileDirty(path, false);
    bar.remove();
  });
  bar.querySelector('.ext-keep').addEventListener('click', () => {
    // Silence the poller for THIS disk revision so it doesn't re-prompt
    // every 1.5s; the user's next save will overwrite it.
    if (typeof diskMtimeNs === 'string') f.mtimeNs = diskMtimeNs;
    bar.remove();
  });
}

// Hook terminal submit + chat shell to refresh immediately too.
const _origSubmit = submitTerminalCommand;
submitTerminalCommand = async function(id, raw) {
  await _origSubmit(id, raw);
  refreshFileTreeIfOpen();
  saveState();                  // cwd may have changed via cd
};
const _origRender = renderAIResponse;
renderAIResponse = function(el, j, opts) {
  _origRender(el, j);
  if (opts && opts.replay) return;   // restoring history: no side effects
  if (j.handler && j.handler.kind === 'shell') refreshFileTreeIfOpen();
  if (j.handler && j.handler.kind === 'components_answer' && j.handler.file_path) {
    refreshFileTreeIfOpen();
    openFile(j.handler.file_path);
  }
};

// Copy the whole conversation, including collapsed thinking chains, as
// plain text. Uses textContent (not innerText) so content inside closed
// <details> expanders is captured too.
function chatLogAsText() {
  const parts = [];
  for (const msg of chatLog.querySelectorAll('.chat-msg')) {
    const roleEl = msg.querySelector('.role');
    const body   = msg.querySelector('.body');
    if (!body) continue;
    const role  = roleEl ? roleEl.textContent : '?';
    const chain = body.querySelector('details.chain');
    let text;
    if (chain) {
      const headline = body.querySelector('.ai-headline');
      text = headline ? headline.textContent.trim() : '';
      for (const layer of chain.querySelectorAll('.layer')) {
        const labEl     = layer.querySelector('.lab');
        const payloadEl = layer.querySelector('.payload');
        const lab     = labEl     ? labEl.textContent.trim() : '';
        const payload = payloadEl ? payloadEl.textContent    : '';
        text += `\n\n[${lab}]\n${payload}`;
      }
      const tagLine = body.querySelector('.tag-line');
      if (tagLine) text += '\n\n' + tagLine.textContent;
    } else {
      text = body.textContent;
    }
    parts.push(`===== ${role} =====\n${(text || '').trim()}`);
  }
  return parts.join('\n\n');
}

const aiCopyBtn = document.getElementById('ai-copy');
aiCopyBtn.addEventListener('click', async () => {
  const text = chatLogAsText();
  const flash = ok => {
    aiCopyBtn.textContent = ok ? '✅' : '⚠️';
    setTimeout(() => { aiCopyBtn.textContent = '📋'; }, 1200);
  };
  try {
    await navigator.clipboard.writeText(text);
    flash(true);
  } catch {
    // No clipboard API (e.g. plain http from another machine): fall back
    // to a transient textarea + execCommand.
    const ta = document.createElement('textarea');
    ta.value = text;
    ta.style.position = 'fixed';
    ta.style.opacity = '0';
    document.body.appendChild(ta);
    ta.select();
    let ok = false;
    try { ok = document.execCommand('copy'); } catch {}
    ta.remove();
    flash(ok);
  }
});

// --- Web-lookup globe toggle ---
// Reflects the current project's .ac9ai.cfg web_lookup flag. Three states:
// disabled (no project open), on (globe), off (globe + red circle-slash).
const webLookupBtn = document.getElementById('ai-weblookup');
let webLookupState = { has_project: false, web_lookup: false };

function paintWebLookupBtn() {
  webLookupBtn.classList.remove('on', 'off', 'disabled');
  if (!state.rootDir) {
    webLookupBtn.classList.add('disabled');
    webLookupBtn.title = 'Web lookup: no project open — open a folder so the setting can be saved';
    webLookupBtn.setAttribute('aria-pressed', 'false');
    return;
  }
  const on = !!webLookupState.web_lookup;
  webLookupBtn.classList.add(on ? 'on' : 'off');
  webLookupBtn.title = on
    ? 'Web lookup ENABLED for this project — click to disable'
    : 'Web lookup DISABLED for this project — click to enable';
  webLookupBtn.setAttribute('aria-pressed', on ? 'true' : 'false');
}

async function refreshWebLookupBtn() {
  if (!state.rootDir) { webLookupState = { has_project: false, web_lookup: false }; paintWebLookupBtn(); return; }
  try {
    const r = await fetch('/api/project/config?cwd=' + encodeURIComponent(state.rootDir));
    if (r.ok) webLookupState = await r.json();
  } catch {}
  paintWebLookupBtn();
}

webLookupBtn.addEventListener('click', async () => {
  if (!state.rootDir) return;   // greyed out; nothing to store
  const next = !webLookupState.web_lookup;
  try {
    const r = await fetch('/api/project/config', {
      method: 'POST', headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ cwd: state.rootDir, web_lookup: next }),
    });
    if (r.ok) webLookupState = await r.json();
    else {
      const j = await r.json().catch(() => ({}));
      alert('Could not update web lookup: ' + (j.error || r.status));
    }
  } catch (err) {
    alert('Could not update web lookup: ' + err.message);
  }
  paintWebLookupBtn();
});

paintWebLookupBtn();

// Boot — resolve session first, then restore state.
bootSession();

async function bootSession() {
  if (currentSessionId) {
    // Verify the session still exists; otherwise fall through to picker.
    try {
      const r = await fetch('/api/sessions/' + currentSessionId);
      if (r.ok) { restoreState(); return; }
    } catch {}
    currentSessionId = null;
    history.replaceState(null, '', location.pathname);
  }
  // Fresh tab (no fragment): if the server has an active tickets/run
  // for some project, auto-resume that project's session so the AI
  // pane rehydrates without the user picking through ghost rows.
  try {
    const r = await fetch('/api/sessions/active');
    if (r.ok) {
      const j = await r.json();
      if (j && j.id) {
        setSessionInUrl(j.id);
        restoreState();
        return;
      }
    }
  } catch {}
  let sessions = [];
  try {
    const r = await fetch('/api/sessions');
    if (r.ok) sessions = (await r.json()).sessions || [];
  } catch {}
  if (sessions.length === 0) {
    const m = await createSessionOnServer({});
    if (m) { setSessionInUrl(m.id); restoreState(); return; }
    // server unreachable — soldier on with a transient id; sync will retry.
    setSessionInUrl('00000000-0000-0000-0000-000000000000');
    restoreState();
    return;
  }
  // Auto-pick the most-recently-active session and skip the picker.
  // The picker is still reachable via the top menu ("New session" /
  // "Switch session"), so users can change out of this default. Server
  // returns sessions sorted by last_active descending; pick the head.
  const winner = sessions[0];
  if (winner && winner.id) {
    setSessionInUrl(winner.id);
    restoreState();
    return;
  }
  openSessionPicker({ allowClose: false, sessions });
}

// ---- tickets (Kanban board for .tickets.agile) ------------------------
// See the shared contract in the release notes for the file/JSON/endpoint
// shape. Column keys are read from the board JSON — never hardcoded — so
// projects can rename columns by editing the file directly.
async function ticketsApiError(r) {
  let msg = 'HTTP ' + r.status;
  try { const j = await r.json(); if (j && j.error) msg = j.error; } catch {}
  return msg;
}

function ticketsCwdFor(path) {
  // <cwd>/.tickets.agile — strip the trailing filename.
  const i = path.lastIndexOf('/');
  return i >= 0 ? path.slice(0, i) : path;
}

async function openTicketsFile(path) {
  const cwd = ticketsCwdFor(path);

  // Surface (per-file editor container).
  const surface = document.createElement('div');
  surface.className = 'editor-surface tickets-surface';
  surface.dataset.path = path;
  editorBody.appendChild(surface);
  for (const ff of Object.values(state.files)) ff.surface.classList.remove('active');
  surface.classList.add('active');

  // Toolbar strip with the run/stop button (hammer icon toggles to a
  // stop sign while the AI is chewing through todo tickets).
  const toolbar = document.createElement('div');
  toolbar.className = 'tickets-toolbar';
  const runBtn = document.createElement('button');
  runBtn.type = 'button';
  runBtn.className = 'tickets-run-btn';
  runBtn.innerHTML = '<span class="icon">🔨</span><span class="label">Build</span>';
  runBtn.title = 'Kick off the AI to work through todo tickets in order';
  const runStatus = document.createElement('span');
  runStatus.className = 'tickets-run-status';
  toolbar.appendChild(runBtn);
  toolbar.appendChild(runStatus);
  surface.appendChild(toolbar);

  const boardHost = document.createElement('div');
  boardHost.className = 'tickets-board';
  surface.appendChild(boardHost);

  // Styling lives in app.css under the .tickets-* namespace, which is
  // also the namespace the DOM below emits.

  // Register in state.files so activateFile / tabs / closeFile all work.
  const fileEntry = {
    mode: 'tickets',
    savedContent: '',
    surface,
    tab: null,
    getContent: () => '',
    setContent: () => {},
    destroy: () => {
      ticketsStopPolling(path);
      ticketsStopRunPolling(path);
      const g = state.files[path];
      if (g && g.eventsAbort) { try { g.eventsAbort.abort(); } catch {} }
    },
    dirty: false,
    cwd,
    board: null,
    ticketsHost: boardHost,
    pollTimer: null,
    runBtn,
    runStatus,
    runPollTimer: null,
    runLastState: { running: false, current_ticket_id: '' },
  };
  state.files[path] = fileEntry;

  buildEditorTab(path, 'tickets');
  activateFile(path);
  saveState();

  runBtn.addEventListener('click', () => ticketsRunToggle(path));

  await ticketsRefresh(path);
  ticketsStartPolling(path);
  ticketsStartRunPolling(path);
  ticketsRunStatus(path);
  // Fire-and-forget the events subscription; it self-reconnects and
  // exits when the file entry is destroyed.
  ticketsSubscribeEvents(path).catch(err =>
    console.error('ticketsSubscribeEvents failed', err));
}

// Client-side ticket runner: drives runChatTurn() in a loop so the user
// sees each ticket's pipeline unfold in the AI pane exactly like a
// manual submit. The server-side /api/tickets/run/* endpoints are still
// wired for the CLI harness; the browser doesn't call them.
// Server-driven run: browser hits /api/tickets/run/start; the C++ worker
// iterates todo tickets, and each ticket's SSE (plus ticket_start /
// ticket_end lifecycle events) is broadcast to any subscriber. The
// tickets tab subscribes to that broadcast on open and renders frames
// into the AI pane so a headless CLI run is visible.
async function ticketsRunToggle(path) {
  const f = state.files[path];
  if (!f) return;
  const url = f.runLastState.running
    ? '/api/tickets/run/stop'
    : '/api/tickets/run/start';
  try {
    const r = await fetch(url, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ cwd: f.cwd }),
    });
    if (!r.ok) alert(await ticketsApiError(r));
  } catch (err) {
    alert('run toggle failed: ' + err.message);
  }
  ticketsRunStatus(path);
}

async function ticketsRunStatus(path) {
  const f = state.files[path];
  if (!f) return;
  try {
    const r = await fetch('/api/tickets/run/status?cwd=' +
                          encodeURIComponent(f.cwd));
    if (!r.ok) return;
    const j = await r.json();
    ticketsApplyRunState(f, j);
  } catch { /* silent */ }
}

function ticketsApplyRunState(f, s) {
  const running = !!(s && s.running);
  const cur     = (s && s.current_ticket_id) || '';
  const layer   = (s && s.last_layer)        || '';
  if (!f.runLastState) f.runLastState = {};
  f.runLastState.running = running;
  f.runLastState.current_ticket_id = cur;
  if (running) {
    f.runBtn.classList.add('running');
    f.runBtn.innerHTML =
      '<span class="icon">🛑</span><span class="label">Stop</span>';
    f.runStatus.textContent = cur
      ? ('Working on ' + cur + (layer ? ' — ' + layer : ''))
      : 'Starting…';
    // AI-pane rehydrate. Server ring is 200 events; after ~140s of
    // heartbeats the ticket_start frame gets rotated out, so a browser
    // close + reopen (or a mid-ticket refresh) leaves the pane dark
    // waiting for a ticket_start that will never replay. Synthesize
    // one locally from the status poll's current_ticket_id + the body
    // we already have cached on f.board.
    if (cur && f.aiBootstrapTicket && f.aiCurTicketId !== cur) {
      let body = '';
      if (f.board && Array.isArray(f.board.tickets)) {
        const rec = f.board.tickets.find(t => t && t.id === cur);
        if (rec && typeof rec.body === 'string') body = rec.body;
      }
      f.aiBootstrapTicket(cur, body);
    }
  } else {
    f.runBtn.classList.remove('running');
    f.runBtn.innerHTML =
      '<span class="icon">🔨</span><span class="label">Build</span>';
    f.runStatus.textContent = '';
  }
}

function ticketsStartRunPolling(path) {
  const f = state.files[path];
  if (!f) return;
  if (f.runPollTimer) clearInterval(f.runPollTimer);
  f.runPollTimer = setInterval(() => {
    if (!state.files[path]) { ticketsStopRunPolling(path); return; }
    // Never let an unhandled rejection escape the interval callback:
    // an uncaught promise error does not stop setInterval, but it does
    // spam the console and can trip devtools "pause on exceptions".
    Promise.resolve()
      .then(() => ticketsRunStatus(path))
      .catch(err => {
        const g = state.files[path]; if (!g) return;
        g.runPollFailStreak = (g.runPollFailStreak || 0) + 1;
        if (g.runPollFailStreak === 3)
          console.warn('run-status poll: 3 consecutive errors:', err && err.message);
      });
  }, 1000);
}
function ticketsStopRunPolling(path) {
  const f = state.files[path];
  if (f && f.runPollTimer) { clearInterval(f.runPollTimer); f.runPollTimer = null; }
}

// Subscribe to the CLI/browser server-side runner's event stream so
// each ticket's pipeline pops into the AI pane. Auto-reconnects with a
// short backoff whenever the stream drops.
// Small connection-state dot rendered next to the tab title. Three
// classes on #conn-dot-<tabId>: ok / reconnecting / offline. Presence of
// the element is enough; CSS colors it. Absent when never subscribed.
function setConnState(path, cls) {
  const f = state.files[path];
  if (!f) return;
  let dot = f.connDot;
  if (!dot) {
    dot = document.createElement('span');
    dot.className = 'conn-dot';
    dot.title = 'Live event stream';
    (f.ticketsHost || document.body).parentNode
      ?.insertBefore(dot, f.ticketsHost || null);
    f.connDot = dot;
  }
  dot.classList.remove('ok', 'reconnecting', 'offline');
  dot.classList.add(cls);
}

// Rolling-avg storage: per-role token counts observed at each final.
// Used to draw a "how far along" progress bar sensibly.
function roleAvgKey(role) { return 'ac9_role_avg_' + role; }
function readRoleAvg(role) {
  try { return parseInt(localStorage.getItem(roleAvgKey(role)) || '0', 10) || 0; }
  catch { return 0; }
}
function writeRoleAvg(role, tokens) {
  if (!role || !Number.isFinite(tokens) || tokens <= 0) return;
  const prev = readRoleAvg(role);
  const next = prev ? Math.round(prev * 0.7 + tokens * 0.3) : tokens;
  try { localStorage.setItem(roleAvgKey(role), String(next)); } catch {}
}

// Apply an enriched heartbeat payload (role/tokens/max/loading) to the
// current ticket's DOM: swap headline between "loading (X)" and
// "thinking (X)", update the thin progress bar, remember which role is
// currently emitting tokens (recorded on next final for rolling avg).
function applyHeartbeatProgress(curCtx, hb) {
  if (!hb.role) return;
  const shorts = (g_modelsMap && g_modelsMap.shorts) || {};
  const short  = shorts[hb.role] || hb.role;
  const verb   = hb.loading ? 'loading' : 'thinking';
  // Match the layer handler: append "on gpu N" when the scheduler has
  // told us which card holds this role (via /api/gpu_stats.role, cached
  // on window.__ac9RoleGpu by the GPU ring widget's poller).
  const roleGpu = (window.__ac9RoleGpu || {})[hb.role];
  const suffix  = (typeof roleGpu === 'number') ? ' on gpu ' + roleGpu : '';
  curCtx.headlinePre.textContent = verb + ' (' + short + suffix + ')';
  if (curCtx.summaryText) {
    const nLayers = curCtx.layerCount || 0;
    curCtx.summaryText.textContent =
      verb + ' (' + short + suffix + ') — ' + nLayers + ' layers';
  }
  curCtx.activeRole = hb.role;
  // Progress: prefer historical avg, fall back to ceiling. Capped at
  // 99% until the final event actually lands.
  if (curCtx.progFill) {
    let pct = 0;
    if (hb.loading) {
      // During load there's no token budget; show an indeterminate
      // striped fill (CSS will animate it) via a special class.
      curCtx.progFill.classList.add('indeterminate');
      curCtx.progFill.style.width = '20%';
    } else {
      curCtx.progFill.classList.remove('indeterminate');
      const avg = readRoleAvg(hb.role);
      if (avg > 0 && hb.tokens) {
        pct = Math.min(99, Math.round((hb.tokens / avg) * 100));
      } else if (hb.tokens && hb.max) {
        pct = Math.min(99, Math.round((hb.tokens / hb.max) * 100));
      }
      curCtx.progFill.style.width = pct + '%';
    }
  }
}

// Fetched once and cached: { shorts: {role:short_name}, active: {planner:..., coder:...} }.
let g_modelsMap = null;
async function loadModelsMap() {
  if (g_modelsMap) return g_modelsMap;
  try {
    const r = await fetch('/api/models_map');
    if (r.ok) g_modelsMap = await r.json();
  } catch { /* ignore */ }
  return g_modelsMap || { shorts: {}, active: {} };
}

// Map a layer name emitted by the pipeline to a role. The chat pipeline
// emits many layers; each one corresponds to a particular model.
function layerToRole(layerName, active) {
  const l = (layerName || '').toLowerCase();
  if (l.startsWith('cleanup'))       return 'cleanup';
  if (l.startsWith('planner'))       return active.planner || 'planner-4b';
  if (l.startsWith('task ') || l === 'tasks' ||
      l.startsWith('rebuild') || l === 'shell' ||
      l === 'comment')                            return active.coder || 'coder';
  // Understanding stack (all served by qwen14b).
  if (['classify','entities','expertise','disambiguate','stylize',
       'render_final','parts_intent','resolve','noted',
       'answer','components_answer','physics_intent','chem_intent',
       'image_intent'].includes(l))
    return active.understanding || 'qwen14b';
  if (l.startsWith('physics'))       return 'physics';
  if (l.startsWith('chem'))          return 'chemistry';
  if (l.startsWith('vision'))        return 'vision';
  if (l === 'image_gen'  || l.startsWith('image gen'))   return 'image_gen';
  if (l === 'image_edit' || l.startsWith('image edit'))  return 'image_edit';
  // Data lookups don't run a model.
  if (['dictionary','thesaurus','knowledge','wikipedia'].includes(l))
    return null;
  return null;
}

// ---- GPU / system ring widgets ----------------------------------------
// Three-circle strip modeled on the FinalQuant worker-node status
// indicator: outer ring = RAM/VRAM %, inner ring = CPU%/util %, centre
// number = temperature °C. Polls /api/gpu_stats twice a second while
// the strip is mounted; stop by calling stopGpuStrip on the return.
// Lifted to module scope so BOTH the ticket runner and the free-form
// chat submit build the same widget set inside their AI reply bubbles.
const AI_SVG_NS = 'http://www.w3.org/2000/svg';
const AI_OUTER_R = 18, AI_OUTER_C = 2 * Math.PI * AI_OUTER_R;
const AI_INNER_R = 13, AI_INNER_C = 2 * Math.PI * AI_INNER_R;
function makeAiRing(label) {
  const el = document.createElement('div');
  el.className = 'ai-gpu';
  const svg = document.createElementNS(AI_SVG_NS, 'svg');
  svg.setAttribute('viewBox', '0 0 42 42');
  const mk = (r, sw, cls, dasharray) => {
    const c = document.createElementNS(AI_SVG_NS, 'circle');
    c.setAttribute('cx', 21); c.setAttribute('cy', 21);
    c.setAttribute('r', r);   c.setAttribute('stroke-width', sw);
    c.setAttribute('class', cls);
    if (dasharray) {
      c.setAttribute('stroke-dasharray', dasharray + ' ' + dasharray);
      c.setAttribute('stroke-dashoffset', dasharray);
    }
    return c;
  };
  const outerBg = mk(AI_OUTER_R, 3.5, 'ai-gpu__bg');
  const outerFg = mk(AI_OUTER_R, 3.5, 'ai-gpu__fg', AI_OUTER_C.toFixed(3));
  const innerBg = mk(AI_INNER_R, 3,   'ai-gpu__bg');
  const innerFg = mk(AI_INNER_R, 3,   'ai-gpu__fg', AI_INNER_C.toFixed(3));
  svg.appendChild(outerBg); svg.appendChild(outerFg);
  svg.appendChild(innerBg); svg.appendChild(innerFg);
  el.appendChild(svg);
  const temp = document.createElement('div');
  temp.className = 'ai-gpu__temp'; temp.textContent = '—';
  el.appendChild(temp);
  const lab = document.createElement('div');
  lab.className = 'ai-gpu__label'; lab.textContent = label;
  el.appendChild(lab);
  return { el, outerFg, innerFg, temp, lab };
}
function paintAiRing(w, memFrac, utilFrac, tempC, title) {
  memFrac  = Math.min(1, Math.max(0, memFrac  || 0));
  utilFrac = Math.min(1, Math.max(0, utilFrac || 0));
  w.outerFg.setAttribute('stroke-dashoffset',
    (AI_OUTER_C * (1 - memFrac)).toFixed(2));
  w.innerFg.setAttribute('stroke-dashoffset',
    (AI_INNER_C * (1 - utilFrac)).toFixed(2));
  const hot = Math.max(memFrac, utilFrac);
  const hue = Math.round(140 * (1 - hot));  // 140 = teal-green, 0 = red
  w.outerFg.setAttribute('stroke', `hsl(${hue}, 68%, 52%)`);
  w.innerFg.setAttribute('stroke', `hsl(${hue}, 55%, 42%)`);
  w.temp.textContent = (tempC != null && tempC >= 0) ? (tempC + '°') : '—';
  if (title) w.el.title = title;
}
async function pollAiGpuStats(rs) {
  try {
    const r = await fetch('/api/gpu_stats', { cache: 'no-store' });
    if (!r.ok) return;
    const j = await r.json();
    const s = j.system || {};
    if (rs.sys) {
      const memFrac = s.mem_total > 0 ? s.mem_used / s.mem_total : 0;
      const cpuFrac = (typeof s.cpu === 'number' && s.cpu >= 0)
                      ? s.cpu / 100 : 0;
      const totGB = s.mem_total ? (s.mem_total / (1024**3)).toFixed(1) : '?';
      const usedGB= s.mem_used  ? (s.mem_used  / (1024**3)).toFixed(1) : '?';
      paintAiRing(rs.sys, memFrac, cpuFrac, s.temp,
        `system: ${usedGB}/${totGB} GiB RAM, cpu ${s.cpu ?? '?'}%, ` +
        `${s.n_cpus ?? '?'} cpus, temp ${s.temp ?? '?'}°C`);
    }
    const gpus = Array.isArray(j.gpus) ? j.gpus : [];
    if (gpus.length !== rs.gpus.length) {
      rs.gpus.forEach(w => w.el.remove());
      rs.gpus = gpus.map((_, i) => {
        const w = makeAiRing('G' + i);
        rs.strip.appendChild(w.el);
        return w;
      });
    }
    // Refresh the role->gpu map so headline text can annotate
    // "thinking (X on gpu N)" when the coder / planner / qwen14b
    // etc. layers fire.
    const roleMap = {};
    gpus.forEach((g, i) => {
      const w = rs.gpus[i]; if (!w) return;
      const memFrac  = g.mem_total > 0 ? g.mem_used / g.mem_total : 0;
      const utilFrac = (typeof g.util === 'number' && g.util >= 0)
                       ? g.util / 100 : 0;
      const totGB = g.mem_total ? (g.mem_total / (1024**3)).toFixed(1) : '?';
      const usedGB= g.mem_used  ? (g.mem_used  / (1024**3)).toFixed(1) : '?';
      const roleStr = g.role ? ` [${g.role}]` : '';
      paintAiRing(w, memFrac, utilFrac, g.temp,
        `gpu${g.id ?? i} ${g.name || ''}${roleStr}: ${usedGB}/${totGB} GiB VRAM, ` +
        `util ${g.util ?? '?'}%, temp ${g.temp ?? '?'}°C`);
      if (g.role) roleMap[g.role] = (g.id ?? i);
    });
    window.__ac9RoleGpu = roleMap;
  } catch { /* server down / poll skipped */ }
}
function startAiGpuStrip(body) {
  const strip = document.createElement('div');
  strip.className = 'ai-gpu-strip';
  const sys = makeAiRing('SYS');
  strip.appendChild(sys.el);
  body.appendChild(strip);
  const rs = { strip, sys, gpus: [], timer: null };
  const tick = () => pollAiGpuStats(rs);
  tick();
  rs.timer = setInterval(tick, 500);
  return rs;
}
function stopAiGpuStrip(rs) {
  if (!rs) return;
  if (rs.timer) { clearInterval(rs.timer); rs.timer = null; }
  // Also remove the strip from the DOM so a finished turn's bubble
  // doesn't keep the rings around as frozen decoration. Caller relies
  // on this — see the chat / ticket final handlers.
  if (rs.strip && rs.strip.parentNode) rs.strip.parentNode.removeChild(rs.strip);
  rs.strip = null;
  rs.sys   = null;
  rs.gpus  = [];
}

async function ticketsSubscribeEvents(path) {
  const f = state.files[path];
  if (!f) return;
  // Ensure the model map is ready before the first layer event lands so
  // the "thinking (Q3 Think 30)" label works from the very first frame.
  await loadModelsMap();
  if (f.eventsAbort) { try { f.eventsAbort.abort(); } catch {} }
  f.eventsAbort = new AbortController();
  // Persistent cursor + de-dupe set survive across reconnects within a
  // single page load. Session id is set from the server's first `session`
  // frame; a change means the server restarted and we must wipe.
  f.evtSession   = f.evtSession   || null;
  f.evtLastSeq   = f.evtLastSeq   || 0;
  f.evtSeenIds   = f.evtSeenIds   || new Set();
  let cur = null;   // { headlinePre, layersEl, summaryText, layerCount, noteHeartbeat, hbStop, sumRow }

  const openTicket = (id, bodyText) => {
    // A fresh chat pair per ticket, styled the same as a manual turn.
    if (cur && cur.hbStop) cur.hbStop();
    if (cur && cur.clockTimer) clearInterval(cur.clockTimer);
    if (cur && cur.gpuStrip) stopAiGpuStrip(cur.gpuStrip);
    pushMsg('user', 'ticket ' + id + ':\n' + (bodyText || ''));
    const aiEl = pushMsg('ai', '');
    const body = aiEl.querySelector('.body');
    body.innerHTML = '';
    // Three-ring node-status widget across the top of the ai reply:
    // system (RAM/CPU/temp) + one per GPU (VRAM/util/temp).
    const gpuStrip = startAiGpuStrip(body);
    // Thin progress bar under the strip.
    const progWrap = document.createElement('div');
    progWrap.className = 'ai-progbar';
    const progFill = document.createElement('div');
    progFill.className = 'ai-progbar-fill';
    progWrap.appendChild(progFill);
    body.appendChild(progWrap);
    // Header row: "thinking (model)" left, elapsed clock right so the
    // GPU circles above don't overlap the running time.
    const headRow = document.createElement('div');
    headRow.className = 'ai-headrow';
    const headlinePre = document.createElement('pre');
    headlinePre.className = 'ai-headline';
    headlinePre.textContent = 'thinking…';
    const clockEl = document.createElement('div');
    clockEl.className = 'ai-clock';
    clockEl.textContent = '0:00';
    headRow.appendChild(headlinePre);
    headRow.appendChild(clockEl);
    body.appendChild(headRow);
    const chain = document.createElement('details');
    chain.className = 'chain';
    chain.innerHTML = '<summary></summary><div class="layers"></div>';
    const sumRow = chain.querySelector('summary');
    const summaryText = document.createElement('span');
    summaryText.textContent = 'thinking… (0 layers)';
    sumRow.appendChild(summaryText);
    body.appendChild(chain);
    const layersEl = chain.querySelector('.layers');
    // Mirror the normal chat submit's rainbow-heartbeat wiring: the
    // .thinking-live class runs the rainbow-wave animation, and its
    // presence is gated by server-emitted `event: heartbeat` SSE frames
    // reaching noteHeartbeat(). A 2.5s watchdog freezes the animation
    // when the pipeline stops producing so it never lies about progress.
    let hbTimer = null;
    const hbStop = () => {
      if (hbTimer) { clearTimeout(hbTimer); hbTimer = null; }
      headlinePre.classList.remove('thinking-live');
      sumRow.classList.remove('thinking-live');
    };
    const noteHeartbeat = () => {
      headlinePre.classList.add('thinking-live');
      sumRow.classList.add('thinking-live');
      if (hbTimer) clearTimeout(hbTimer);
      hbTimer = setTimeout(hbStop, 12000);
    };
    // Kick the animation on immediately so the bubble looks alive from
    // ticket-open, even before the first pipeline heartbeat lands.
    noteHeartbeat();
    // Running elapsed clock.
    const t0 = Date.now();
    const clockTimer = setInterval(() => {
      const s = Math.floor((Date.now() - t0) / 1000);
      clockEl.textContent = Math.floor(s / 60) + ':' +
        String(s % 60).padStart(2, '0');
    }, 250);
    cur = { headlinePre, layersEl, summaryText, layerCount: 0,
            noteHeartbeat, hbStop, sumRow,
            progFill, clockEl, clockTimer, t0, gpuStrip };
    f.aiCurTicketId = id;
    chatLog.scrollTop = chatLog.scrollHeight;
  };
  // Publish an idempotent bootstrap hook so the run/status poller can
  // synthesize a local ticket_start when the SSE ring has rotated its
  // real one out (server ring is 200 events; a 20-min ticket produces
  // more heartbeats than that, so on a browser close + reopen the
  // replay contains no ticket_start and the pane would stay dark).
  f.aiBootstrapTicket = (id, bodyText) => {
    if (!id) return;
    if (f.aiCurTicketId === id) return;
    openTicket(id, bodyText || '(ticket in progress; body reconstructed ' +
                                'from board — no SSE ticket_start left in ' +
                                'ring buffer)');
  };
  const baseUrl = '/api/tickets/run/events?cwd=' + encodeURIComponent(f.cwd);
  let backoff = 500;
  let stuckTimer = null;
  const setStuckSoon = () => {
    if (stuckTimer) clearTimeout(stuckTimer);
    stuckTimer = setTimeout(() => setConnState(path, 'offline'), 8000);
  };
  const clearStuck = () => {
    if (stuckTimer) { clearTimeout(stuckTimer); stuckTimer = null; }
  };
  setConnState(path, 'reconnecting');
  setStuckSoon();
  while (state.files[path]) {
    let url = baseUrl;
    // Always append since=<sess>-<seq>, using 0-0 on the initial connect
    // so the server replays whatever is in the ring buffer. This is what
    // makes ticket_start / earlier layers show up when a browser tab
    // opens mid-ticket.
    url += '&since=' + encodeURIComponent(
        (f.evtSession || '0') + '-' + (f.evtLastSeq || 0));
    try {
      const r = await fetch(url, { signal: f.eventsAbort.signal });
      if (!r.ok || !r.body) throw new Error('HTTP ' + r.status);
      backoff = 500;
      clearStuck();
      setConnState(path, 'ok');
      const reader  = r.body.getReader();
      const decoder = new TextDecoder('utf-8');
      let buf = '';
      while (true) {
        const { value, done } = await reader.read();
        if (done) break;
        buf += decoder.decode(value, { stream: true });
        let sep;
        while ((sep = buf.indexOf('\n\n')) >= 0) {
          const frame = buf.slice(0, sep);
          buf = buf.slice(sep + 2);
          let evt = 'message', payload = '', evtId = '';
          for (const line of frame.split('\n')) {
            if      (line.startsWith('id: '))    evtId = line.slice(4).trim();
            else if (line.startsWith('event: ')) evt = line.slice(7).trim();
            else if (line.startsWith('data: '))  payload += line.slice(6);
          }
          if (!evt || evt === 'message') continue;
          // De-dupe against anything we already rendered in this page load.
          if (evtId) {
            if (f.evtSeenIds.has(evtId)) continue;
            f.evtSeenIds.add(evtId);
            if (f.evtSeenIds.size > 1000) {
              // Bound the set: drop the oldest ~200 by rebuilding.
              const arr = Array.from(f.evtSeenIds).slice(-800);
              f.evtSeenIds = new Set(arr);
            }
            // Track highest seq for the next reconnect cursor.
            const dash = evtId.indexOf('-');
            if (dash > 0) {
              const seq = Number(evtId.slice(dash + 1));
              if (Number.isFinite(seq) && seq > f.evtLastSeq) f.evtLastSeq = seq;
            }
          }
          if (evt === 'session') {
            let sj = {};
            try { sj = payload ? JSON.parse(payload) : {}; } catch {}
            const sid = String(sj.id || '');
            if (f.evtSession && sid && f.evtSession !== sid) {
              // Server restarted mid-page. Wipe the pane so we don't
              // stack a stale ticket on top of the fresh session.
              if (cur && cur.hbStop) cur.hbStop();
              chatLog.innerHTML = '';
              cur = null;
              f.evtLastSeq = 0;
              f.evtSeenIds = new Set();
            }
            if (sid) f.evtSession = sid;
            continue;
          }
          if (evt === 'run_start') {
            if (cur && cur.hbStop) cur.hbStop();
            chatLog.innerHTML = '';
            cur = null;
            f.aiCurTicketId = null;
            continue;
          }
          if (evt === 'heartbeat') {
            if (cur && cur.noteHeartbeat) cur.noteHeartbeat();
            // Parse role/tokens/max/loading enrichments (server extended
            // the heartbeat payload; older clients ignore these fields).
            try {
              const hb = payload ? JSON.parse(payload) : null;
              if (hb && cur) applyHeartbeatProgress(cur, hb);
            } catch { /* ignore malformed */ }
            continue;
          }
          let j;
          try { j = payload ? JSON.parse(payload) : {}; } catch { continue; }
          if (evt === 'ticket_start') {
            openTicket(j.id || '?', j.body || '');
          } else if (evt === 'layer' && cur) {
            cur.layerCount++;
            const div = document.createElement('div');
            div.className = 'layer';
            div.innerHTML = '<div class="lab"></div><div class="payload"></div>';
            div.querySelector('.lab').textContent = j.name || '';
            div.querySelector('.payload').textContent = j.content || '';
            cur.layersEl.appendChild(div);
            // Update headline text with the model doing this layer, e.g.
            // "thinking (Q3 Think 30)". Rainbow class stays on and pulses
            // via the heartbeat watchdog.
            const role = layerToRole(j.name, (g_modelsMap && g_modelsMap.active) || {});
            const short = role && g_modelsMap && g_modelsMap.shorts
                        ? g_modelsMap.shorts[role] : null;
            // Add "on gpu N" when the LRU scheduler has recorded a card
            // for this role (from /api/gpu_stats' role field).
            const roleGpu = (window.__ac9RoleGpu || {})[role];
            const suffix = (typeof roleGpu === 'number') ? ' on gpu ' + roleGpu : '';
            if (short) {
              cur.headlinePre.textContent = 'thinking (' + short + suffix + ')';
            }
            cur.summaryText.textContent = short
              ? `thinking (${short}${suffix}) — ${cur.layerCount} layers`
              : `thinking… (${cur.layerCount} layers)`;
            if (cur.noteHeartbeat) cur.noteHeartbeat();
            chatLog.scrollTop = chatLog.scrollHeight;
          } else if (evt === 'final' && cur) {
            if (cur.hbStop) cur.hbStop();
            // Freeze the clock at the exact finish moment so a completed
            // ticket bubble shows the actual total time the run took.
            if (cur.clockTimer) {
              if (cur.clockEl && typeof cur.t0 === 'number') {
                const s = Math.floor((Date.now() - cur.t0) / 1000);
                cur.clockEl.textContent =
                  Math.floor(s / 60) + ':' + String(s % 60).padStart(2, '0');
              }
              clearInterval(cur.clockTimer); cur.clockTimer = null;
            }
            // Remove the rings entirely instead of leaving them frozen
            // as decoration on a finished bubble.
            if (cur.gpuStrip) { stopAiGpuStrip(cur.gpuStrip); cur.gpuStrip = null; }
            // Same for the progress bar — its parent (progWrap) is
            // stripped from the DOM so the finished bubble is clean.
            if (cur.progFill && cur.progFill.parentNode) {
              cur.progFill.parentNode.remove();
              cur.progFill = null;
            }
            // Record the last-seen token count as the rolling avg for
            // whatever role was actively producing when we hit final.
            // Not exact per-role but gives a usable baseline.
            if (cur.activeRole && j && j.handler && j.handler.stdout) {
              const est = Math.max(50, j.handler.stdout.length >> 2);
              writeRoleAvg(cur.activeRole, est);
            }
            cur.headlinePre.innerHTML = formatChatMarkdown(computeHeadline(j));
            highlightCodeIn(cur.headlinePre);
            cur.summaryText.textContent = `${cur.layerCount} layers`;
            const tag = document.createElement('div');
            tag.className = 'layer';
            tag.innerHTML = '<div class="lab">tags</div><div class="payload"></div>';
            const act = j.act || {};
            const tags = (act.tags || []).join(',');
            tag.querySelector('.payload').textContent =
              `[act=${act.act || '?'}${act.subtype ? ' subtype=' + act.subtype : ''}` +
              `${tags ? ' tags=' + tags : ''}] [${j.expertise || '?'}]`;
            cur.layersEl.appendChild(tag);
            cur = null;
            f.aiCurTicketId = null;
            chatLog.scrollTop = chatLog.scrollHeight;
          } else if (evt === 'ticket_end') {
            if (cur && cur.hbStop) cur.hbStop();
            // Same treatment as `final`: freeze the clock so the ticket
            // bubble shows how long it ran even when it ended without a
            // final event (blocked / errored).
            if (cur && cur.clockTimer) {
              if (cur.clockEl && typeof cur.t0 === 'number') {
                const s = Math.floor((Date.now() - cur.t0) / 1000);
                cur.clockEl.textContent =
                  Math.floor(s / 60) + ':' + String(s % 60).padStart(2, '0');
              }
              clearInterval(cur.clockTimer); cur.clockTimer = null;
            }
            if (cur && cur.gpuStrip) { stopAiGpuStrip(cur.gpuStrip); cur.gpuStrip = null; }
            // Strip the progress bar from the DOM. Blocked / errored
            // tickets used to leave a half-transparent 100% bar behind;
            // remove it so the finished bubble looks the same whether
            // the ticket succeeded or failed.
            if (cur && cur.progFill && cur.progFill.parentNode) {
              cur.progFill.parentNode.remove();
              cur.progFill = null;
            }
            cur = null;
            f.aiCurTicketId = null;
          }
        }
      }
    } catch (err) {
      if (err && err.name === 'AbortError') { clearStuck(); return; }
    }
    setConnState(path, 'reconnecting');
    setStuckSoon();
    await new Promise(r => setTimeout(r, backoff));
    backoff = Math.min(backoff * 2, 5000);
  }
  clearStuck();
}

function ticketsStartPolling(path) {
  const f = state.files[path];
  if (!f) return;
  if (f.pollTimer) clearInterval(f.pollTimer);
  f.pollTimer = setInterval(() => {
    if (!state.files[path]) { ticketsStopPolling(path); return; }
    Promise.resolve()
      .then(() => ticketsRefresh(path, /*silent*/ true))
      .catch(err => {
        const g = state.files[path]; if (!g) return;
        g.pollFailStreak = (g.pollFailStreak || 0) + 1;
        if (g.pollFailStreak === 3)
          console.warn('tickets poll: 3 consecutive rejections:', err && err.message);
      });
  }, 1500);
}

function ticketsStopPolling(path) {
  const f = state.files[path];
  if (f && f.pollTimer) { clearInterval(f.pollTimer); f.pollTimer = null; }
}

async function ticketsRefresh(path, silent) {
  const f = state.files[path];
  if (!f) return;
  let board;
  try {
    const r = await fetch('/api/tickets?cwd=' + encodeURIComponent(f.cwd));
    if (!r.ok) {
      f.pollFailStreak = (f.pollFailStreak || 0) + 1;
      if (!silent) alert(await ticketsApiError(r));
      else if (f.pollFailStreak === 3) console.warn(
        'tickets poll: 3 consecutive HTTP', r.status, 'from', f.cwd);
      return;
    }
    board = await r.json();
    f.pollFailStreak = 0;
  } catch (err) {
    f.pollFailStreak = (f.pollFailStreak || 0) + 1;
    if (!silent) alert('tickets fetch failed: ' + err.message);
    else if (f.pollFailStreak === 3) console.warn(
      'tickets poll: 3 consecutive fetch errors:', err.message);
    return;
  }
  // Skip the rerender when the fetched board is byte-identical to what
  // we already have. The board reloads on a 1.5 s poll; unconditional
  // innerHTML rewrites reset every scroll position (board horizontal
  // scroll + every column-body vertical scroll), which the user sees as
  // the page snapping back to the top every second and a half.
  const sig = JSON.stringify(board);
  if (f.boardSig === sig) return;
  f.boardSig = sig;
  f.board    = board;
  ticketsRender(path);
}

function ticketsRender(path) {
  const f = state.files[path];
  if (!f || !f.board) return;
  const board = f.board;
  const host = f.ticketsHost;

  // Snapshot scroll positions before rewriting the DOM so that a
  // rerender caused by an actual board change (drag-drop, external edit,
  // AI mutation) doesn't yank the user back to the top of the board.
  const prevScrollLeft = host.scrollLeft;
  const prevColScroll = {};
  for (const colEl of host.querySelectorAll('.tickets-column')) {
    const key = colEl.dataset.colKey;
    const body = colEl.querySelector('.tickets-column-body');
    if (key && body) prevColScroll[key] = body.scrollTop;
  }

  host.innerHTML = '';

  const columns = Array.isArray(board.columns) ? board.columns : [];
  const tickets = Array.isArray(board.tickets) ? board.tickets : [];

  // Group tickets by their status key.
  const byStatus = {};
  for (const col of columns) byStatus[col.key] = [];
  for (const t of tickets) {
    if (!byStatus[t.status]) byStatus[t.status] = [];
    byStatus[t.status].push(t);
  }
  // Client-side sort: highest priority first, then id ascending.
  const prioRank = { urgent: 0, high: 1, normal: 2, low: 3 };
  const idNum = (id) => parseInt(String(id || '').replace(/^T-/, ''), 10) || 0;
  for (const k of Object.keys(byStatus)) {
    byStatus[k].sort((a, b) => {
      const pa = prioRank[a.priority] ?? 2, pb = prioRank[b.priority] ?? 2;
      if (pa !== pb) return pa - pb;
      return idNum(a.id) - idNum(b.id);
    });
  }

  for (const col of columns) {
    const colEl = document.createElement('div');
    colEl.className = 'tickets-column';
    colEl.dataset.colKey = col.key;

    const head = document.createElement('div');
    head.className = 'tickets-column-header';
    const title = document.createElement('span');
    title.className = 'tickets-column-title';
    title.textContent = col.title || col.key;
    const count = document.createElement('span');
    count.className = 'tickets-column-count';
    count.textContent = '(' + (byStatus[col.key]?.length || 0) + ')';
    const add = document.createElement('button');
    add.className = 'tickets-column-add';
    add.type = 'button';
    add.title = 'New ticket in ' + (col.title || col.key);
    add.textContent = '+';
    add.addEventListener('click', () => openTicketCreateModal(path, col.key));
    head.appendChild(title);
    head.appendChild(count);
    head.appendChild(add);
    colEl.appendChild(head);

    const body = document.createElement('div');
    body.className = 'tickets-column-body';
    body.addEventListener('dragover', e => {
      e.preventDefault();
      e.dataTransfer.dropEffect = 'move';
      colEl.classList.add('tickets-drop-hover');
    });
    body.addEventListener('dragleave', () => colEl.classList.remove('tickets-drop-hover'));
    body.addEventListener('drop', async e => {
      e.preventDefault();
      colEl.classList.remove('tickets-drop-hover');
      const id = e.dataTransfer.getData('text/x-ticket-id');
      if (!id) return;
      await ticketsMove(path, id, col.key);
    });
    for (const t of byStatus[col.key] || []) {
      body.appendChild(ticketsCardEl(path, t));
    }
    colEl.appendChild(body);
    host.appendChild(colEl);
    // Restore this column's vertical scroll if it had one before rerender.
    if (prevColScroll[col.key]) body.scrollTop = prevColScroll[col.key];
  }
  // Restore the board's horizontal scroll after all columns are back in.
  if (prevScrollLeft) host.scrollLeft = prevScrollLeft;
}

function ticketsCardEl(path, t) {
  const card = document.createElement('div');
  card.className = 'tickets-card';
  card.draggable = true;
  card.dataset.ticketId = t.id;
  card.addEventListener('dragstart', e => {
    e.dataTransfer.setData('text/x-ticket-id', t.id);
    e.dataTransfer.effectAllowed = 'move';
    card.classList.add('dragging');
  });
  card.addEventListener('dragend', () => card.classList.remove('dragging'));
  card.addEventListener('click', () => openTicketEditModal(path, t.id));

  const head = document.createElement('div');
  head.className = 'tickets-card-id';
  head.textContent = t.id;
  card.appendChild(head);

  const title = document.createElement('div');
  title.className = 'tickets-card-title';
  title.textContent = t.title || '(untitled)';
  card.appendChild(title);

  const meta = document.createElement('div');
  meta.className = 'tickets-card-meta';
  const pri = document.createElement('span');
  const priKey = (t.priority || 'normal');
  pri.className = 'ticket-priority-' + priKey;
  pri.textContent = priKey;
  meta.appendChild(pri);
  if (Array.isArray(t.labels)) {
    for (const lab of t.labels) {
      if (!lab) continue;
      const chip = document.createElement('span');
      chip.className = 'ticket-label';
      chip.textContent = lab;
      meta.appendChild(chip);
    }
  }
  card.appendChild(meta);
  return card;
}

async function ticketsMove(path, id, status) {
  const f = state.files[path];
  if (!f) return;
  try {
    const r = await fetch('/api/tickets/' + encodeURIComponent(id) + '/move', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ cwd: f.cwd, status }),
    });
    if (!r.ok) { alert(await ticketsApiError(r)); return; }
  } catch (err) { alert('move failed: ' + err.message); return; }
  await ticketsRefresh(path);
}

// ---- ticket modal (create / edit) -------------------------------------
function ticketsModalEl() {
  let m = document.getElementById('ticket-modal');
  if (m) return m;
  m = document.createElement('div');
  m.id = 'ticket-modal';
  m.className = 'tickets-modal-overlay hidden';
  m.innerHTML =
    '<div class="tickets-modal">' +
      '<div class="tickets-modal-title">' +
        '<span id="ticket-modal-title">Ticket</span>' +
        '<button type="button" class="modal-close" id="ticket-modal-close" title="Close">×</button>' +
      '</div>' +
      '<div class="tickets-modal-body">' +
        '<label>Title<input type="text" id="tm-title"></label>' +
        '<label>Description<textarea id="tm-body"></textarea></label>' +
        '<label>Status<select id="tm-status"></select></label>' +
        '<label>Labels (comma-separated)<input type="text" id="tm-labels"></label>' +
        '<label>Priority<select id="tm-priority">' +
          '<option value="low">low</option>' +
          '<option value="normal">normal</option>' +
          '<option value="high">high</option>' +
          '<option value="urgent">urgent</option>' +
        '</select></label>' +
        '<label>Parent (ticket id or empty)<input type="text" id="tm-parent"></label>' +
      '</div>' +
      '<div class="tickets-modal-footer">' +
        '<button type="button" class="danger" id="tm-delete">Delete</button>' +
        '<button type="button" id="tm-cancel">Cancel</button>' +
        '<button type="button" class="primary" id="tm-save">Save</button>' +
      '</div>' +
    '</div>';
  document.body.appendChild(m);
  const close = () => m.classList.add('hidden');
  m.querySelector('#ticket-modal-close').addEventListener('click', close);
  m.querySelector('#tm-cancel').addEventListener('click', close);
  m.addEventListener('click', e => { if (e.target === m) close(); });
  return m;
}

function ticketsPopulateStatusSelect(sel, columns, current) {
  sel.innerHTML = '';
  for (const c of columns) {
    const opt = document.createElement('option');
    opt.value = c.key;
    opt.textContent = c.title || c.key;
    if (c.key === current) opt.selected = true;
    sel.appendChild(opt);
  }
}

function openTicketCreateModal(path, statusKey) {
  const f = state.files[path];
  if (!f || !f.board) return;
  const m = ticketsModalEl();
  m.querySelector('#ticket-modal-title').textContent = 'New ticket';
  m.querySelector('#tm-title').value = '';
  m.querySelector('#tm-body').value = '';
  ticketsPopulateStatusSelect(
    m.querySelector('#tm-status'), f.board.columns, statusKey
  );
  m.querySelector('#tm-labels').value = '';
  m.querySelector('#tm-priority').value = 'normal';
  m.querySelector('#tm-parent').value = '';
  m.querySelector('#tm-delete').style.display = 'none';

  // Rewire Save button (clear prior handlers by cloning).
  const oldSave = m.querySelector('#tm-save');
  const save = oldSave.cloneNode(true);
  oldSave.replaceWith(save);
  save.addEventListener('click', async () => {
    const title = m.querySelector('#tm-title').value.trim();
    if (!title) { alert('Title is required.'); return; }
    const labels = m.querySelector('#tm-labels').value.split(',')
                    .map(s => s.trim()).filter(Boolean);
    const parentRaw = m.querySelector('#tm-parent').value.trim();
    const payload = {
      cwd: f.cwd,
      title,
      body: m.querySelector('#tm-body').value,
      status: m.querySelector('#tm-status').value,
      labels,
      priority: m.querySelector('#tm-priority').value,
      parent: parentRaw || null,
    };
    try {
      const r = await fetch('/api/tickets/create', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload),
      });
      if (!r.ok) { alert(await ticketsApiError(r)); return; }
    } catch (err) { alert('create failed: ' + err.message); return; }
    m.classList.add('hidden');
    await ticketsRefresh(path);
  });
  m.classList.remove('hidden');
  m.querySelector('#tm-title').focus();
}

function openTicketEditModal(path, id) {
  const f = state.files[path];
  if (!f || !f.board) return;
  const t = (f.board.tickets || []).find(x => x.id === id);
  if (!t) { alert('ticket not found: ' + id); return; }
  const m = ticketsModalEl();
  m.querySelector('#ticket-modal-title').textContent = 'Edit ' + t.id;
  m.querySelector('#tm-title').value = t.title || '';
  m.querySelector('#tm-body').value  = t.body  || '';
  ticketsPopulateStatusSelect(
    m.querySelector('#tm-status'), f.board.columns, t.status
  );
  m.querySelector('#tm-labels').value   = Array.isArray(t.labels) ? t.labels.join(', ') : '';
  m.querySelector('#tm-priority').value = t.priority || 'normal';
  m.querySelector('#tm-parent').value   = t.parent || '';
  const delBtn = m.querySelector('#tm-delete');
  delBtn.style.display = '';

  const oldSave = m.querySelector('#tm-save');
  const save = oldSave.cloneNode(true);
  oldSave.replaceWith(save);
  save.addEventListener('click', async () => {
    const title = m.querySelector('#tm-title').value.trim();
    if (!title) { alert('Title is required.'); return; }
    const labels = m.querySelector('#tm-labels').value.split(',')
                    .map(s => s.trim()).filter(Boolean);
    const parentRaw = m.querySelector('#tm-parent').value.trim();
    const payload = {
      cwd: f.cwd,
      title,
      body: m.querySelector('#tm-body').value,
      status: m.querySelector('#tm-status').value,
      labels,
      priority: m.querySelector('#tm-priority').value,
      parent: parentRaw || null,
    };
    try {
      const r = await fetch('/api/tickets/' + encodeURIComponent(id), {
        method: 'PATCH',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload),
      });
      if (!r.ok) { alert(await ticketsApiError(r)); return; }
    } catch (err) { alert('save failed: ' + err.message); return; }
    m.classList.add('hidden');
    await ticketsRefresh(path);
  });

  const oldDel = delBtn;
  const del = oldDel.cloneNode(true);
  oldDel.replaceWith(del);
  del.addEventListener('click', async () => {
    if (!confirm('Delete ' + id + '?')) return;
    try {
      const r = await fetch(
        '/api/tickets/' + encodeURIComponent(id) +
        '?cwd=' + encodeURIComponent(f.cwd),
        { method: 'DELETE' }
      );
      if (!r.ok) { alert(await ticketsApiError(r)); return; }
    } catch (err) { alert('delete failed: ' + err.message); return; }
    m.classList.add('hidden');
    await ticketsRefresh(path);
  });

  m.classList.remove('hidden');
}

// ---- utils ------------------------------------------------------------
function escapeHTML(s) {
  return s.replace(/[&<>"']/g, c => ({
    '&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'
  }[c]));
}
