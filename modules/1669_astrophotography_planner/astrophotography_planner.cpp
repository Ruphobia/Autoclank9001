#include "astrophotography_planner.hpp"

namespace astrophotography_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Astrophotography Planner (Photography and cinematography). Awaits wire-up.";
    return s;
}

}
