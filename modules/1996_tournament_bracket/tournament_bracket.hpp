#pragma once

#include <string>

// Stub module for the "Tournament Bracket" tool entry.
// Esports, speedrunning, competitive game analytics.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace tournament_bracket {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
