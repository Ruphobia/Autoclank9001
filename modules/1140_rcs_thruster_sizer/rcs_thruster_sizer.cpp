#include "rcs_thruster_sizer.hpp"

namespace rcs_thruster_sizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: RCS & Thruster Sizer (Aerospace and orbital mechanics). Awaits wire-up.";
    return s;
}

}
