#include "planting_calendar.hpp"

namespace planting_calendar {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Planting Calendar (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
