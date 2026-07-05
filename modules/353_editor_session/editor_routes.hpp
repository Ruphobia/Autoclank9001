// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// HTTP routes for the interactive editor. Registered against the same
// cpp-httplib server used by 010_interface. Wire in with:
//
//     editor_routes::register_all(app);
//
// alongside the existing eda_routes::register_all(app).

namespace httplib { class Server; }

namespace editor_routes {

// Attach every /api/eda/editor/* endpoint.
void register_all(httplib::Server & app);

}
