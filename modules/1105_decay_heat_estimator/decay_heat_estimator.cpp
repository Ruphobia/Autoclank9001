#include "decay_heat_estimator.hpp"

namespace decay_heat_estimator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Decay heat estimator (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
