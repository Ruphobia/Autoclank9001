#include "seismic_trace_viewer.hpp"

namespace seismic_trace_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Seismic Trace Viewer (Geology, GIS, and earth sciences). Awaits wire-up.";
    return s;
}

}
