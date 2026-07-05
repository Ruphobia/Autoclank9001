#include "coral_dhw_tracker.hpp"

namespace coral_dhw_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Coral DHW tracker (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
