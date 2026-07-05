#pragma once

#include <string>

// Stub module for the "Citation manager" tool entry.
// Writing, publishing, journalism.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace citation_manager {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
