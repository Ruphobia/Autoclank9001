#include "brick_wall_research_planner.hpp"

namespace brick_wall_research_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Brick Wall Research Planner (Detailed genealogy and family history). Awaits wire-up.";
    return s;
}

}
