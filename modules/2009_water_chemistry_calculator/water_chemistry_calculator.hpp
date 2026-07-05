#pragma once

#include <string>

// Stub module for the "Water chemistry calculator" tool entry.
// Hobbies: gardening, beekeeping, aquariums, pets.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace water_chemistry_calculator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
