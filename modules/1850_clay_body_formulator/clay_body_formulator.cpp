#include "clay_body_formulator.hpp"

namespace clay_body_formulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Clay Body Formulator (Traditional crafts: horology, blacksmithing, jewelry, glassblowing, pottery, leatherwork, cobbling). Awaits wire-up.";
    return s;
}

}
