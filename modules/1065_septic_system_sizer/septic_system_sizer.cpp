#include "septic_system_sizer.hpp"

namespace septic_system_sizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Septic system sizer (Civil utility (water, waste, sanitation)). Awaits wire-up.";
    return s;
}

}
