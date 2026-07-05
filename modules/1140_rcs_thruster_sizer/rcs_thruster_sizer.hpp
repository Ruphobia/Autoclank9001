#pragma once

#include <string>

// Stub module for the "RCS & Thruster Sizer" tool entry.
// Aerospace and orbital mechanics.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace rcs_thruster_sizer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
