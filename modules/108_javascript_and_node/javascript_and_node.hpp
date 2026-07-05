#pragma once

#include <string>

// Stub module for the "JavaScript and Node" tool entry.
// Per-language toolchains.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace javascript_and_node {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
