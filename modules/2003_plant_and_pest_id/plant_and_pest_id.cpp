#include "plant_and_pest_id.hpp"

namespace plant_and_pest_id {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Plant and pest ID (Hobbies: gardening, beekeeping, aquariums, pets). Awaits wire-up.";
    return s;
}

}
