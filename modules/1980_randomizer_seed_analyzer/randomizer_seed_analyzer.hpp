#pragma once

#include <string>

// Stub module for the "Randomizer seed analyzer" tool entry.
// Game playing assistants.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace randomizer_seed_analyzer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
