#pragma once

#include <string>

// Stub module for the "Tufting Rug Designer" tool entry.
// Textiles, sewing, soft crafts.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace tufting_rug_designer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
