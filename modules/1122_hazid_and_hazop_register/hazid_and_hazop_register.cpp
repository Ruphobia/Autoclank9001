#include "hazid_and_hazop_register.hpp"

namespace hazid_and_hazop_register {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HAZID and HAZOP register (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
