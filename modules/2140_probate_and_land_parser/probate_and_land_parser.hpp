#pragma once

#include <string>

// Stub module for the "Probate and Land Parser" tool entry.
// Detailed genealogy and family history.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace probate_and_land_parser {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
