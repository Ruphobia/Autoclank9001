#include "nexrad_level_ii_viewer.hpp"

namespace nexrad_level_ii_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: NEXRAD Level II viewer (Climate, weather, and atmospheric science). Awaits wire-up.";
    return s;
}

}
