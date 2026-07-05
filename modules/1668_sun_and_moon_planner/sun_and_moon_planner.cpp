#include "sun_and_moon_planner.hpp"

namespace sun_and_moon_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sun and Moon Planner (Photography and cinematography). Awaits wire-up.";
    return s;
}

}
