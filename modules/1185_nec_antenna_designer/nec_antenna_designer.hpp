#pragma once

#include <string>

// Stub module for the "NEC antenna designer" tool entry.
// Aviation, amateur radio, and RF.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace nec_antenna_designer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
