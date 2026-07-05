#include "cp_plotter.hpp"

namespace cp_plotter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cp Plotter (Aerodynamics and CFD). Awaits wire-up.";
    return s;
}

}
