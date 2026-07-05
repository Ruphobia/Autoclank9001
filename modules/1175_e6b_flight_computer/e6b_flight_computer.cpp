#include "e6b_flight_computer.hpp"

namespace e6b_flight_computer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: E6B flight computer (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
