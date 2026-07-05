#pragma once

#include <string>

// Stub module for the "Citation Graph Explorer" tool entry.
// Memory and knowledge accrual.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace citation_graph_explorer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
