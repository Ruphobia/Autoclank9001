#pragma once

#include <string>

// Stub module for the "Lee-Carter mortality forecaster" tool entry.
// Real estate, insurance, actuarial.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace lee_carter_mortality_forecaster {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
