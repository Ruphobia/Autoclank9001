#pragma once

#include <string>

// Stub module for the "UBO Graph Explorer" tool entry.
// KYC, identity, compliance verification.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace ubo_graph_explorer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
