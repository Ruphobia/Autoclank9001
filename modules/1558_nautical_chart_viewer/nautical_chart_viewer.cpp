#include "nautical_chart_viewer.hpp"

namespace nautical_chart_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Nautical chart viewer (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
