#include "aircraft_logbook.hpp"

namespace aircraft_logbook {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Aircraft logbook (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
