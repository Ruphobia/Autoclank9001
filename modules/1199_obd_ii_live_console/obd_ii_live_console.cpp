#include "obd_ii_live_console.hpp"

namespace obd_ii_live_console {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: OBD-II Live Console (Automotive and vehicle systems). Awaits wire-up.";
    return s;
}

}
