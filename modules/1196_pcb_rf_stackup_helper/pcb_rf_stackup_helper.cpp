#include "pcb_rf_stackup_helper.hpp"

namespace pcb_rf_stackup_helper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: PCB RF stackup helper (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
