#pragma once

#include <string>

// Stub module for the "Lab and Method Comparator" tool entry.
// Academic and scholarly research.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace lab_and_method_comparator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
