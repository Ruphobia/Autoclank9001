#include "sprint_planner.hpp"

namespace sprint_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sprint planner (Project management). Awaits wire-up.";
    return s;
}

}
