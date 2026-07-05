#pragma once

#include <string>

// Stub module for the "Emulator RAM watcher" tool entry.
// Game playing assistants.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace emulator_ram_watcher {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
