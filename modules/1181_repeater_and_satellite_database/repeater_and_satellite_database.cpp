#include "repeater_and_satellite_database.hpp"

namespace repeater_and_satellite_database {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Repeater and satellite database (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
