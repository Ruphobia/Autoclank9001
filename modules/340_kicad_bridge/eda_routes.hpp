// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// HTTP surface for the KiCad integration. Registered against the
// existing cpp-httplib server owned by 010_interface.
//
// To wire in from 010_interface/server.cpp, add one line to the routes
// setup section (see the block that hosts /api/tickets, /api/terminal,
// etc.):
//
//     eda_routes::register_all(app);
//
// then include this header. No other server.cpp change is required.

namespace httplib { class Server; }

namespace eda_routes {

// Attach every /api/eda/* endpoint plus the /api/kicad/status probe.
void register_all(httplib::Server & app);

}
