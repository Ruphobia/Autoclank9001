#include "gravity_log_curve_fitter.hpp"

namespace gravity_log_curve_fitter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Gravity Log & Curve Fitter (Cooking, brewing, fermentation, food science). Awaits wire-up.";
    return s;
}

}
