#include "thermal_performance_modeler.hpp"

namespace thermal_performance_modeler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Thermal Performance Modeler (Energy systems). Awaits wire-up.";
    return s;
}

}
