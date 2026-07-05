#pragma once

#include <string>

// Stub module for the "Rehearsal Scheduler" tool entry.
// Performing arts: theater, sound, lighting, stage.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace rehearsal_scheduler {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
