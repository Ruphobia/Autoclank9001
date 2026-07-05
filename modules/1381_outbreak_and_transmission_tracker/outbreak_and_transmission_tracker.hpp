#pragma once

#include <string>

// Stub module for the "Outbreak and transmission tracker" tool entry.
// Microbiology, virology, and immunology.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace outbreak_and_transmission_tracker {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
