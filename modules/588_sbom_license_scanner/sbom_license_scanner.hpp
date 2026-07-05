#pragma once

#include <string>

// Stub module for the "SBOM License Scanner" tool entry.
// Legal, patents, trademarks, compliance.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace sbom_license_scanner {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
