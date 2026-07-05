#include "colregs_and_lights_advisor.hpp"

namespace colregs_and_lights_advisor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: COLREGs and lights advisor (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
