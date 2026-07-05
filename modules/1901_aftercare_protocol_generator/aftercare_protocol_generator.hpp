#pragma once

#include <string>

// Stub module for the "Aftercare Protocol Generator" tool entry.
// Body art: tattoo and body modification.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace aftercare_protocol_generator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
