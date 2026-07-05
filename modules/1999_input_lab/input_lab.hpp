#pragma once

#include <string>

// Stub module for the "Input Lab" tool entry.
// Esports, speedrunning, competitive game analytics.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace input_lab {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
