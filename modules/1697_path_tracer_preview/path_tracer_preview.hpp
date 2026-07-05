#pragma once

#include <string>

// Stub module for the "Path Tracer Preview" tool entry.
// Animation, VFX, and 3D rendering.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace path_tracer_preview {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
