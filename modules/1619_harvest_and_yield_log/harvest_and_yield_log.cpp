#include "harvest_and_yield_log.hpp"

namespace harvest_and_yield_log {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Harvest and Yield Log (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
