#pragma once

#include <string>

// Stub module for the "Conworld Linguistic Atlas" tool entry.
// Linguistics and conlanging.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace conworld_linguistic_atlas {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
