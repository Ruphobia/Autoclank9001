#pragma once

#include <string>

// Stub module for the "Polling aggregator" tool entry.
// Sociology, political science, public policy.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace polling_aggregator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
