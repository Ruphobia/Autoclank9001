#pragma once

#include <string>

// Stub module for the "ADIF logbook" tool entry.
// Aviation, amateur radio, and RF.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace adif_logbook {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
