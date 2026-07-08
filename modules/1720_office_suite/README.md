# 1720_office_suite -- Collabora Online supervisor

## What this is

Collabora Online IS OpenOffice with a browser front-end. It bundles a
LibreOffice back-end (LGPL-3.0+ / MPL-2.0 in the shipping build) behind
a WebSocket + WOPI service called `coolwsd`. This module makes ac9 own
the `coolwsd` lifecycle:

- Fork + `execv` the `coolwsd` binary at ac9 startup.
- Redirect its stdout / stderr to `~/.ac9/office-suite.log`.
- Poll `/hosting/discovery` in the background until 200.
- SIGTERM (10 s grace) + SIGKILL on ac9 shutdown, ctrl-C, or crash.

The four surfaces (Writer, Calc, Impress, Draw) live in ac9's Office
menu. Clicking one opens an iframe in the CENTER PANE pointed at
`coolwsd`'s `cool.html`, with a WOPI src that resolves back to ac9's
own native C++ WOPI endpoints in `modules/010_interface/server.cpp`.

## Where the runtime comes from

Collabora Online is packaged separately. On this box the operator
installed it into a prefix at `/home/jwoods/work/collabora-prefix/`;
the runtime working dir is `/home/jwoods/work/cool-runtime/`. It is
NOT vendored into this repo (500 MB of chroot systemplate + a full
LibreOffice tree do not belong in git).

Install upstream via one of:

- `apt install coolwsd libreoffice-core` on Debian / Ubuntu.
- `dnf install coolwsd loolwsd-systemd` on Fedora.
- Container image `collabora/code`.
- Roll your own from https://github.com/CollaboraOnline/online.

Distribution: MPL-2.0. See `NOTICE` in the runtime prefix. This repo
does not redistribute Collabora binaries; it launches whatever is on
disk.

## How to disable at startup

Set `AC9_OFFICE_SUITE=0` in the environment before starting `ac9`.
The Office menu items will still appear, but clicking one shows the
`detail` from `/api/office/config` explaining that the suite is off.

## Env vars to override paths

Every path the supervisor uses can be redirected without recompile:

| Env var                         | Default                                                          |
|---------------------------------|------------------------------------------------------------------|
| `AC9_OFFICE_SUITE`              | `1` (set to `0` to disable)                                     |
| `AC9_COLLABORA_BINARY`          | `/home/jwoods/work/collabora-prefix/usr/bin/coolwsd`             |
| `AC9_OFFICE_SUITE_RUNTIME`      | `/home/jwoods/work/cool-runtime` (holds config, systemplate, ...)|
| `AC9_OFFICE_SUITE_LOTEMPLATE`   | `/home/jwoods/work/collabora-prefix/opt/collaboraoffice`         |
| `AC9_OFFICE_SUITE_FSROOT`       | `/home/jwoods/work/collabora-prefix/usr/share/coolwsd`           |
| `AC9_OFFICE_SUITE_LOG`          | `~/.ac9/office-suite.log`                                        |
| `AC9_OFFICE_SUITE_URL`          | `http://<hostname>:9980` (override for reverse-proxy setups)     |

If the runtime dir contains `nss_wrapper_passwd` and `nss_wrapper_group`,
the supervisor also sets `NSS_WRAPPER_PASSWD` / `NSS_WRAPPER_GROUP`
plus `LD_PRELOAD=libnss_wrapper.so` in the child so `coolwsd` starts
without a real `/etc/passwd` entry.

## Auth model

At `office_suite::init()`, ac9 generates a 64-character hex access
token from `/dev/urandom`. The token is exposed via
`/api/office/config` to the browser and required as `access_token=` on
every `/wopi/files/...` request. The token rotates every ac9 restart;
it is not persisted.

The WOPI handlers sandbox file access to `docs_dir()` (default
`/home/jwoods/work/cool-runtime/docs`). Any `file_id` containing a
slash, backslash, null byte, `.`, or `..` is rejected before the
filesystem sees it. Symlinks are resolved with
`std::filesystem::weakly_canonical` and the result must stay under
`docs_dir` or the request 404s.

## How to open a doc programmatically

The front-end helper accepts one of `writer` / `calc` / `impress` /
`draw`:

```js
openOfficeTab('writer');   // opens Untitled.odt in a new tab
openOfficeTab('calc');     // opens Untitled.ods
```

To point a surface at a specific document, drop the file into
`AC9_OFFICE_SUITE_RUNTIME/docs/`, then in the JS console:

```js
await fetch('/api/office/ensure', {
  method: 'POST', headers: {'Content-Type':'application/json'},
  body: JSON.stringify({ file_id: 'my-report.odt' }),
});
// then open with the standard Writer surface (edit OFFICE_SURFACES to
// point 'writer' at 'my-report.odt' if you want the menu item to
// track that file).
```

## Licensing note

- `coolwsd` and the shipped LibreOffice tree: MPL-2.0 (with parts LGPL-3.0+).
  Not redistributed by this repo. See the upstream `NOTICE` files.
- This module (source under `modules/1720_office_suite/`) is GPL-3.0-or-later,
  matching the rest of ac9.

## Logs

`~/.ac9/office-suite.log` (or `$AC9_OFFICE_SUITE_LOG`). Coolwsd writes
several MB per doc-open into this file; rotate it manually or
`truncate -s 0` when it gets large.
