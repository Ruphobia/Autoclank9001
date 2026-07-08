// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// office_suite:: -- Collabora Online supervisor.
//
// Collabora Online IS OpenOffice (LibreOffice) with a browser front-end,
// distributed under MPL-2.0. ac9 owns the coolwsd lifecycle: fork the
// binary at ac9 startup, redirect its stdio to ~/.ac9/office-suite.log,
// probe /hosting/discovery until it comes up, and SIGTERM+SIGKILL it on
// ac9 shutdown. The front-end embeds cool.html in an iframe pointed at
// this WOPI host's /wopi/files/<file_id>?access_token=<token> endpoints
// (see modules/010_interface/server.cpp for the native C++ handlers).
//
// Environment overrides:
//   AC9_OFFICE_SUITE            "0" disables the supervisor entirely.
//   AC9_COLLABORA_BINARY        path to coolwsd (default
//                               /home/jwoods/work/collabora-prefix/usr/bin/coolwsd).
//   AC9_OFFICE_SUITE_RUNTIME    runtime dir holding config/, systemplate/,
//                               child-roots/, cache/, docs/  (default
//                               /home/jwoods/work/cool-runtime).
//   AC9_OFFICE_SUITE_LOTEMPLATE LibreOffice program dir (default
//                               /home/jwoods/work/collabora-prefix/opt/collaboraoffice).
//   AC9_OFFICE_SUITE_FSROOT     file-server root (default
//                               /home/jwoods/work/collabora-prefix/usr/share/coolwsd).
//   AC9_OFFICE_SUITE_LOG        override log path
//                               (default $HOME/.ac9/office-suite.log).
//   AC9_OFFICE_SUITE_URL        override the browser-visible coolwsd URL
//                               (default http://<hostname>:<port>).

#include <string>

namespace office_suite {

struct Status {
    bool        ready = false;   // /hosting/discovery returned 200 recently
    bool        enabled = true;  // set false when AC9_OFFICE_SUITE=0
    std::string detail;          // one-line human explanation
};

// Fork+exec coolwsd as a supervised subprocess. Returns immediately
// after the fork; a background thread polls /hosting/discovery so
// status() reflects readiness. Idempotent: subsequent calls are no-ops.
// Never throws; on failure, status().ready stays false and detail
// explains why.
void init(int coolwsd_port, const std::string & docs_dir);

// SIGTERM the supervised child, wait up to 10 s, then SIGKILL. Safe to
// call from a signal handler (async-signal-safe primitives only in the
// direct kill path). Idempotent.
void shutdown();

// Poll /hosting/discovery via a fresh httplib::Client; refreshes the
// cached status structure. Cheap enough to call every second.
Status status();

// Full URL the browser uses to load cool.html, honoring
// AC9_OFFICE_SUITE_URL if set, else http://<hostname>:<port>.
std::string coolwsd_url();

// Configured docs directory (absolute path). New Untitled.* files land
// here; the WOPI handlers sandbox reads/writes to this directory.
std::string docs_dir();

// 64-char hex opaque token generated once at init(). Front-end reads
// it from /api/office/config and passes it as ?access_token=... on the
// iframe URL. Server-side WOPI handlers reject any other value.
std::string access_token();

// True when init() decided the runtime is present and coolwsd was
// launched (or adopted). False when disabled or the binary is missing.
bool enabled();

}
