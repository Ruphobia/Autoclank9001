#pragma once

#include <string>

// Stub module for the "Incident and CAPA tracker" tool entry.
// Quality, safety, risk, and compliance.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace incident_and_capa_tracker {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
