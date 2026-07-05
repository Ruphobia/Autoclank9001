#include "koji_and_miso_schedule.hpp"

namespace koji_and_miso_schedule {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Koji and Miso Schedule (Cooking, brewing, fermentation, food science). Awaits wire-up.";
    return s;
}

}
