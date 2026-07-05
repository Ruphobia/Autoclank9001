#include "sanger_trace_viewer.hpp"

namespace sanger_trace_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sanger Trace Viewer (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
