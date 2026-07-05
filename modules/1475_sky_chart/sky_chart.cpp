#include "sky_chart.hpp"

namespace sky_chart {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sky Chart (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
