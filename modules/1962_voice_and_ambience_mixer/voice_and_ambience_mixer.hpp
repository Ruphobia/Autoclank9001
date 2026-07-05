#pragma once

#include <string>

// Stub module for the "Voice And Ambience Mixer" tool entry.
// Tabletop RPG and world building.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace voice_and_ambience_mixer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
