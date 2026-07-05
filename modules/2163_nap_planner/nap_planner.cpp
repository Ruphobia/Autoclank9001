#include "nap_planner.hpp"

namespace nap_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Nap Planner (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
