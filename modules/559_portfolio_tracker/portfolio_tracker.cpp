#include "portfolio_tracker.hpp"

namespace portfolio_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Portfolio Tracker (Finance, markets, accounting). Awaits wire-up.";
    return s;
}

}
