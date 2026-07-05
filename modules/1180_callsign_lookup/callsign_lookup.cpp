#include "callsign_lookup.hpp"

namespace callsign_lookup {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Callsign lookup (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
