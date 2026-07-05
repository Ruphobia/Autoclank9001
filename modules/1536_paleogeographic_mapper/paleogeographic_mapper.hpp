#pragma once

#include <string>

// Stub module for the "Paleogeographic mapper" tool entry.
// Paleontology and archaeology.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace paleogeographic_mapper {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
