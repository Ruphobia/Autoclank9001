#include "meta_tracker.hpp"

namespace meta_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Meta Tracker (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
