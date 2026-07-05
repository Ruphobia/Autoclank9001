#pragma once

#include <string>

// Stub module for the "GM Screen Reference" tool entry.
// Tabletop RPG and world building.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace gm_screen_reference {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
