#include "heat_exchanger_sizer.hpp"

namespace heat_exchanger_sizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Heat exchanger sizer (Acoustic, thermal, hydraulic, pneumatic engineering). Awaits wire-up.";
    return s;
}

}
