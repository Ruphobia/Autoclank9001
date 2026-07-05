#include "thermal_simulator.hpp"

namespace thermal_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Thermal simulator (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
