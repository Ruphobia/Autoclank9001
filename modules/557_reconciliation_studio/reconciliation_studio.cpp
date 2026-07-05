#include "reconciliation_studio.hpp"

namespace reconciliation_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Reconciliation Studio (Finance, markets, accounting). Awaits wire-up.";
    return s;
}

}
