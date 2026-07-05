#include "light_curve_analyzer.hpp"

namespace light_curve_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Light Curve Analyzer (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
