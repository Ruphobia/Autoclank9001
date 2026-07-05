#include "transaction_monitor.hpp"

namespace transaction_monitor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Transaction Monitor (KYC, identity, compliance verification). Awaits wire-up.";
    return s;
}

}
