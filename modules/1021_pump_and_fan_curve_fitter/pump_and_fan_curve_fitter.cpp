#include "pump_and_fan_curve_fitter.hpp"

namespace pump_and_fan_curve_fitter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pump and fan curve fitter (Acoustic, thermal, hydraulic, pneumatic engineering). Awaits wire-up.";
    return s;
}

}
