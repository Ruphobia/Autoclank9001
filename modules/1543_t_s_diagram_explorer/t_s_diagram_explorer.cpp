#include "t_s_diagram_explorer.hpp"

namespace t_s_diagram_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: T-S diagram explorer (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
