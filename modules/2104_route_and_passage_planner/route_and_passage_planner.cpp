#include "route_and_passage_planner.hpp"

namespace route_and_passage_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Route and passage planner (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
