#pragma once

#include <string>

// Stub module for the "Pollinator phenology log" tool entry.
// Hobbies: gardening, beekeeping, aquariums, pets.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace pollinator_phenology_log {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
