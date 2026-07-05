#include "camera_move_planner.hpp"

namespace camera_move_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Camera Move Planner (Photography and cinematography). Awaits wire-up.";
    return s;
}

}
