#pragma once

#include <string>

// Stub module for the "Row-level lineage" tool entry.
// Databases.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace row_level_lineage {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
