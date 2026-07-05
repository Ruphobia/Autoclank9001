#pragma once

#include <string>

// Stub module for the "Sloper Block Library" tool entry.
// Textiles, sewing, soft crafts.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace sloper_block_library {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
