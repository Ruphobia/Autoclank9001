#pragma once

#include <string>

// Stub module for the "Traceroute and latency map" tool entry.
// Web development.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace traceroute_and_latency_map {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
