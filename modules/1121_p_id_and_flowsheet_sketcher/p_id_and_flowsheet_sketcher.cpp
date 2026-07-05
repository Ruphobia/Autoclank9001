#include "p_id_and_flowsheet_sketcher.hpp"

namespace p_id_and_flowsheet_sketcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: P&ID and flowsheet sketcher (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
