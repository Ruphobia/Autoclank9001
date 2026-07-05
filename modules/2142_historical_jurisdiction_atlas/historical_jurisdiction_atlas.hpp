#pragma once

#include <string>

// Stub module for the "Historical Jurisdiction Atlas" tool entry.
// Detailed genealogy and family history.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace historical_jurisdiction_atlas {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
