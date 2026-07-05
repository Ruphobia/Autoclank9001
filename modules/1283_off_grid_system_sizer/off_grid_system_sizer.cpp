#include "off_grid_system_sizer.hpp"

namespace off_grid_system_sizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Off-Grid System Sizer (Energy systems). Awaits wire-up.";
    return s;
}

}
