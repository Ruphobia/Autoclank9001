#include "fluid_properties_database.hpp"

namespace fluid_properties_database {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Fluid properties database (Acoustic, thermal, hydraulic, pneumatic engineering). Awaits wire-up.";
    return s;
}

}
