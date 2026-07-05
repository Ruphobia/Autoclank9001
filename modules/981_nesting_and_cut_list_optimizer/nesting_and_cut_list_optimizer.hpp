#pragma once

#include <string>

// Stub module for the "Nesting and Cut List Optimizer" tool entry.
// Maker fab and personal manufacturing.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace nesting_and_cut_list_optimizer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
