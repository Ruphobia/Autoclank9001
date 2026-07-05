#include "irrigation_zone_designer.hpp"

namespace irrigation_zone_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Irrigation zone designer (Home and DIY (plumbing, residential electrical, HVAC, landscaping)). Awaits wire-up.";
    return s;
}

}
