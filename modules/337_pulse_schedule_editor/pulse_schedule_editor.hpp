#pragma once

#include <string>

// Stub module for the "Pulse Schedule Editor" tool entry.
// Quantum computing.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace pulse_schedule_editor {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
