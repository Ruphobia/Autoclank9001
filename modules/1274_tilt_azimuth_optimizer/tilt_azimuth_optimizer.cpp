#include "tilt_azimuth_optimizer.hpp"

namespace tilt_azimuth_optimizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Tilt & Azimuth Optimizer (Energy systems). Awaits wire-up.";
    return s;
}

}
