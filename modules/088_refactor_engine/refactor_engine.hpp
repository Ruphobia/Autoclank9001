#pragma once

#include <string>

// Stub module for the "Refactor engine" tool entry.
// Software development.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace refactor_engine {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
