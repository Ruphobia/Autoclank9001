#pragma once

#include <string>

// Stub module for the "Multispectral Image Stack" tool entry.
// Ancient texts and manuscript studies.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace multispectral_image_stack {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
