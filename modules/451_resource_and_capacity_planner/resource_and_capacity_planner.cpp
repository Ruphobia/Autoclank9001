#include "resource_and_capacity_planner.hpp"

namespace resource_and_capacity_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Resource and capacity planner (Project management). Awaits wire-up.";
    return s;
}

}
