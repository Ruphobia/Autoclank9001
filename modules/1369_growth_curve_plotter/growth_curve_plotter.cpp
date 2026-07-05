#include "growth_curve_plotter.hpp"

namespace growth_curve_plotter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Growth curve plotter (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
