#include "splits_timer.hpp"

namespace splits_timer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Splits Timer (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
