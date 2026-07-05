#include "battery_bank_sizer.hpp"

namespace battery_bank_sizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Battery Bank Sizer (Energy systems). Awaits wire-up.";
    return s;
}

}
