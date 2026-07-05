#pragma once

#include <string>

// Stub module for the "Embedded browser tool" tool entry.
// Foundation (bootstrap).
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace embedded_browser_tool {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
