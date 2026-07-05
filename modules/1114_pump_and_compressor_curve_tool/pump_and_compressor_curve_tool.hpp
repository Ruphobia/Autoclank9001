#pragma once

#include <string>

// Stub module for the "Pump and compressor curve tool" tool entry.
// Heavy engineering: naval, petroleum, mining, nuclear.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace pump_and_compressor_curve_tool {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
