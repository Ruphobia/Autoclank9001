#include "light_pollution_map.hpp"

namespace light_pollution_map {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Light Pollution Map (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
