#include "vintage_serial_terminal.hpp"

namespace vintage_serial_terminal {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Vintage Serial Terminal (Vintage hardware preservation and retrocomputing). Awaits wire-up.";
    return s;
}

}
