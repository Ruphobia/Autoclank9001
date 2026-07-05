#pragma once

#include <string>

// Stub module for the "World Bible" tool entry.
// Tabletop RPG and world building.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace world_bible {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
