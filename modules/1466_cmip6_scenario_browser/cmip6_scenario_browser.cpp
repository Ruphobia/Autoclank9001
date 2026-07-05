#include "cmip6_scenario_browser.hpp"

namespace cmip6_scenario_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CMIP6 scenario browser (Climate, weather, and atmospheric science). Awaits wire-up.";
    return s;
}

}
