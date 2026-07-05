#include "panel_method_studio.hpp"

namespace panel_method_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Panel Method Studio (Aerodynamics and CFD). Awaits wire-up.";
    return s;
}

}
