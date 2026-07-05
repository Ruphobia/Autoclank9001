#include "watchlist_screener.hpp"

namespace watchlist_screener {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Watchlist Screener (KYC, identity, compliance verification). Awaits wire-up.";
    return s;
}

}
