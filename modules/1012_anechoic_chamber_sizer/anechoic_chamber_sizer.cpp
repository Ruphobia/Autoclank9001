#include "anechoic_chamber_sizer.hpp"

namespace anechoic_chamber_sizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Anechoic chamber sizer (Acoustic, thermal, hydraulic, pneumatic engineering). Awaits wire-up.";
    return s;
}

}
