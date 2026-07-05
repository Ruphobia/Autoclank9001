#pragma once

#include <string>

// Stub module for the "Vector index lab" tool entry.
// Data, statistics, and ML.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace vector_index_lab {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
