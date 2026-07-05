#include "retrosynthesis_planner.hpp"

namespace retrosynthesis_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Retrosynthesis planner (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
