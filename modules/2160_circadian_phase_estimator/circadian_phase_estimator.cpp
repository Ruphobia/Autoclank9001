#include "circadian_phase_estimator.hpp"

namespace circadian_phase_estimator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Circadian Phase Estimator (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
