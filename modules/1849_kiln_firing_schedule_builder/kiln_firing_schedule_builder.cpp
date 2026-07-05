#include "kiln_firing_schedule_builder.hpp"

namespace kiln_firing_schedule_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Kiln Firing Schedule Builder (Traditional crafts: horology, blacksmithing, jewelry, glassblowing, pottery, leatherwork, cobbling). Awaits wire-up.";
    return s;
}

}
