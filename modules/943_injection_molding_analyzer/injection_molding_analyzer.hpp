#pragma once

#include <string>

// Stub module for the "Injection molding analyzer" tool entry.
// Mechanical and manufacturing.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace injection_molding_analyzer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
