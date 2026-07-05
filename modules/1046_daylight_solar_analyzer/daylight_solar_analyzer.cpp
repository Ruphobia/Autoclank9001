#include "daylight_solar_analyzer.hpp"

namespace daylight_solar_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Daylight & Solar Analyzer (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
