#pragma once

#include <string>

// Stub module for the "Species Season Calendar" tool entry.
// Hunting, fishing, trapping, wildlife tracking.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace species_season_calendar {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
