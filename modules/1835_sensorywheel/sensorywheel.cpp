#include "sensorywheel.hpp"

namespace sensorywheel {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: SensoryWheel (Coffee, tea, wine, spirits, beverages). Awaits wire-up.";
    return s;
}

}
