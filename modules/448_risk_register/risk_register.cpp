#include "risk_register.hpp"

namespace risk_register {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Risk register (Project management). Awaits wire-up.";
    return s;
}

}
