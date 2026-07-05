#include "alloy_composer.hpp"

namespace alloy_composer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Alloy Composer (Traditional crafts: horology, blacksmithing, jewelry, glassblowing, pottery, leatherwork, cobbling). Awaits wire-up.";
    return s;
}

}
