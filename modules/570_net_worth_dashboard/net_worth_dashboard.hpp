#pragma once

#include <string>

// Stub module for the "Net Worth Dashboard" tool entry.
// Finance, markets, accounting.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace net_worth_dashboard {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
