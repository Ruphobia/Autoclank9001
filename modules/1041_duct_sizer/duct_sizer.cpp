#include "duct_sizer.hpp"

namespace duct_sizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Duct Sizer (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
