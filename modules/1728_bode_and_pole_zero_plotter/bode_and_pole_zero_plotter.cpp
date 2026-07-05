#include "bode_and_pole_zero_plotter.hpp"

namespace bode_and_pole_zero_plotter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bode and pole-zero plotter (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
