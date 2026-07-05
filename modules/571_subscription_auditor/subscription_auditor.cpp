#include "subscription_auditor.hpp"

namespace subscription_auditor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Subscription Auditor (Finance, markets, accounting). Awaits wire-up.";
    return s;
}

}
