#pragma once

#include <string>

// Stub module for the "Audience Beat Sheet" tool entry.
// Magic, juggling, stage illusion, performance.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace audience_beat_sheet {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
