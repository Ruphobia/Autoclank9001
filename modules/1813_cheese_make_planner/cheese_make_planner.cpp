#include "cheese_make_planner.hpp"

namespace cheese_make_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cheese Make Planner (Cooking, brewing, fermentation, food science). Awaits wire-up.";
    return s;
}

}
