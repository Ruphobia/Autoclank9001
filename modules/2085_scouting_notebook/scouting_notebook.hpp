#pragma once

#include <string>

// Stub module for the "Scouting notebook" tool entry.
// Sports analytics, fitness, nutrition, coaching.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace scouting_notebook {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
