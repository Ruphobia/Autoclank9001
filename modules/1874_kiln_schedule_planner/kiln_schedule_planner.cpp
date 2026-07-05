#include "kiln_schedule_planner.hpp"

namespace kiln_schedule_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Kiln schedule planner (Visual arts: calligraphy, painting, sculpture). Awaits wire-up.";
    return s;
}

}
