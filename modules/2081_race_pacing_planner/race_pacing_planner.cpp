#include "race_pacing_planner.hpp"

namespace race_pacing_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Race pacing planner (Sports analytics, fitness, nutrition, coaching). Awaits wire-up.";
    return s;
}

}
