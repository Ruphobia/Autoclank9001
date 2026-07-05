#pragma once

#include <string>

// Stub module for the "Consent and Intake Forms" tool entry.
// Body art: tattoo and body modification.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace consent_and_intake_forms {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
