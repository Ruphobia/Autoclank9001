#pragma once

#include <string>

// Stub module for the "Parasitology bench" tool entry.
// Zoology, veterinary, wildlife.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace parasitology_bench {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
