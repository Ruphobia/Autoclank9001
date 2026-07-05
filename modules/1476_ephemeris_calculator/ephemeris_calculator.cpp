#include "ephemeris_calculator.hpp"

namespace ephemeris_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ephemeris Calculator (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
