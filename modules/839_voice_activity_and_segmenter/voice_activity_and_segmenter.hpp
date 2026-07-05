#pragma once

#include <string>

// Stub module for the "Voice activity and segmenter" tool entry.
// Speech, language, and translation.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace voice_activity_and_segmenter {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
