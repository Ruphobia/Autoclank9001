#include "gravity_assist_planner.hpp"

namespace gravity_assist_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Gravity Assist Planner (Aerospace and orbital mechanics). Awaits wire-up.";
    return s;
}

}
