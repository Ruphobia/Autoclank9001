#include "okr_and_goal_tracker.hpp"

namespace okr_and_goal_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: OKR and goal tracker (Project management). Awaits wire-up.";
    return s;
}

}
