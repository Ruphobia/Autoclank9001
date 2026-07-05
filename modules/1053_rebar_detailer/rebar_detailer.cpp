#include "rebar_detailer.hpp"

namespace rebar_detailer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rebar Detailer (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
