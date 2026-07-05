#include "hunt_and_catch_log.hpp"

namespace hunt_and_catch_log {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hunt and Catch Log (Hunting, fishing, trapping, wildlife tracking). Awaits wire-up.";
    return s;
}

}
