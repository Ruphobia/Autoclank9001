#include "sleep_debt_ledger.hpp"

namespace sleep_debt_ledger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sleep Debt Ledger (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
