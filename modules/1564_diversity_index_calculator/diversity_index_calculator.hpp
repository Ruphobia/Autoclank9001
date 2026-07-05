#pragma once

#include <string>

// Stub module for the "Diversity index calculator" tool entry.
// Ecology, conservation, sustainability.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace diversity_index_calculator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
