#include "adif_logbook.hpp"

namespace adif_logbook {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ADIF logbook (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
