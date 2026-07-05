#include "polymer_property_tables.hpp"

namespace polymer_property_tables {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Polymer Property Tables (Materials science and metallurgy). Awaits wire-up.";
    return s;
}

}
