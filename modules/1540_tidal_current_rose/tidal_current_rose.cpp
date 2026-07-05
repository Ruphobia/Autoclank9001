#include "tidal_current_rose.hpp"

namespace tidal_current_rose {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Tidal current rose (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
