#include "nec_wire_and_conduit_sizer.hpp"

namespace nec_wire_and_conduit_sizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: NEC wire and conduit sizer (Home and DIY (plumbing, residential electrical, HVAC, landscaping)). Awaits wire-up.";
    return s;
}

}
