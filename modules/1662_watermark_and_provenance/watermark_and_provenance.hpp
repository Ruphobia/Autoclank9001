#pragma once

#include <string>

// Stub module for the "Watermark and provenance" tool entry.
// Image and video.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace watermark_and_provenance {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
