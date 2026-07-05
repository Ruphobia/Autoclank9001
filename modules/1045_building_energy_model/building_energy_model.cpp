#include "building_energy_model.hpp"

namespace building_energy_model {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Building Energy Model (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
