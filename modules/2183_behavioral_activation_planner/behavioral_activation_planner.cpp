#include "behavioral_activation_planner.hpp"

namespace behavioral_activation_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Behavioral activation planner (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
