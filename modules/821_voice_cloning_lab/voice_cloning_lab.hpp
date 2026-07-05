#pragma once

#include <string>

// Stub module for the "Voice cloning lab" tool entry.
// Speech, language, and translation.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace voice_cloning_lab {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
