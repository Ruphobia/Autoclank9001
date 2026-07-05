#include "string_inverter_sizing.hpp"

namespace string_inverter_sizing {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: String & Inverter Sizing (Energy systems). Awaits wire-up.";
    return s;
}

}
