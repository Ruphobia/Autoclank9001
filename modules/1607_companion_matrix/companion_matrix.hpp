#pragma once

#include <string>

// Stub module for the "Companion Matrix" tool entry.
// Agriculture and horticulture.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace companion_matrix {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
