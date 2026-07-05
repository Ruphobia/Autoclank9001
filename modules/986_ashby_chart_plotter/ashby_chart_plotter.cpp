#include "ashby_chart_plotter.hpp"

namespace ashby_chart_plotter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ashby Chart Plotter (Materials science and metallurgy). Awaits wire-up.";
    return s;
}

}
