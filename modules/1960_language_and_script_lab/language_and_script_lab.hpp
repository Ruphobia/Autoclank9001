#pragma once

#include <string>

// Stub module for the "Language And Script Lab" tool entry.
// Tabletop RPG and world building.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace language_and_script_lab {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
