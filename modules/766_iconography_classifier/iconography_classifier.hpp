#pragma once

#include <string>

// Stub module for the "Iconography classifier" tool entry.
// Religious studies and comparative religion.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace iconography_classifier {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
