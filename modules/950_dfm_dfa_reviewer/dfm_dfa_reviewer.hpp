#pragma once

#include <string>

// Stub module for the "DFM/DFA reviewer" tool entry.
// Mechanical and manufacturing.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace dfm_dfa_reviewer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
