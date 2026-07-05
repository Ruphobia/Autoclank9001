#pragma once

#include <string>

// Stub module for the "Contract Drafter" tool entry.
// Legal, patents, trademarks, compliance.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace contract_drafter {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
