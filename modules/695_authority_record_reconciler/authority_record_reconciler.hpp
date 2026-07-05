#pragma once

#include <string>

// Stub module for the "Authority record reconciler" tool entry.
// History and archival research.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace authority_record_reconciler {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
