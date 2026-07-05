#include "lighting_diagram_designer.hpp"

namespace lighting_diagram_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lighting Diagram Designer (Photography and cinematography). Awaits wire-up.";
    return s;
}

}
