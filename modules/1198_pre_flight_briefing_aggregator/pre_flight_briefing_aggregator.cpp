#include "pre_flight_briefing_aggregator.hpp"

namespace pre_flight_briefing_aggregator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pre-flight briefing aggregator (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
