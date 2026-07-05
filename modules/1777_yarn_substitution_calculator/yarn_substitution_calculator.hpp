#pragma once

#include <string>

// Stub module for the "Yarn Substitution Calculator" tool entry.
// Textiles, sewing, soft crafts.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace yarn_substitution_calculator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
