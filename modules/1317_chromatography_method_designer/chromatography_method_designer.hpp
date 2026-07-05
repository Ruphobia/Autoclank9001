#pragma once

#include <string>

// Stub module for the "Chromatography method designer" tool entry.
// Chemistry and lab.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace chromatography_method_designer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
