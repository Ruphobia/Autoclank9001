#include "growing_degree_day_tracker.hpp"

namespace growing_degree_day_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Growing Degree Day Tracker (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
