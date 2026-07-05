#include "sharding_and_partition_planner.hpp"

namespace sharding_and_partition_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sharding and partition planner (Databases). Awaits wire-up.";
    return s;
}

}
