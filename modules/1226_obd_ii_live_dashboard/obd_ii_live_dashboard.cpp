#include "obd_ii_live_dashboard.hpp"

namespace obd_ii_live_dashboard {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: OBD-II Live Dashboard (Automotive and vehicle systems). Awaits wire-up.";
    return s;
}

}
