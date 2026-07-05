#include "flight_planner.hpp"

namespace flight_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Flight planner (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
