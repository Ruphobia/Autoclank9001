#pragma once

#include <string>

// Stub module for the "Ecological momentary assessment scheduler" tool entry.
// Psychology and behavioral science.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace ecological_momentary_assessment_scheduler {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
