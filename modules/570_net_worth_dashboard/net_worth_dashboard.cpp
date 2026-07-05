#include "net_worth_dashboard.hpp"

namespace net_worth_dashboard {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Net Worth Dashboard (Finance, markets, accounting). Awaits wire-up.";
    return s;
}

}
