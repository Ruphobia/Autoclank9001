#include "flow_visualizer.hpp"

namespace flow_visualizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Flow Visualizer (Aerodynamics and CFD). Awaits wire-up.";
    return s;
}

}
