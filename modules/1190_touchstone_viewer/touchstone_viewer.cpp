#include "touchstone_viewer.hpp"

namespace touchstone_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Touchstone viewer (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
