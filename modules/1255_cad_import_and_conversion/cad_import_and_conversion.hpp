#pragma once

#include <string>

// Stub module for the "CAD import and conversion" tool entry.
// Robotics and 3D.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace cad_import_and_conversion {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
