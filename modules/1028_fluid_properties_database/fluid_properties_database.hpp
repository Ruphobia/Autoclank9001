#pragma once

#include <string>

// Stub module for the "Fluid properties database" tool entry.
// Acoustic, thermal, hydraulic, pneumatic engineering.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace fluid_properties_database {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
