#include "chart_viewer.hpp"

namespace chart_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Chart viewer (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
