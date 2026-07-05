#pragma once

#include <string>

// Stub module for the "Sharding and partition planner" tool entry.
// Databases.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace sharding_and_partition_planner {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
