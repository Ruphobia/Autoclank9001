#include "er_diagram_designer.hpp"

namespace er_diagram_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ER diagram designer (Databases). Awaits wire-up.";
    return s;
}

}
