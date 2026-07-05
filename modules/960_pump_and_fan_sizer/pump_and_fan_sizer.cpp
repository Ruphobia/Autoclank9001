#include "pump_and_fan_sizer.hpp"

namespace pump_and_fan_sizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pump and fan sizer (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
