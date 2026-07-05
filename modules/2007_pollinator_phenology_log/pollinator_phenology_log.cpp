#include "pollinator_phenology_log.hpp"

namespace pollinator_phenology_log {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pollinator phenology log (Hobbies: gardening, beekeeping, aquariums, pets). Awaits wire-up.";
    return s;
}

}
