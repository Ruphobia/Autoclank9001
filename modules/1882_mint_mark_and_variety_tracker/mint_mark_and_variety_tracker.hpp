#pragma once

#include <string>

// Stub module for the "Mint Mark and Variety Tracker" tool entry.
// Numismatics, philately, collecting, antiques.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace mint_mark_and_variety_tracker {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
