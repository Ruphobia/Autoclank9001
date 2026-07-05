#pragma once

#include <string>

// Stub module for the "Launch Window Finder" tool entry.
// Aerospace and orbital mechanics.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace launch_window_finder {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
