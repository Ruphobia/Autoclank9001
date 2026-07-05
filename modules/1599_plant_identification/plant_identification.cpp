#include "plant_identification.hpp"

namespace plant_identification {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Plant identification (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
