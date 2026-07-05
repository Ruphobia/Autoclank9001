#include "sourdough_schedule_builder.hpp"

namespace sourdough_schedule_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sourdough Schedule Builder (Cooking, brewing, fermentation, food science). Awaits wire-up.";
    return s;
}

}
