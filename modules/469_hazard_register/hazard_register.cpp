#include "hazard_register.hpp"

namespace hazard_register {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hazard register (Quality, safety, risk, and compliance). Awaits wire-up.";
    return s;
}

}
