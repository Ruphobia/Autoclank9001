#include "borehole_log_viewer.hpp"

namespace borehole_log_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Borehole Log Viewer (Geology, GIS, and earth sciences). Awaits wire-up.";
    return s;
}

}
