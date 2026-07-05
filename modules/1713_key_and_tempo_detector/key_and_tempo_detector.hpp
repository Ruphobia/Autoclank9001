#pragma once

#include <string>

// Stub module for the "Key and Tempo Detector" tool entry.
// Music composition and theory.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace key_and_tempo_detector {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
