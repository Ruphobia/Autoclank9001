#pragma once

#include <string>

// Stub module for the "Log aggregator" tool entry.
// General computing and sysadmin.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace log_aggregator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
