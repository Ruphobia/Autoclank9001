#include "radio_link_planner.hpp"

namespace radio_link_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Radio link planner (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
