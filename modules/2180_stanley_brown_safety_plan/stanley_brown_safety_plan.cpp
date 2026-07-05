#include "stanley_brown_safety_plan.hpp"

namespace stanley_brown_safety_plan {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Stanley-Brown safety plan (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
