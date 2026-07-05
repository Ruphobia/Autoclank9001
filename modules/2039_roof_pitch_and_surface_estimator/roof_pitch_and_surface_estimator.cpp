#include "roof_pitch_and_surface_estimator.hpp"

namespace roof_pitch_and_surface_estimator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Roof pitch and surface estimator (Home and DIY (plumbing, residential electrical, HVAC, landscaping)). Awaits wire-up.";
    return s;
}

}
