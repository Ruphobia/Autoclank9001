#include "capacitor_atlas.hpp"

namespace capacitor_atlas {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Capacitor Atlas (Vintage hardware preservation and retrocomputing). Awaits wire-up.";
    return s;
}

}
