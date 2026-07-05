#include "gnu_radio_flowgraph_runner.hpp"

namespace gnu_radio_flowgraph_runner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: GNU Radio flowgraph runner (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
