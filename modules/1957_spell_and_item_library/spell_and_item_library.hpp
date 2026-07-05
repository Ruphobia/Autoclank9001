#pragma once

#include <string>

// Stub module for the "Spell And Item Library" tool entry.
// Tabletop RPG and world building.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace spell_and_item_library {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
