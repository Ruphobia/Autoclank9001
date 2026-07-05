#include "stash_and_project_ledger.hpp"

namespace stash_and_project_ledger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Stash And Project Ledger (Textiles, sewing, soft crafts). Awaits wire-up.";
    return s;
}

}
