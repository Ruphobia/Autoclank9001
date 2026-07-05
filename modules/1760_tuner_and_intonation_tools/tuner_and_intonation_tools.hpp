#pragma once

#include <string>

// Stub module for the "Tuner and intonation tools" tool entry.
// Signal processing and audio.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace tuner_and_intonation_tools {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
