#pragma once

#include <string>

// Stub module for the "Meter and Prosody Scanner" tool entry.
// Ancient languages and paleography.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace meter_and_prosody_scanner {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
