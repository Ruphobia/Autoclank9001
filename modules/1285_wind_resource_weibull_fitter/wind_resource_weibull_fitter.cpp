#include "wind_resource_weibull_fitter.hpp"

namespace wind_resource_weibull_fitter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Wind Resource & Weibull Fitter (Energy systems). Awaits wire-up.";
    return s;
}

}
