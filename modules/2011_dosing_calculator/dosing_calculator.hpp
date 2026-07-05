#pragma once

#include <string>

// Stub module for the "Dosing calculator" tool entry.
// Hobbies: gardening, beekeeping, aquariums, pets.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace dosing_calculator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
