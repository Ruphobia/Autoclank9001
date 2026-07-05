#include "nec_antenna_designer.hpp"

namespace nec_antenna_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: NEC antenna designer (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
