#include "pre_registration_drafter.hpp"

namespace pre_registration_drafter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pre-registration drafter (Psychology and behavioral science). Awaits wire-up.";
    return s;
}

}
