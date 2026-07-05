#include "ev_pack_and_range_calculator.hpp"

namespace ev_pack_and_range_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: EV Pack and Range Calculator (Automotive and vehicle systems). Awaits wire-up.";
    return s;
}

}
