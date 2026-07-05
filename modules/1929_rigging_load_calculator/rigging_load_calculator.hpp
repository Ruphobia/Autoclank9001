#pragma once

#include <string>

// Stub module for the "Rigging Load Calculator" tool entry.
// Performing arts: theater, sound, lighting, stage.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace rigging_load_calculator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
