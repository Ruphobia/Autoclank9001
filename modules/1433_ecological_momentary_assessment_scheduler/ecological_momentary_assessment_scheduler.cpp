#include "ecological_momentary_assessment_scheduler.hpp"

namespace ecological_momentary_assessment_scheduler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ecological momentary assessment scheduler (Psychology and behavioral science). Awaits wire-up.";
    return s;
}

}
