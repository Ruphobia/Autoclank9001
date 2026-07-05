#include "npsh_and_cavitation_checker.hpp"

namespace npsh_and_cavitation_checker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: NPSH and cavitation checker (Acoustic, thermal, hydraulic, pneumatic engineering). Awaits wire-up.";
    return s;
}

}
