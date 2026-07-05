#include "hysplit_trajectory_runner.hpp"

namespace hysplit_trajectory_runner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HYSPLIT trajectory runner (Climate, weather, and atmospheric science). Awaits wire-up.";
    return s;
}

}
