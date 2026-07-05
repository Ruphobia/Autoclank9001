#include "ethics_scenario_explorer.hpp"

namespace ethics_scenario_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ethics scenario explorer (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
