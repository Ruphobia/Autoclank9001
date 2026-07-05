#include "module_i_v_curve_simulator.hpp"

namespace module_i_v_curve_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Module I-V Curve Simulator (Energy systems). Awaits wire-up.";
    return s;
}

}
