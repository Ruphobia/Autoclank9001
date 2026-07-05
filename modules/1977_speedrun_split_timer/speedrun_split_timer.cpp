#include "speedrun_split_timer.hpp"

namespace speedrun_split_timer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Speedrun split timer (Game playing assistants). Awaits wire-up.";
    return s;
}

}
