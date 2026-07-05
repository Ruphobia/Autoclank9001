#pragma once

#include <string>

namespace web_server {

// Bind on 0.0.0.0:port and serve the embedded UI + /api/* endpoints.
// Blocks until the server is stopped via SIGINT or stop().
void run(const std::string & host = "0.0.0.0", int port = 8080);

// Signal the running server (if any) to stop accepting new connections
// and exit run(). Safe to call from a signal handler.
void stop();

}
