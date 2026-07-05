#include "initiative_tracker.hpp"

namespace initiative_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Initiative Tracker (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
