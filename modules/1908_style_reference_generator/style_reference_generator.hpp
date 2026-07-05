#pragma once

#include <string>

// Stub module for the "Style Reference Generator" tool entry.
// Body art: tattoo and body modification.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace style_reference_generator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
