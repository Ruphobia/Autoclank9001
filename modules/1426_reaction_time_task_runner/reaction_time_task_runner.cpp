#include "reaction_time_task_runner.hpp"

namespace reaction_time_task_runner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Reaction time task runner (Psychology and behavioral science). Awaits wire-up.";
    return s;
}

}
