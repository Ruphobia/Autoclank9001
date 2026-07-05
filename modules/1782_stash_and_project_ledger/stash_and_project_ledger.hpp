#pragma once

#include <string>

// Stub module for the "Stash And Project Ledger" tool entry.
// Textiles, sewing, soft crafts.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace stash_and_project_ledger {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
