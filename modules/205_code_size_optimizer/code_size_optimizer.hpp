#pragma once

#include <string>

// Stub module for the "Code-size optimizer" tool entry.
// Firmware and embedded.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace code_size_optimizer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
