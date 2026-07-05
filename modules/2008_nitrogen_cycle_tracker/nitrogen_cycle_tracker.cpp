#include "nitrogen_cycle_tracker.hpp"

namespace nitrogen_cycle_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Nitrogen cycle tracker (Hobbies: gardening, beekeeping, aquariums, pets). Awaits wire-up.";
    return s;
}

}
