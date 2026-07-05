#include "foundation_sizer.hpp"

namespace foundation_sizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Foundation Sizer (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
