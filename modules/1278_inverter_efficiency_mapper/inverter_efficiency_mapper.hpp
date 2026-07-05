#pragma once

#include <string>

// Stub module for the "Inverter Efficiency Mapper" tool entry.
// Energy systems.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace inverter_efficiency_mapper {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
