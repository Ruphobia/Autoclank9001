#include "qth_and_station_planner.hpp"

namespace qth_and_station_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: QTH and station planner (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
