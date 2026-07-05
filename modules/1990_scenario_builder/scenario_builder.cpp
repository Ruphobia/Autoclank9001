#include "scenario_builder.hpp"

namespace scenario_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Scenario Builder (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
