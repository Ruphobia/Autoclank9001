#include "teasessionplanner.hpp"

namespace teasessionplanner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: TeaSessionPlanner (Coffee, tea, wine, spirits, beverages). Awaits wire-up.";
    return s;
}

}
