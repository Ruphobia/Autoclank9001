#include "connectivity_modeler.hpp"

namespace connectivity_modeler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Connectivity modeler (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
