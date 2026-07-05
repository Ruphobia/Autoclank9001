#pragma once

#include <string>

// Stub module for the "Costume and Wig Tracker" tool entry.
// Performing arts: theater, sound, lighting, stage.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace costume_and_wig_tracker {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
