#include "ledger_workbench.hpp"

namespace ledger_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ledger Workbench (Finance, markets, accounting). Awaits wire-up.";
    return s;
}

}
