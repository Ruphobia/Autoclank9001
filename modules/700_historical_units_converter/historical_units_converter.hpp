#pragma once

#include <string>

// Stub module for the "Historical units converter" tool entry.
// History and archival research.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace historical_units_converter {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
