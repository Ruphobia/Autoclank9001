#include "envelope_heat_loss_stack.hpp"

namespace envelope_heat_loss_stack {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Envelope heat-loss stack (Home and DIY (plumbing, residential electrical, HVAC, landscaping)). Awaits wire-up.";
    return s;
}

}
