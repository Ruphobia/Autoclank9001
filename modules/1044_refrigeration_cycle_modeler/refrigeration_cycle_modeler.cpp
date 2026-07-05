#include "refrigeration_cycle_modeler.hpp"

namespace refrigeration_cycle_modeler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Refrigeration Cycle Modeler (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
