#pragma once

#include <string>

// Stub module for the "Frame Geometry Comparator" tool entry.
// Automotive and vehicle systems.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace frame_geometry_comparator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
